#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include "sms_suite.h"
#include "nw_suite.h"
#include "testconf.h"


static sms_client_handle_type h_sms;

//#define TEST_SDK_DEST_PHONE_NUMBER  "18054099355"

static int sms_send_flag = 0;
#define SMS_REPLY_BUFF_LEN  (128)
static char sms_reply_buff[SMS_REPLY_BUFF_LEN] = {0};
static char sms_cent_num[11+1] = {'\0'};

extern char g_dest_phone_number[32];
extern int g_reply_sms_times;
extern int g_verbose;
extern int g_interval;


int sms_suite_init()
{
	int ret;
	ret = QL_SMS_Client_Init(&h_sms);
	if(ret)
	{
		printErrMsg("QL_SMS_Client_Init failed.\n");
		return -1;
	}
	return 0;
}


int sms_suite_clean()
{
	int ret;

	ret = QL_SMS_Client_Deinit(h_sms);
	if(ret)
	{
		printDebugFormatMsg("%s: Client Deinit failed.\n", __FUNCTION__);
		return -1;
	}
	return 0;
}

void sms_suite_SetUp()
{
	return;
}

void sms_suite_TearDown()
{
	return;
}


#define MAX_CHARACTER_SIZE    8
unsigned char* UnicodeToUTF8( int unicode, unsigned char *p)
{
    unsigned char *e = NULL;
    if((e = p))
    {
        if(unicode < 0x80)
        {
            *e++ = unicode;
        }
        else if(unicode < 0x800)
        {
            *e++ = ((unicode >> 6) & 0x1f)|0xc0;
            *e++ = (unicode & 0x3f)|0x80;
        }
        else if(unicode < 0x10000)
        {
            *e++ = ((unicode >> 12) & 0x0f)|0xe0;
            *e++ = ((unicode >> 6) & 0x3f)|0x80;
            *e++ = (unicode & 0x3f)|0x80;
        }
        else if(unicode < 0x200000)
        {
            *e++ = ((unicode >> 18) & 0x07)|0xf0;
            *e++ = ((unicode >> 12) & 0x3f)|0x80;
            *e++ = ((unicode >> 6) & 0x3f)|0x80;
            *e++ = (unicode & 0x3f)|0x80;
        }
        else if(unicode < 0x4000000)
        {
            *e++ = ((unicode >> 24) & 0x03)|0xf8 ;
            *e++ = ((unicode >> 18) & 0x3f)|0x80;
            *e++ = ((unicode >> 12) & 0x3f)|0x80;
            *e++ = ((unicode >> 6) & 0x3f)|0x80;
            *e++ = (unicode & 0x3f)|0x80;
        }
        else
        {
            *e++ = ((unicode >> 30) & 0x01)|0xfc;
            *e++ = ((unicode >> 24) & 0x3f)|0x80;
            *e++ = ((unicode >> 18) & 0x3f)|0x80;
            *e++ = ((unicode >> 12) & 0x3f)|0x80;
            *e++ = ((unicode >> 6) & 0x3f)|0x80;
            *e++ = (unicode & 0x3f)|0x80;
        }
    }

    return e;
}

