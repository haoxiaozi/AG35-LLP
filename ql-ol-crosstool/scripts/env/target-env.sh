export QL_FAKEROOT=1
    
if [[ ! -d "${QL_SDKPATH}/../ql-ol-rootfs" && -f "${QL_SDKPATH}/../ql-ol-rootfs.tar.gz" ]]; then
    printf "Extracting the rootfs..."
    mkdir -p ${QL_SDKPATH}/../ql-ol-rootfs
    ql_fakeroot tar -xzf ${QL_SDKPATH}/../ql-ol-rootfs.tar.gz -C ${QL_SDKPATH}/../ql-ol-rootfs
    echo "Done."
fi

if [ -f "${QL_SDKPATH}/../autogen-env.sh" ];then
    source ${QL_SDKPATH}/../autogen-env.sh
fi