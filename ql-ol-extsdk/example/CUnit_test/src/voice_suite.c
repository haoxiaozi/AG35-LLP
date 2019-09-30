#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include "voice_suite.h"
#include "testconf.h"

static voice_client_handle_type h_voice = 0;
static ql_mcm_voice_call_waiting_service_t service;
static int voice_state_change_flag = 0;
static int voice_state_value = -1 ;
static int voice_call_id = -1;
//static int voice_waiting_interval = 60;

//#define VOICE_DEST_PHONE_NUMBER "18054099355"
extern char g_dest_phone_number[32];
extern int g_verbose ;
extern int g_interval;


int voice_suite_init()
{
    int ret;
    ret = QL_Voice_Call_Client_Init(&h_voice);
    if(ret != MCM_SUCCESS_V01)
    {
        printErrMsg("QL_Voice_Call_Client_Init failed.ret=%d\n", ret);
        return -1;
    }
    printErrMsg("Successful, ret=%d\n", ret);
    return 0;
}


int voice_suite_clean()
{
    int ret;
    ret = QL_Voice_Call_Client_Deinit(h_voice);
    if(ret != MCM_SUCCESS_V01)
    {
        printErrMsg("QL_Voice_Call_Client_Deinit failed.ret=%d\n", ret);
        return -1;
    }
    printErrMsg("Successful, ret=%d\n", ret);
    return 0;
}

void voice_suite_SetUp()
{
	return;
}

void voice_suite_TearDown()
{
	return;
}

static void ql_voice_call_cb_func(int                     call_id,
                                  char*                   phone_num,
                                  E_QL_VOICE_CALL_STATE_T state,
                                  void                    *contextPtr)
{
	//state
	// char *call_state[] = {"INCOMING", "DIALING", "ALERTING", "ACTIVE", "HOLDING", "END", "WAITING"};
	// printFormatMsg("phonenumber: %s, state: %s\n", phone_num, call_state[state]);
	if(strcmp(phone_num, g_dest_phone_number) == 0)
	{
		voice_state_change_flag = 1;
		voice_call_id = call_id;
		voice_state_value = state;
	}
	
}


/********************************TEST CASE START******************************/
//
static void TEST_QL_Voice_Call_Start(void)
{
	int ret, interval = 0;
	int flag = 0;

	voice_state_change_flag = 0;
	voice_state_value = -1;
	voice_call_id = -1;

	printFuncPrefix;
	ret = QL_Voice_Call_Start(h_voice, E_QL_VCALL_EXTERNAL_SLOT_1, g_dest_phone_number, &voice_call_id);
	CU_ASSERT_EQUAL(ret, MCM_SUCCESS_V01);
	if(ret != MCM_SUCCESS_V01)
	{
		printFormatMsg("QL_Voice_Call_Start error.ret=%d\n", ret);
		return ;
	}

	do
	{
		//if(interval % 10 == 0)
		//{
		//	printDebugFormatMsg("waiting %s hung up [%d].\n", g_dest_phone_number, interval);
		//}
		//printf("\b\b\b%2dS", interval);
		fflush(stdout);
		if(voice_state_value == E_QL_MCM_VOICE_CALL_STATE_END)
		{
			flag = 1;
			CU_PASS("QL_Voice_Call_start success!");
			break;
		}
		sleep(1);
		interval++;
	}
	while (interval < g_interval);
	printFormatMsg("waiting %s hung up %2dS\n", g_dest_phone_number, interval);
	if(flag == 0)
	{
		CU_FAIL("\tQL_Voice_Call_Start failed.");
	}
	voice_state_change_flag = -1;
}

static void TEST_QL_Voice_Call_Answer(void)
{
    int ret = 0;
    ret = QL_Voice_Call_Answer(h_voice, voice_call_id);
    CU_ASSERT_EQUAL(ret, MCM_SUCCESS_V01);
    if(ret != MCM_SUCCESS_V01)
    {
    	printFormatMsg(" Failed!voice_call_id=%d, h_voice=%d, ret=%d\n", voice_call_id, h_voice, ret);
    }
    else
    {
    	printFormatMsg(" Successful,voice_call_id=%d, h_voice=%d, ret=%d\n", voice_call_id, h_voice, ret);
    }
    return;
}