int UTF8ToUnicode (unsigned char *ch, int *unicode)
{
    unsigned char *p = NULL;
    int e = 0, n = 0;
    if((p = ch) && unicode)
    {
        if(*p >= 0xfc)
        {
            e = (p[0] & 0x01) << 30;
            e |= (p[1] & 0x3f) << 24;
            e |= (p[2] & 0x3f) << 18;
            e |= (p[3] & 0x3f) << 12;
            e |= (p[4] & 0x3f) << 6;
            e |= (p[5] & 0x3f);
            n = 6;
        }
        else if(*p >= 0xf8)
        {
            e = (p[0] & 0x03) << 24;
            e |= (p[1] & 0x3f) << 18;
            e |= (p[2] & 0x3f) << 12;
            e |= (p[3] & 0x3f) << 6;
            e |= (p[4] & 0x3f);
            n = 5;
        }
        else if(*p >= 0xf0)
        {
            e = (p[0] & 0x07) << 18;
            e |= (p[1] & 0x3f) << 12;
            e |= (p[2] & 0x3f) << 6;
            e |= (p[3] & 0x3f);
            n = 4;
        }
        else if(*p >= 0xe0)
        {
            e = (p[0] & 0x0f) << 12;
            e |= (p[1] & 0x3f) << 6;
            e |= (p[2] & 0x3f);
            n = 3;
        }
        else if(*p >= 0xc0)
        {
            e = (p[0] & 0x1f) << 6;
            e |= (p[1] & 0x3f);
            n = 2;
        }
        else
        {
            e = p[0];
            n = 1;
        }
        *unicode = e;
    }

    return n;
}

unsigned char* UnicodeStrToUTF8Str (unsigned short *unicode_str,
                                    unsigned char   *utf8_str,
                                    int             utf8_str_size)
{
    int unicode = 0;
    unsigned char *e = NULL, *s = NULL;
    unsigned char utf8_ch[MAX_CHARACTER_SIZE];
    s = utf8_str;
    if ((unicode_str) && (s))
    {
        while ((unicode = (int) (*unicode_str++)))
        {
            #if 1//LE
                unicode = ((unicode&0xFF)<<8) | ((unicode&0xFF00) >> 8);  //error
            #else//BE
                unicode = unicode;
            #endif
            memset (utf8_ch, 0, sizeof (utf8_ch));

            if ((e = UnicodeToUTF8 (unicode, utf8_ch)) > utf8_ch)
            {
                *e = '\0';

                if ((s - utf8_str + strlen ((const char *) utf8_ch)) >= utf8_str_size)
                {
                    return s;
                }
                else
                {
                    memcpy (s, utf8_ch, strlen ((const char *) utf8_ch));
                    s += strlen ((const char *) utf8_ch);
                    *s = '\0';
                }
            }
            else
            {
                return s;
            }
        }
    }

    return s;
}

int UTF8StrToUnicodeStr(unsigned char   *utf8_str,
                        uint16_t        *unicode_str,
                        int             unicode_str_size)
{
    int unicode = 0;
    int n = 0;
    int count = 0;
    unsigned char *s = NULL;
    uint16_t *e = NULL;

    s = utf8_str;
    e = unicode_str;

    if ((utf8_str) && (unicode_str))
    {
        while (*s)
        {
            if ((n = UTF8ToUnicode (s, &unicode)) > 0)
            {
                if ((count + 1) >= unicode_str_size)
                {
                    return count;
                }
                else
                {
                #if 0//LE
                    *e = (unsigned short) unicode;
                #else//BE
                    *e = ((unicode&0xFF)<<8) | ((unicode&0xFF00) >> 8);
                #endif
                    e++;
                    *e = 0;
                    s += n;
                    count++;
                }
            }
            else
            {
                return count;
            }
        }
    }

    return count;
}

void phone_to_binstr(char *dest, char *src)
{
    int arr[4];//save bin
    int i=0,j=0,n;
    char *tmp;
    tmp = dest + 18;//offset
    while(src[j] != '\0')
    {
        if(src[j] == '0')
        {
            src[j] = ':'; /* when encode , the 0 in phone number becomes 10,
                             so phonenumber '0' should becomes ':' and ':'-'0' = 10*/
        }
        n = src[j]-'0';
        for (i = 3; i >=0; i--)  // 1 dec -> 4 bit bin
        {
            arr[i] = n % 2;
            n = n / 2;
        }

        for (i = 0; i <= 3; i++)
        {
            sprintf(&tmp[ i + j*4 ],"%d",arr[i]);
        }

        j++;

    }
}

