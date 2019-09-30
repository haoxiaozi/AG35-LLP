#ifndef __SDK_TEST_CONF_H__
#define __SDK_TEST_CONF_H__
#include <string.h>
#include<stdio.h>
#include <stdlib.h>
#include"time.h"

#ifndef TEST_IMEI_LENGTH
#define TEST_IMEI_LENGTH  	(15)
#endif

#ifndef TEST_MEID_LENGTH
#define TEST_MEID_LENGTH  	(14)
#endif

#ifndef TEST_ESN_LENGTH
#define TEST_ESN_LENGTH   	(8)
#endif

#ifndef TEST_IMSI_LENGTH
#define TEST_IMSI_LENGTH	(15)
#endif

#ifndef TEST_ICCID_LENGTH
#define TEST_ICCID_LENGTH	(20)
#endif

#ifndef TEST_ERROR_BUFF_LENGTH
#define TEST_ERROR_BUFF_LENGTH	(256)
#endif

/*=================================================================
 *  Defined macros for write log to file *
 *=================================================================*/
/** Result log file path*/
#define RUN_LOG_FILE "/tmp/ql_auto_test_log.txt"
#define RUN_CPU_MEM_USAGE "/tmp/ql_auto_test_cpu_mem_usage.txt"

//#define SDK_TEST_PRINT_FUNCTION_PREFIX
#ifdef SDK_TEST_PRINT_FUNCTION_PREFIX
	#define printFuncPrefix		\
            printf("\n %s\n", __FUNCTION__);
#else
	#define printFuncPrefix
#endif


//#define SDK_TEST_PRINT_ERROR
#ifdef SDK_TEST_PRINT_ERROR
	#define printErrMsg(format, ...)	\
			printf("[%s:%d] "format, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
	#define printErrMsg(format, ...)
#endif


//#define SDK_TEST_PRINT_DEBUG_FORMAT_MSG
#ifdef SDK_TEST_PRINT_DEBUG_FORMAT_MSG
	#define printDebugFormatMsg(format, ...) \
			printf("[%s:%d] "format, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
	#define printDebugFormatMsg(format, ...)
#endif


#define SDK_TEST_PRINT_FORMAT_MSG
#ifdef SDK_TEST_PRINT_FORMAT_MSG
	#define printFormatMsg(format, ...)\
				do {\
					if(g_verbose == 1)\
					{\
                        freopen(RUN_LOG_FILE, "a", stdout);\
			            setbuf(stdout, NULL);\
                        char s[21];\
                        time_t t;\
                        struct tm* ltime;\
                        time(&t);\
                        ltime = localtime(&t);\
                        strftime(s, 21, "%Y-%m-%d %H:%M:%S,", ltime);\
                        fprintf(stdout,"%s [line:%4d] [_%s]"format,s, __LINE__, __func__,##__VA_ARGS__);\
                        fflush(stdout);\
					}\
				}while(0)
#else
	#define printFormatMsg(format, ...)
#endif


#if 0
static void hexdump(char *buff, int len)
{
	int i;
	
	if(len > 0)
	{
		for(i = 0; i < len; i++)
		{
			printf("%02X ", buff[len]);
			if((i + 1) % 16 == 0)
			{
				printf("\n");
			}
		}
		if(i % 16 != 0)
		{
			printf("\n");
		}
	}
}
#endif

#endif //__SDK_TEST_CONF_H__