static void TEST_QL_Voice_Call_SetAutoAnswer(void)
{
    int ret1 = 0, ret2 = 0;
    E_QL_MCM_VOICE_AUTO_ANSWER_T auto_type = 0;/* 0-enable  1-disable*/
    int answer_time = 5;
    ret1 = QL_Voice_Call_SetAutoAnswer(h_voice, auto_type, answer_time);
    CU_ASSERT_EQUAL(ret1, MCM_SUCCESS_V01);
    auto_type = 1;
    ret2 = QL_Voice_Call_SetAutoAnswer(h_voice, auto_type, answer_time);
    CU_ASSERT_EQUAL(ret2, MCM_SUCCESS_V01);
    if((ret1 != MCM_SUCCESS_V01) || (ret1 != MCM_SUCCESS_V01))
    {
        printFormatMsg("Failed h_voice=%d, ret1=%d, ret2=%d\n", h_voice, ret1, ret2);
    }
    else
    {
        printFormatMsg("Successful h_voice=%d, ret1=%d, ret2=%d\n", h_voice, ret1, ret2);
    }
    return;
}

static void TEST_QL_Voice_Call_Hold(void)
{
    int ret = 0;
    ret = QL_Voice_Call_Hold(h_voice);
    CU_ASSERT_EQUAL(ret, MCM_SUCCESS_V01);
    if(ret != MCM_SUCCESS_V01)
    {
        printFormatMsg("Failed, h_voice=%d, ret=%d\n", h_voice, ret);
    }
    else
    {
        printFormatMsg("Successful, h_voice=%d, ret=%d\n", h_voice, ret);
    }
    return;
}

static void TEST_QL_Voice_Call_UnHold(void)
{
    int ret = 0;
    ret = QL_Voice_Call_UnHold(h_voice);
    CU_ASSERT_EQUAL(ret, MCM_SUCCESS_V01);
    if(ret != MCM_SUCCESS_V01)
    {
        printFormatMsg("Failed, h_voice=%d, ret=%d\n", h_voice, ret);
    }
    else
    {        
        printFormatMsg("Successful, h_voice=%d, ret=%d\n", h_voice, ret);
    }
    return;
}

static void TEST_QL_Voice_Call_Conference(void)
{
    int ret = 0;
    ret = QL_Voice_Call_Conference(h_voice);
    CU_ASSERT_EQUAL(ret, MCM_SUCCESS_V01);
    if(ret != MCM_SUCCESS_V01)
    {
        printFormatMsg("Failed, h_voice=%d, ret=%d\n", h_voice, ret);
    }
    else
    {    
        printFormatMsg("Successful, h_voice=%d, ret=%d\n", h_voice, ret);
    }
    return;
}

static void TEST_QL_Voice_Call_EndConference(void)
{
    int ret1 = 0, ret2 = 0;
	ret1 = QL_Voice_Call_Start(h_voice, E_QL_VCALL_EXTERNAL_SLOT_1, g_dest_phone_number, &voice_call_id);
	CU_ASSERT_EQUAL(ret1, MCM_SUCCESS_V01);

    sleep(3);

    ret2 = QL_Voice_Call_EndConference(h_voice);
    CU_ASSERT_EQUAL(ret2, MCM_SUCCESS_V01);

    if((ret1 != MCM_SUCCESS_V01) || (ret2 != MCM_SUCCESS_V01))
    {
        printFormatMsg("QL_Voice_Call_EndConference failed.ret1=%d, ret2=%d\n", ret1, ret2);
        
    }
    else
    {
        printFormatMsg("Successful, ret1=%d, ret2=%d\n", ret1, ret2);
    }
    return;
}