void binstr_to_hexstr(char *dest_hex,char *src,int src_len)
{
    int hex = 0;
    char bin[5] = {'\0'};
    char *tmp = dest_hex;
    printFormatMsg("src=%s, src_len=%d\n", src, src_len);
    while(src_len)
    {
        strncpy(bin,src,4);
        hex = (bin[0]-'0')*8 + (bin[1]-'0')*4 + (bin[2]-'0')*2 + (bin[3]-'0')*1;
        sprintf(tmp,"%x",hex);
        src = src + 4;
        tmp++;
        src_len = src_len - 4;
    }
    //printFormatMsg("dest_hex=%s\n",dest_hex);
}

int sms_invertnumbers(const char* pSrc, char* pDst, int nSrcLength)
{
	int nDstLength;
	char ch;
        int i = 0;

	nDstLength = nSrcLength;
	for(i=0; i<nSrcLength; i+=2)
	{
		ch = *pSrc++;
		*pDst++ = *pSrc++;
		*pDst++ = ch;
	}

	if(nSrcLength & 1)
	{
		*(pDst-2) = 'F';
		nDstLength++;
	}

	*pDst = '\0';

    return nDstLength;
}

void print_hexstr(uint8_t *apdu,uint32_t apdu_len)
{
    int i = 0;
    char apdu_temp[1024] = {0,};
    char *temp = apdu_temp;
    for(i = 0; i < apdu_len; i++)
    {
        temp +=sprintf(temp,"%02x ",apdu[i]);
    }

    printFormatMsg("sms pdu_len = %d , pdu = %s \n",apdu_len,apdu_temp);
}

uint8_t chrtodec(char chr)
{
    uint8_t value=0;

    if((chr>='a')&&(chr<='z'))
            chr=chr-32;

    if((chr>='0')&&(chr<='9'))
            value=chr-48;
    else if((chr>='A')&&(chr<='Z'))
            value=chr-65+10;

    return value;
}

int cal16match(uint8_t n)
{
    uint8_t i=0;
    int value=1;

    for(i=0;i<n;i++)
            value*=16;

    return value;
}

int hexstrto_dec(char *chr)
{
    uint8_t i,chrlen;
    uint8_t sta=0;
    int chrtoint=0;
    int value=0;

    chrlen=strlen(chr);

    if((chr[0] == '0') && ((chr[1]=='x') || (chr[1] == 'X')))
    {
            sta=2;
            chrlen=chrlen-2;
    }

    for(i=0;i<chrlen;i++)
    {
            chrtoint=chrtodec(chr[sta+i]);
            if(i!=(chrlen-1))
                    value+=chrtoint*cal16match(chrlen-1-i);
            else if(i==(chrlen-1))
                    value+=chrtoint;

    }
        return value;
}

bool long_hexstr_to_oct
(
    uint8_t     *pSrc,
    uint32_t    uSrcLen,
    uint8_t     *pDest,
    uint32_t    *pDestLen,
    uint32_t     uDestMaxLen
)
{
   if(pSrc == NULL || pDest == NULL || pDestLen == NULL)
   {
        printFormatMsg("Paramer error\n");
        return false;
   }

   if(uSrcLen % 2 != 0)
   {
        printFormatMsg("Paramer error uSrcLen:%d\n",uSrcLen);
        return false;
   }

   uint8_t *pTmp = pDest;
   int i = 0;
   char cStr[3] = {0,};
   if(uDestMaxLen < (uSrcLen / 2))
   {
        printFormatMsg("Paramer error uDestMaxLen:%d\n",uDestMaxLen);
        return false;
   }

   for(i = 0; i < uSrcLen; i += 2)
   {
        memset(cStr,0x00,sizeof(cStr));
        memcpy(cStr,&pSrc[i],2);
        *pTmp = hexstrto_dec(cStr);
        pTmp++;
   }

   *pDestLen = pTmp - pDest;

   return TRUE;

}

