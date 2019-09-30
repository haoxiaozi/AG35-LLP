# Makefile for QuecOpen SDK
#
# Copyright (C) 2017 Quectel
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

TOPDIR:=$(shell pwd)
ABOOT_DIR:=$(TOPDIR)/ql-ol-bootloader
KERNEL_DIR:=$(TOPDIR)/ql-ol-kernel
ROOTFS_DIR:=$(TOPDIR)/ql-ol-rootfs
EXTSDK_DIR:=$(TOPDIR)/ql-ol-extsdk
#export TOPDIR

empty:=
space:= $(empty) $(empty)
$(if $(findstring $(space),$(TOPDIR)),$(error ERROR: The path to the work directory must not include any spaces))

crosstool=crosstool
aboot=aboot
kernel_menuconfig=kernel_menuconfig
debug_kernel_menuconfig=debug_kernel_menuconfig
kernel=kernel
kernel_module=kernel_module
rootfs=rootfs
squashrootfs=squashrootfs
usrdata=usrdata
extsdk=extsdk
debug_version=debug_version
#single=$(shell ${#1##*/})
ABOOT_MAKE_FLAGS_EXTRA:=
ubicfg-fname:=ubinize_system_userdata.cfg
sysfs-ubi-fname:=mdm9607-perf-sysfs.ubi
has_usrfs:=false
boot-img-fname:=mdm9607-perf-boot.img

ifneq ($(filter $(QUECTEL_PROJECT_NAME), AG35C AG35CE AG35E AG35CEN AG35CEVBM AG35NA AG35J AG35LA), )
	ABOOT_MAKE_FLAGS_EXTRA+=TOOLCHAIN_PREFIX=arm-oe-linux-gnueabi- LIBGCC=${LIBGCC} SIGNED_KERNEL=1
	ubicfg-fname=ubinize_system_userdata.cfg
	sysfs-ubi-fname=mdm9607-sysfs.ubi
	has_usrfs=true
	ubicfg-fname=ubinize_system_userdata.cfg
else
	ABOOT_MAKE_FLAGS_EXTRA+=TOOLCHAIN_PREFIX=arm-oe-linux-gnueabi- SIGNED_KERNEL=1
	ubicfg-fname=ubinize.cfg
	sysfs-ubi-fname=mdm9607-perf-sysfs.ubi

endif

#$(shell source ql-ol-crosstool/ql-ol-crosstool-env-init)
$(shell mkdir -p target)

world : $(crosstool) $(kernel) $(kernel_module) $(aboot) $(rootfs) $(usrdata)

$(crosstool):
ifneq ($(CROSSTOOL_FLAG), 1)
	@echo -e "\033[31m###### Please exec cmd to init crosstool: source ql-ol-crosstool/ql-ol-crosstool-env-init #######\033[0m"
	@exit 1
else
	@echo -e "\033[32m########## crosstool have inited previously #########\033[0m"
endif

$(aboot):
	cd $(ABOOT_DIR) ; make -j 4 mdm9607 $(ABOOT_MAKE_FLAGS_EXTRA) || exit ; \
	cp build-mdm9607/appsboot.mbn $(TOPDIR)/target

$(kernel_menuconfig):
	cd $(KERNEL_DIR) ; make ARCH=arm mdm9607-perf_defconfig menuconfig O=build
	cp $(KERNEL_DIR)/build/.config $(KERNEL_DIR)/arch/arm/configs/mdm9607-perf_defconfig

$(debug_kernel_menuconfig):
	cd $(KERNEL_DIR) ; make ARCH=arm mdm9607_defconfig menuconfig O=build
	cp $(KERNEL_DIR)/build/.config $(KERNEL_DIR)/arch/arm/configs/mdm9607_defconfig