static void TEST_QL_Voice_Call_CancelDial(void)
{
    int ret1 = 0, ret2 = 0;
    ret1 = QL_Voice_Call_Start(h_voice, E_QL_VCALL_EXTERNAL_SLOT_1, g_dest_phone_number, &voice_call_id);
    CU_ASSERT_EQUAL(ret1, MCM_SUCCESS_V01);	
    sleep(25);
    ret2 = QL_Voice_Call_CancelDial(h_voice);
    CU_ASSERT_EQUAL(ret2, MCM_SUCCESS_V01);
    if(ret2 != MCM_SUCCESS_V01)
    {
        printFormatMsg("QL_Voice_Call_CancelDial failed.ret1=%d,ret2=%d\n", ret1, ret2);
    }
    else
    {
        printFormatMsg("QL_Voice_Call_CancelDial Successful, ret1=%d,ret2=%d\n", ret1, ret2);
    }
    return;
}

static void TEST_QL_Voice_Call_GetCallStatus(void)
{
    int ret = 0;
    int i;
    ql_mcm_voice_calls_state_t t_info = {0};
    char *call_direct[] = { "MO",       "MT",       "??",           "??"};
    char *voice_tech[]  = { "3GPP   ",  "3GPP2  ",  "UNKNOWN",      "UNKNOWN"};
    char *voice_prsnt[] = { "UNKNOWN",  "ALLOWED",  "RESTRICTED",   "PAYPHONE  "};
    char *call_state[]  = { "INCOMING", "DIALING",  "ALERTING",     "ACTIVE", 
                            "HOLD",     "END",      "WAITING",      "UNKNOWN"};
    
    ret = QL_Voice_Call_GetCallStatus(h_voice, -1, &t_info); // -1<0 means all, you can specified it with voice_call_id
    CU_ASSERT_EQUAL(ret, MCM_SUCCESS_V01);    
    if(ret != MCM_SUCCESS_V01)
    {
        printFormatMsg("QL_Voice_Call_GetCallStatus failed.ret=%d\n", ret);
    }
    
    printFormatMsg("ret=%d t_info.calls_len=%d \n", ret,t_info.calls_len);
    for (i = 0; i < t_info.calls_len; i++)
    {
        printFormatMsg("call[%d] call_ID:%d, direction:%s, number:%s, presentation:%s, tech:%s, state:%s, call_end_reason:%d\n",
                        i,
                        t_info.calls[i].call_id,
                        call_direct[t_info.calls[i].direction & 0x03],
                        t_info.calls[i].number,
                        voice_prsnt[t_info.calls[i].number_presentation & 0x03],
                        voice_tech[t_info.calls[i].tech & 0x03],
                        call_state[t_info.calls[i].state & 0x07],
                        t_info.calls[i].call_end_reason);
    }
    
    return;
}

static void TEST_QL_Voice_Call_GetWaitingStatus(void)
{
    int ret = 0;
    memset(&service,0,sizeof(service));
    ret = QL_Voice_Call_GetWaitingStatus(h_voice, &service);
    CU_ASSERT_EQUAL(ret, MCM_SUCCESS_V01);
    if(ret != MCM_SUCCESS_V01)
    {
        printFormatMsg("QL_Voice_Call_GetWaitingStatus failed.ret=%d\n", ret);
    }
    else
    {
        printFormatMsg("Successful ret=%d\n", ret);
    }
    return;
}


static void TEST_QL_Voice_Call_SetWaiting(void)
{
    int ret = 0;
    ret = QL_Voice_Call_SetWaiting(h_voice, service);
    CU_ASSERT_EQUAL(ret, MCM_SUCCESS_V01);
    if(ret != MCM_SUCCESS_V01)
    {
        printFormatMsg("QL_Voice_Call_SetWaiting failed.ret=%d\n", ret);
    }
    else
    {
        printFormatMsg("Successful ret=%d\n", ret);
    }
    return;
}