static void ql_sms_cb_func(QL_SMS_MsgRef msgRef, void *contextPtr)
{
	char *p = NULL;
	printDebugFormatMsg("%s: recv msg from %s, content: %s\n", __FUNCTION__, msgRef->src_addr, msgRef->sms_data);
	printFuncPrefix;

	if(memcmp(msgRef->src_addr, "+86", 3) == 0)
	{
		p = msgRef->src_addr + 3;
	}
	else
	{
		p = msgRef->src_addr;
	}
	if(strcmp(p, g_dest_phone_number) == 0)
	{
		sms_send_flag = 1;
		memset(sms_reply_buff, 0, SMS_REPLY_BUFF_LEN);
		//printFormatMsg("\trecv msg from %s, content: %s\n", msgRef->src_addr, msgRef->sms_data);
		if(msgRef->format == E_QL_SMS_FORMAT_UCS2)
		{
	        UnicodeStrToUTF8Str((unsigned short*)(msgRef->sms_data),
	                              (unsigned char*)sms_reply_buff,
	                              SMS_REPLY_BUFF_LEN) ;
		}
		else
		{
			strcpy(sms_reply_buff, msgRef->sms_data);
		}
	}
}

/**
* 1. set sms message recv callback function
* 2. set route list, specified sms restore type ..
* 2. client: send sms message ( eg. magic_number 0xdeaddead)
* 3. server: recv sms message, and reply it with magic_number + 1
* 4. client: recv reply message, check it.
*/

/********************************TEST CASE START******************************/

//QL_SMS_AddRxMsgHandler
static void TEST_QL_SMS_AddRxMsgHandler(void)
{
	int ret;

	printFuncPrefix;
	ret = QL_SMS_AddRxMsgHandler(ql_sms_cb_func, (void*)h_sms);
	CU_ASSERT_EQUAL(ret, MCM_SUCCESS_V01);
	if(ret != MCM_SUCCESS_V01)
	{
		printFormatMsg("QL_SMS_AddRxMsgHandler failed. ret=%d\n", ret);
	}
	return;
}



struct smsdata {
	char buff[128];
	char reply[128];
};

//buff:hello  reply:world
static struct smsdata send_sms_data[] = {
							{"please reply the following msg: qwert", "qwert"},
							{"please reply the following msg: asdfg", "asdfgh"},
							{"please reply the following msg: zxcvb", "zxcvb"},
							{"please reply the following msg: qazws", "qazws"},
							{"please reply the following msg: 12345", "12345"},
						};