$(kernel):
	cd $(KERNEL_DIR) ; [ ! -f build/.config ] && echo -e "\033[31m.config is nonexist, Please exec \"make kernel_menuconfig\" \033[0m" && exit 1 ;\
	make ARCH=arm CC=arm-oe-linux-gnueabi-gcc LD=arm-oe-linux-gnueabi-ld.bfd -j 4 O=build || exit ; \
	cp build/arch/arm/boot/zImage build/arch/arm/boot/dts/qcom/mdm9607-mtp.dtb $(EXTSDK_DIR)/tools/quectel_mkboot/
	cd $(EXTSDK_DIR)/tools/quectel_mkboot ; chmod +x * ;  \
        ./quec_mkboot dtb2img mdm9607-mtp.dtb zImage   ; \
	cp ./target/* $(TOPDIR)/target

$(kernel_module):
	cd $(KERNEL_DIR) ; [ ! -f build/.config ] && echo -e "\033[31m.config is nonexist, Please exec \"make kernel_menuconfig\" \033[0m" && exit 1; \
	make modules ARCH=arm CC=arm-oe-linux-gnueabi-gcc LD=arm-oe-linux-gnueabi-ld.bfd -j 4 O=build || exit ; \
	make ARCH=arm CROSS_COMPILE=arm-oe-linux-gnueabi- INSTALL_MOD_STRIP=1 O=$(KERNEL_DIR)/build INSTALL_MOD_PATH=$(ROOTFS_DIR)/usr  modules_install || exit
	echo -e "\033[32m###### kernel module compile completely, need make rootfs to generate sysfs.ubi again! ######\033[0m"

ifeq ($(QL_FAKEROOT), 1)
FAKEROOT=ql_fakeroot
else
FAKEROOT=fakeroot
endif

$(rootfs):
	cd $(TOPDIR) ; chmod +x ./ql-ol-extsdk/tools/quectel_ubi/* ; \
	$(FAKEROOT) ./ql-ol-extsdk/tools/quectel_ubi/mkfs.ubifs -r ql-ol-rootfs -o mdm9607-perf-sysfs.ubifs ${MKUBIFS_ARGS}
ifeq ($(has_usrfs),true)
	$(FAKEROOT) ./ql-ol-extsdk/tools/quectel_ubi/mkfs.ubifs -r ql-ol-usrfs -o mdm9607-usrfs.ubifs ${MKUBIFS_ARGS}
endif
	./ql-ol-extsdk/tools/quectel_ubi/ubinize  -o $(sysfs-ubi-fname) ${UBINIZE_ARGS} ql-ol-extsdk/tools/quectel_ubi/$(ubicfg-fname)
	rm -rf mdm9607*.ubifs
	mv $(sysfs-ubi-fname) target/

$(usrdata):
	cd $(TOPDIR) ; chmod +x ./ql-ol-extsdk/tools/quectel_ubi/* ; $(FAKEROOT) ./ql-ol-extsdk/tools/quectel_ubi/mkfs.ubifs -r ql-ol-usrdata -o usrdata.ubifs ${MKUBIFS_ARGS} ;	\
	./ql-ol-extsdk/tools/quectel_ubi/ubinize  -o usrdata.ubi ${UBINIZE_ARGS} ql-ol-extsdk/tools/quectel_ubi/ubinize_usrdata.cfg ; \
	rm -rf usrdata.ubifs
	mv usrdata.ubi target/

$(debug_version): kernel kernel_module rootfs

$(extsdk):
	cd $(EXTSDK_DIR)/example ; make clean || exit ; make || exit

clean: aboot/clean kernel/clean rootfs/clean extsdk/clean usrdata/clean

aboot/clean:
	cd $(ABOOT_DIR) ; make -j 4 mdm9607 clean ; rm -rf build-mdm9607
	rm -rf target/appsboot.mbn

kernel/clean:
	cd $(KERNEL_DIR) ; make clean O=build || exit
	rm -rf target/*.img

kernel/distclean:
	cd $(KERNEL_DIR) ; make distclean O=build || exit
	rm -rf target/*.img

rootfs/clean:
	rm -rf target/mdm9607-perf-sysfs.ubi*

usrdata/clean:
	rm -rf target/usrdata.ubi*

extsdk/clean:
	cd $(EXTSDK_DIR)/example ; make clean || exit