static void TEST_QL_Voice_Call_End(void)
{
	int ret = 0, interval1 = 0, interval2 = 0;
	int flag = 0;

	printFuncPrefix;
	do
	{
		interval1++;
		//if(interval1 % 10 == 0)
		//{
		//	printDebugFormatMsg("waiting %s Call me [%d].\n", g_dest_phone_number, interval1);
		//}
		//printf("\b\b\b%02dS", interval1);
		if(voice_state_value == E_QL_MCM_VOICE_CALL_STATE_INCOMING && 
			voice_state_change_flag == 1)
		{
			voice_state_change_flag = -1;
			
			ret = QL_Voice_Call_End(h_voice, voice_call_id);
			CU_ASSERT(ret == MCM_SUCCESS_V01);
			
			do
			{
				interval2++;
				if(voice_state_change_flag == 1)
				{
					CU_ASSERT(voice_state_value == E_QL_MCM_VOICE_CALL_STATE_END);
					break;
				}
				sleep(1);	
			}
			while (interval2 < g_interval);
			
			flag = 1;
			break;
		}
		sleep(1);
	}
	while (interval1 < g_interval);
	printFormatMsg("ret=%d,waiting %s Call me %02dS\n", ret, g_dest_phone_number, interval1);
	if(flag == 0)
	{
		CU_FAIL("QL_Voice_Call_End failed.");
	}
	voice_state_change_flag = -1;
	
	return;

}


static void TEST_QL_Voice_Call_AddStateHandler(void)
{
    int ret;

    printFuncPrefix;
    ret = QL_Voice_Call_AddStateHandler(h_voice, ql_voice_call_cb_func, &voice_call_id);
    CU_ASSERT_EQUAL(ret, MCM_SUCCESS_V01);
    if(ret != MCM_SUCCESS_V01)
    {
    	printFormatMsg("QL_Voice_Call_AddStateHandler failed.ret=%d\n", ret);
    }
    else
    {
        printFormatMsg("Successful! h_voice=%d\n", h_voice);
    }
    return;
}
static void TEST_QL_Voice_Call_RemoveStateHandler(void)
{
    int ret;

    ret = QL_Voice_Call_RemoveStateHandler(h_voice);
    CU_ASSERT_EQUAL(ret, MCM_SUCCESS_V01);
    if(ret != MCM_SUCCESS_V01)
    {
    	printFormatMsg("QL_Voice_Call_RemoveStateHandler failed.ret=%d\n", ret);
    }
    else
    {
        printFormatMsg("Successful! ret=%d\n", ret);

    }
    return;
}

/********************************TEST CASE END********************************/

/*
	Test case array - must be NULL terminated
*/
static CU_TestInfo voice_suite_case[] = {
    {"QL_Voice_Call_AddStateHandler", TEST_QL_Voice_Call_AddStateHandler},
    {"QL_Voice_Call_CancelDial", TEST_QL_Voice_Call_CancelDial},
    {"QL_Voice_Call_Start", TEST_QL_Voice_Call_Start},
    {"QL_Voice_Call_End", TEST_QL_Voice_Call_End},
    {"QL_Voice_Call_Answer", TEST_QL_Voice_Call_Answer},
    {"QL_Voice_Call_Hold", TEST_QL_Voice_Call_Hold},
    {"QL_Voice_Call_UnHold", TEST_QL_Voice_Call_UnHold},
    {"QL_Voice_Call_Conference", TEST_QL_Voice_Call_Conference},
    {"QL_Voice_Call_EndConference", TEST_QL_Voice_Call_EndConference},
    {"QL_Voice_Call_GetCallStatus", TEST_QL_Voice_Call_GetCallStatus}, 
    {"QL_Voice_Call_GetWaitingStatus", TEST_QL_Voice_Call_GetWaitingStatus},
    {"QL_Voice_Call_SetAutoAnswer", TEST_QL_Voice_Call_SetAutoAnswer},   
    {"QL_Voice_Call_SetWaiting", TEST_QL_Voice_Call_SetWaiting},
    {"QL_Voice_Call_RemoveStateHandler", TEST_QL_Voice_Call_RemoveStateHandler},   
    CU_TEST_INFO_NULL,
};


/**
structure:
suite name, init func, cleanup func, SetUp func, TearDown func, testfunc
*/
CU_SuiteInfo voice_suites[] = {
	{"voice", voice_suite_init, voice_suite_clean, voice_suite_SetUp, voice_suite_TearDown, voice_suite_case},
	CU_SUITE_INFO_NULL,
};