//QL_SMS_Send_Sms
static void TEST_QL_SMS_Send_Sms(void)
{
	int ret, interval, len = 0, i;
	int reply_yet;
	E_QL_SMS_FORMAT_T format;
	ql_sms_info_t *pt_sms_info = NULL;

	printFuncPrefix;
	pt_sms_info = (ql_sms_info_t*)malloc(sizeof(ql_sms_info_t));
    if(pt_sms_info == NULL)
    {
        CU_FAIL("TEST_QL_SMS_Send_Sms malloc error.\n");
		return;
    }

	for(i = 0; i < g_reply_sms_times; i++)
	{
		format = i & 0x03;

		memset(pt_sms_info, 0, sizeof(ql_sms_info_t));

		sms_send_flag = 0;

		memset(sms_reply_buff, 0, sizeof(sms_reply_buff));
		pt_sms_info->format = format;

		strncpy(pt_sms_info->src_addr, g_dest_phone_number, sizeof(pt_sms_info->src_addr));

		len = strlen(send_sms_data[i].buff);

		if(format == E_QL_SMS_FORMAT_GSM_7BIT)
		{
			strcpy(pt_sms_info->sms_data, send_sms_data[i].buff);
			pt_sms_info->sms_data_len = len;
		}
		else if(format == E_QL_SMS_FORMAT_BINARY_DATA)
		{
			memcpy(pt_sms_info->sms_data, send_sms_data[i].buff, len + 1);
			pt_sms_info->sms_data_len = len + 1;
		}
		else
		{

			len = UTF8StrToUnicodeStr((unsigned char*)send_sms_data[i].buff,
                                          (uint16_t*)pt_sms_info->sms_data,
                                          len + 1);// Got BE data
            pt_sms_info->sms_data_len = (len + 1) * 2;//in bytes
		}

		ret = QL_SMS_Send_Sms(h_sms, pt_sms_info);
		CU_ASSERT_EQUAL(ret, MCM_SUCCESS_V01);
		if(ret != MCM_SUCCESS_V01)
		{
			printFormatMsg("QL_SMS_Send_Sms failed. ret=%d\n", ret);
			return;
		}

		interval = 0;
		reply_yet = 0;
		do
		{
			//if(interval % 10 == 0)
			//{
			//	printDebugFormatMsg("waiting reply sms[%d].\n", interval);
			//}
			//printf("\b\b\b%02dS", interval);
			//fflush(stdout);
			if(sms_send_flag == 1)
			{
				if(strcmp(sms_reply_buff, send_sms_data[i].reply) != 0)
				{
					CU_FAIL("recv message content error.");
				}
				reply_yet = 1;
				break;
			}
			interval++;
			sleep(1);
		}
		while (interval < g_interval);
		printFormatMsg("waiting reply message: %2dS\n", interval);
		if(reply_yet != 1)
		{
			CU_FAIL("\trecv reply message failed.\n");
		}
	}
	return;
}

//QL_SMS_SetRouteList Skip

//QL_SMS_GetSmsCenterAddress
static void TEST_QL_SMS_GetSmsCenterAddress(void)
{
	int ret, len;
	ql_sms_service_center_cfg_t sca_cfg;

	printFuncPrefix;
	memset(&sca_cfg, 0, sizeof(ql_sms_service_center_cfg_t));
	ret = QL_SMS_GetSmsCenterAddress(h_sms, &sca_cfg);
	CU_ASSERT_EQUAL(ret, MCM_SUCCESS_V01);
	if(ret != MCM_SUCCESS_V01)
	{
		printFormatMsg("QL_SMS_GetSmsCenterAddress failed.ret=%d\n", ret);
		return;
	}

	//CU_ASSERT(strlen(sca_cfg.service_center_addr) > 0);
	len = strlen(sca_cfg.service_center_addr);
	if(len <= 0)
	{
		CU_FAIL("service_center_addr len error.\n");
		printFormatMsg("service_center_addr len error.ret = %d\n", ret);
	}
    else
    {
	memset(sms_cent_num, 0, 12);
	strncpy(sms_cent_num, sca_cfg.service_center_addr +3, 11);
        printFormatMsg("service_center_addr: %s, len: %d, sms_cent_num= %s\n",
                        sca_cfg.service_center_addr, len, sms_cent_num);
    }

	if(sca_cfg.service_center_addr_type_valid)
	{
		//CU_ASSERT(strlen(sca_cfg.service_center_addr_type) > 0);
		if(strlen(sca_cfg.service_center_addr_type) <= 0)
		{
			CU_FAIL("service_center_addr_type error.\n");
		}

		printFormatMsg("service_center_addr_type: %s\n", sca_cfg.service_center_addr_type);
	}
	return;
}

//QL_SMS_SetPreferredStorage skip,not need storage
//QL_SMS_DeleteFromStorage skip ,not need storage


