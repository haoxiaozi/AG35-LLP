#include "ql_oe.h"
#include "ql_gpio.h"



static Enum_PinName m_GpioPin3 = PINNAME_GPIO4; // OPEN EVB J0203 GPIO3
static Enum_PinName m_GpioPin1 = PINNAME_GPIO6;
static Enum_PinName m_GpioPin5 = PINNAME_GPIO5;
static Enum_PinName m_GpioPin6 = PINNAME_GPIO2;
static unsigned int case_status = 0;

static void callback_onAlarm(int param);
void led_status3(void);
void led_status2(void);
void led_status1(void);
void delay_ms(int ms);

int main(int argc, char* argv[])
{
	int iRet1,iRet3,iRet5,iRet6;

	printf("< OpenLinux: GPIO example >\n");
	
	iRet1 = Ql_GPIO_Init(m_GpioPin1, PINDIRECTION_OUT, PINLEVEL_HIGH, PINPULLSEL_DISABLE);
    iRet3 = Ql_GPIO_Init(m_GpioPin3, PINDIRECTION_OUT, PINLEVEL_HIGH, PINPULLSEL_DISABLE);
    iRet5 = Ql_GPIO_Init(m_GpioPin5, PINDIRECTION_OUT, PINLEVEL_HIGH, PINPULLSEL_DISABLE);
    iRet6 = Ql_GPIO_Init(m_GpioPin6, PINDIRECTION_OUT, PINLEVEL_HIGH, PINPULLSEL_DISABLE);

	printf("< Init GPIO: pin=%d, dir=%d, level=%d, iRet=%d >\n", m_GpioPin1, PINDIRECTION_OUT, PINLEVEL_HIGH, iRet1);
    printf("< Init GPIO: pin=%d, dir=%d, level=%d, iRet=%d >\n", m_GpioPin3, PINDIRECTION_OUT, PINLEVEL_HIGH, iRet3);
    printf("< Init GPIO: pin=%d, dir=%d, level=%d, iRet=%d >\n", m_GpioPin5, PINDIRECTION_OUT, PINLEVEL_HIGH, iRet5);
    printf("< Init GPIO: pin=%d, dir=%d, level=%d, iRet=%d >\n", m_GpioPin6, PINDIRECTION_OUT, PINLEVEL_HIGH, iRet6);
	signal(SIGALRM, callback_onAlarm);
	alarm(2);

    while (1)
	{
        switch(case_status)
        {
            case 0:
                led_status1();
                break;
            case 1:
                led_status2();
                break;
             case 2:
                led_status3();
                break; 
            default:
                break;   
        }

		sleep(1);
	}
 
    return 0;
}


void delay_ms(int ms)
{
	struct timeval delay;
	delay.tv_sec = 0;
	delay.tv_usec = ms * 1000; // 20 ms
	select(0, NULL, NULL, NULL, &delay);
}


void led1_blink(void)
{
    int lvl1 = Ql_GPIO_GetLevel(m_GpioPin1);
    if (lvl1 < 0)
	{
		printf("< fail to read pin level >\n");
		return;
	}
	if (1 == lvl1)
	{
		Ql_GPIO_SetLevel(m_GpioPin1, PINLEVEL_LOW);
	//	printf("< Pull pin level to low >\n");
	}else{
		Ql_GPIO_SetLevel(m_GpioPin1, PINLEVEL_HIGH);
	//	printf("< Pull pin level to high >\n");
	}

}

void led2_blink(void)
{
    int lvl3 = Ql_GPIO_GetLevel(m_GpioPin3);
    if (lvl3 < 0)
	{
		printf("< fail to read pin level >\n");
		return;
	}
	if (1 == lvl3)
	{
		Ql_GPIO_SetLevel(m_GpioPin3, PINLEVEL_LOW);
	//	printf("< Pull pin level to low >\n");
	}else{
		Ql_GPIO_SetLevel(m_GpioPin3, PINLEVEL_HIGH);
	//	printf("< Pull pin level to high >\n");
	}

}

void led3_blink(void)
{
    int lvl5 = Ql_GPIO_GetLevel(m_GpioPin5);
	if (lvl5 < 0)
	{
		printf("< fail to read pin level >\n");
		return;
	}
	if (1 == lvl5)
	{
		Ql_GPIO_SetLevel(m_GpioPin5, PINLEVEL_LOW);
	//	printf("< Pull pin level to low >\n");
	}else{
		Ql_GPIO_SetLevel(m_GpioPin5, PINLEVEL_HIGH);
	//	printf("< Pull pin level to high >\n");
	}

}

void led4_blink(void)
{
    int lvl6 = Ql_GPIO_GetLevel(m_GpioPin6);
	if (lvl6 < 0)
	{
		printf("< fail to read pin level >\n");
		return;
	}
	if (1 == lvl6)
	{
		Ql_GPIO_SetLevel(m_GpioPin6, PINLEVEL_LOW);
	//	printf("< Pull pin level to low >\n");
	}else{
		Ql_GPIO_SetLevel(m_GpioPin6, PINLEVEL_HIGH);
	//	printf("< Pull pin level to high >\n");
	}

}

void led_status1(void)
{
    led1_blink();
    delay_ms(300);
    led2_blink();
    delay_ms(300);
    led3_blink();
    delay_ms(300);
    led4_blink();
    delay_ms(300);

}

void led_status2(void)
{
    led1_blink();
    led2_blink();
    delay_ms(300);
    delay_ms(300);
    led4_blink();
    led3_blink();
    delay_ms(300);
    delay_ms(300);
}

void led_status3(void)
{
    led1_blink();
    led2_blink();
    led4_blink();
    led3_blink();
    delay_ms(300);
    delay_ms(300);
    led1_blink();
    led2_blink();
    led4_blink();
    led3_blink();

}

void callback_onAlarm(int param)
{
    case_status=(case_status+1)%3;
	alarm(10);
}