//QL_SMS_SetSmsCenterAddress
static void TEST_QL_SMS_SetSmsCenterAddress(void)
{
	int ret, len;
	ql_sms_service_center_cfg_t sca_cfg;

	printFuncPrefix;
	memset(&sca_cfg, 0, sizeof(ql_sms_service_center_cfg_t));
	ret = QL_SMS_GetSmsCenterAddress(h_sms, &sca_cfg);
	CU_ASSERT_EQUAL(ret, MCM_SUCCESS_V01);
	if(ret != MCM_SUCCESS_V01)
	{
		printFormatMsg("TEST_QL_SMS_SetSmsCenterAddress"
                                               "QL_SMS_GetSmsCenterAddress failed.ret=%d\n", ret);
		return;
	}

	len = strlen(sca_cfg.service_center_addr);
	if(len <= 0)
	{
		CU_FAIL("TEST_QL_SMS_SetSmsCenterAddress service_center_addr len error.\n");
	}

	printFormatMsg("TEST_QL_SMS_SetSmsCenterAddress service_center_addr: %s, len: %d\n",
					sca_cfg.service_center_addr, len);

	if(sca_cfg.service_center_addr_type_valid)
	{
		if(strlen(sca_cfg.service_center_addr_type) <= 0)
		{
			CU_FAIL("TEST_QL_SMS_SetSmsCenterAddress service_center_addr_type error.\n");
		}

		printFormatMsg("TEST_QL_SMS_SetSmsCenterAddress service_center_addr_type: %s\n",
                                                                 sca_cfg.service_center_addr_type);
	}


	ret = QL_SMS_SetSmsCenterAddress(h_sms, &sca_cfg);
	CU_ASSERT_EQUAL(ret, MCM_SUCCESS_V01);
	if(ret != MCM_SUCCESS_V01)
	{
		printFormatMsg("QL_SMS_SetSmsCenterAddress failed.ret=%d\n", ret);
		return;
	}

	return;
}

//QL_SMS_Send_SmsPdu
static void TEST_QL_SMS_Send_SmsPdu(void)
{
    int ret = 0, len1=0, len2=0;
    char aPduMsg[255*2+1];
    char aUnicodeCent[64+1] = {"B01800680065006C006C006FFF0C8FD9662F6D4B8BD577ED4FE1"};//sms content
    char aPhoneNum[14+1];
    char aPhoneNum_CDMA[64+1]={"000000110110000110"};//86 + phone num
    char aPhoneNum_CDMA_Hex[16+1]={0};
    char aCenterNumResver[14+1] = {0};
    int simFormat = 0;
    uint8_t uFormat = -1;/* 0-CDMDA_PP; 6-GW_PP */
    ql_wms_send_raw_message_data_t sWmsRawMsg;
    ql_wms_raw_send_resp_t  sWmsRawResp;


    memset(&sWmsRawMsg,0,sizeof(sWmsRawMsg));
    memset(&sWmsRawResp,0,sizeof(sWmsRawResp));

    simFormat = QL_MCM_NW_GetSIMFormat(); /*0-CDMA  1-GSM*/
    if(simFormat == 0)
    {
        uFormat = 0;
    }
    else if(simFormat == 1)
    {
        uFormat = 6;
    }
    else
    {
        CU_FAIL("Unrecognized SIM Card!!");
        printFormatMsg("Unrecognized SIM Card\n");
        return ;
    }

    if(simFormat && (0 < strlen(sms_cent_num)))//simFormat == 6 ->GSM
    {
		 printFormatMsg("sms_cent_num = %s\n",sms_cent_num);
         len1 = sms_invertnumbers(g_dest_phone_number, aPhoneNum, strlen(g_dest_phone_number));
         len2 = sms_invertnumbers(sms_cent_num, aCenterNumResver, strlen(sms_cent_num));

         printFormatMsg("aPhoneNum = %s\n",aPhoneNum);
         printFormatMsg("aCenterNumResver = %s\n",aCenterNumResver);
         sprintf(aPduMsg,"089168%s11000D9168%s0008%s",aCenterNumResver, aPhoneNum, aUnicodeCent);
         //char aPduMsg[255*2+1] = {"0891683110102105F011000D91687133234251F50008B01800680065006C006C006FFF0C8FD9662F6D4B8BD577ED4FE1"};
         printFormatMsg("aPduMsg = %s,len1=%d,len2=%d \n",aPduMsg,len1,len2);
    }
    else  //simFormat == 0 ->CDMA
    {
        phone_to_binstr(aPhoneNum_CDMA,g_dest_phone_number);
        aPhoneNum_CDMA[62] = '0';
        aPhoneNum_CDMA[63] = '0';
        printFormatMsg("aPhoneNum_CDMA len=%d, aPhoneNum_CDMA_Hex = %s\n", strlen(aPhoneNum_CDMA), aPhoneNum_CDMA_Hex);
        binstr_to_hexstr(aPhoneNum_CDMA_Hex,aPhoneNum_CDMA,strlen(aPhoneNum_CDMA));
        printFormatMsg("aPhoneNum_CDMA len=%d, aPhoneNum_CDMA_Hex = %s\n", strlen(aPhoneNum_CDMA), aPhoneNum_CDMA_Hex);
        sprintf(aPduMsg,"00000210020408%s0601fc081800032000100106102e8cbb366f0306190518133129080100",aPhoneNum_CDMA_Hex);//"036185cccc890554");
        //sprintf(aPduMsg,"%s","00000210020408036185cccc8905540601fc081800032000100106102e8cbb366f0306190518133129080100");
        printFormatMsg("aPduMsg =%s aPduMsgLen=%d\n", aPduMsg, strlen(aPduMsg));
    }

    if(false == long_hexstr_to_oct(aPduMsg,strlen(aPduMsg),sWmsRawMsg.raw_message,&sWmsRawMsg.raw_message_len,sizeof(sWmsRawMsg.raw_message)))
    {
        printFormatMsg(" long_hexstr_to_oct FAIL\n");
        return  ;
    }

    print_hexstr(sWmsRawMsg.raw_message,sWmsRawMsg.raw_message_len);
    sWmsRawMsg.format = uFormat;

    printFormatMsg("Choice FORMAT = %d(0-CDMDA_PP; 6-GW_PP) simFormat=%d(0-CDMDA; 1-GSM) \n", sWmsRawMsg.format, simFormat);

    ret = QL_SMS_Send_SmsPdu(h_sms,&sWmsRawMsg,&sWmsRawResp);
    CU_ASSERT_EQUAL(ret, MCM_SUCCESS_V01);
    if(ret < 0)
    {
        printFormatMsg("QL_SMS_Send_SmsPdu FAIL. ret=%d\n",ret);
    }
    else
    {
        printFormatMsg(" ret=%d, message_id:%d\n", ret, sWmsRawResp.message_id);
    }

    if(sWmsRawResp.cause_code_valid)
    {
       printFormatMsg(" cause_code:%02x\n",sWmsRawResp.cause_code);
    }

    return ;
}



/********************************TEST CASE END********************************/


/*
	Test case array - must be NULL terminated
*/
static CU_TestInfo sms_suite_case[] = {
    {"QL_SMS_AddRxMsgHandler", TEST_QL_SMS_AddRxMsgHandler},
    {"QL_SMS_Send_Sms", TEST_QL_SMS_Send_Sms},
    {"QL_SMS_GetSmsCenterAddress", TEST_QL_SMS_GetSmsCenterAddress},
    {"QL_SMS_SetSmsCenterAddress", TEST_QL_SMS_SetSmsCenterAddress},
    {"QL_SMS_Send_SmsPdu", TEST_QL_SMS_Send_SmsPdu},
    CU_TEST_INFO_NULL,
};


/**
structure:
suite name, init func, cleanup func, SetUp func, TearDown func, testfunc
*/
CU_SuiteInfo sms_suites[] = {
	{"sms", sms_suite_init, sms_suite_clean, sms_suite_SetUp, sms_suite_TearDown, sms_suite_case},
	CU_SUITE_INFO_NULL,
};

