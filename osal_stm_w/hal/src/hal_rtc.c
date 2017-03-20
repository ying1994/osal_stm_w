/**
 * @file    hal_rtc.c
 * @author  WSF
 * @version V1.0.0
 * @date    2016.03.15
 * @brief   
 ******************************************************************************
 * @attention
 *
 ******************************************************************************
 * COPYRIGHT NOTICE  
 * Copyright 2016, wsf 
 * All rights res
 *
 */
#include "stdafx.h"
#include "hal_rtc.h"

#ifdef CFG_HAL_RTC

//ƽ�����·����ڱ�
static const UINT8 mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};

static BOOL CheckLeapYear(UINT32 year)
{
    if(year%4==0)//�������ܱ�4����
    {
        if(year%100==0)
        {
            if(year%400==0)
				return TRUE;//�����00��β����Ҫ�ܱ�400����
            else 
				return FALSE;
        }
        else
		{
			return TRUE;
		}
    }
	
	return FALSE;
}

/**
 * @brief ����RTCʱ��
 * @param year  ��
 * @param mon   ��
 * @param day   ��
 * @param hour  ʱ
 * @param min   ��
 * @param sec   ��
 * @retval ���ص�ǰʱ��
 */
static time_t rtc_set_time(int year, int mon, int day, int hour, int min, int sec)
{
	time_t t;
    time_t seconds=0;
    if(year<1970||year>2099)
        return 0;
	
    for(t=1970; t<year; t++)    //��������ݵ��������
    {
        if(CheckLeapYear(t))//����
            seconds += 31622400;//�����������
        else
            seconds += 31536000;    
    }
	
    mon-=1;//�ȼ���һ����������������������5��10�գ���ֻ��Ҫ��ǰ4���µ��������ټ���10�죬Ȼ�����������
	
    for(t=0; t<mon; t++)
    {
        seconds += (u32)mon_table[t] * 86400;//�·����������
        if(CheckLeapYear(year) && (1 == t))
            seconds += 86400;//���꣬2�·�����һ���������
    }
    
    seconds += ((day-1) * 86400);//��ǰ�����ڵ���������ӣ���һ�컹û���꣬����-1��
    seconds += (hour * 3600);//Сʱ������
    seconds += (min * 60);//����������
    seconds += sec;
	
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR    | RCC_APB1Periph_BKP,ENABLE);
//    PWR_BackupAccessCmd(ENABLE);

    RTC_SetCounter(seconds);//����RTC��������ֵ
    RTC_WaitForLastTask();    //�ȴ����һ�ζ�RTC�Ĵ�����д�������
	
	return seconds;
}

/**
 * @brief RTC�жϷ�����
 */
void RTC_IRQHandler(void)
{							    
	if(RTC->CRL & RTC_IT_SEC)//�����ж�
	{							
		hal_rtc_get(NULL);//����ʱ�� 	 
	}
	if(RTC->CRL & RTC_IT_ALR)//�����ж�
	{
		RTC->CRL &= ~(RTC_IT_ALR);//�������ж�
		//TODO: ���Ӵ���
	} 				  								 
	RTC->CRL &= 0X0FFA;         //�������������жϱ�־
	while(!(RTC->CRL & BIT(5)));//�ȴ�RTC�Ĵ����������		   							 	   	 
}

/**
 * @brief RTCʱ�ӳ�ʼ��
 * @param None
 * @retval ���س�ʼ��״̬: 0 �ɹ�, -1 ʧ��
 */
int hal_rtc_init(void)
{
	u8 temp=0;
	
    NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;		//RTCȫ���ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//��ռ���ȼ�1λ,�����ȼ�3λ
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	//��ռ���ȼ�0λ,�����ȼ�4λ
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//ʹ�ܸ�ͨ���ж�
	NVIC_Init(&NVIC_InitStructure);		//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
	
	//����ǲ��ǵ�һ������ʱ��
	if (BKP_ReadBackupRegister(BKP_DR1) != 0x5050)		//��ָ���ĺ󱸼Ĵ����ж�������:��������д���ָ�����ݲ����
	{	 			
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//ʹ��PWR��BKP����ʱ��   
		PWR_BackupAccessCmd(ENABLE);	//ʹ��RTC�ͺ󱸼Ĵ������� 
		BKP_DeInit();	//������BKP��ȫ���Ĵ�������Ϊȱʡֵ 	
		RCC_LSEConfig(RCC_LSE_ON);	//�����ⲿ���پ���(LSE),ʹ��������پ���
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)	//���ָ����RCC��־λ�������,�ȴ����پ������
		{
			temp++;
			msleep(10);
		}
		if(temp>=250)
			return 1;//��ʼ��ʱ��ʧ��,����������	    
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);		//����RTCʱ��(RTCCLK),ѡ��LSE��ΪRTCʱ��    
		RCC_RTCCLKCmd(ENABLE);	//ʹ��RTCʱ��  
		RTC_WaitForSynchro();		//�ȴ����һ�ζ�RTC�Ĵ�����д�������
		RTC_WaitForLastTask();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������
		RTC_ITConfig(RTC_IT_SEC, ENABLE);		//ʹ��RTC���ж�
		RTC_WaitForLastTask();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������
		RTC_SetPrescaler(32767); //����RTCԤ��Ƶ��ֵ
		RTC_WaitForLastTask();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������
		rtc_set_time(2016, 1, 1, 0, 0, 0);  //����ʱ��	  
		BKP_WriteBackupRegister(BKP_DR1, 0X5050);	//��ָ���ĺ󱸼Ĵ�����д���û���������
	}
	else//ϵͳ������ʱ
	{
		RTC_WaitForSynchro();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������
		RTC_ITConfig(RTC_IT_SEC, ENABLE);	//ʹ��RTC���ж�
		RTC_WaitForLastTask();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������
	}		    				     
	hal_rtc_get(NULL);//����ʱ��	
	RCC_ClearFlag();	//���RCC�ĸ�λ��־λ
	
	return 0;
}

/**
 * @brief ����RTCʱ��
 * @param t ����ʱ��
 * @retval ���ص�ǰʱ��
 */
time_t hal_rtc_set(struct tm* t)
{
	return rtc_set_time(t->tm_year, t->tm_mon, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);;
}

/**
 * @brief ����RTCʱ��
 * @param t ��ȡ��ʱ�仺����
 * @retval ���ص�ǰʱ��
 */
time_t hal_rtc_get(time_t* t)
{
	time_t seconds = RTC_GetCounter();
	if (t != NULL)
		*t = seconds;
	return seconds;
}

/**
 * @brief ����ʱ��
 * @param t ����ʱ��
 * @retval ���ص�ǰʱ��
 */
time_t settime(time_t t)
{
    RTC_SetCounter(t);//����RTC��������ֵ
    RTC_WaitForLastTask();    //�ȴ����һ�ζ�RTC�Ĵ�����д�������
	return t;
}

/**
 * @brief ��ȡ����ʱ��
 * @param t ��ȡ��ʱ�仺����
 * @retval ���ص�ǰʱ��
 */
time_t time(time_t *t)
{
	time_t seconds = RTC_GetCounter();
	if (t != NULL)
		*t = seconds;
	
	return seconds;
}

struct tm* localtime(const time_t *t)
{
	static struct tm tm;
	time_t seconds = 0;
	UINT32 tsecs=0;
	UINT32 tdays=0;
	UINT32 tmon=0;
	UINT32 tyear=0;
	UINT32 daycnt=0;
	
	if (t != NULL)
		seconds = *t;
	else
		seconds = time(NULL);
	
	tdays=seconds / 86400;//�õ�����
	memset(&tm, 0, sizeof(tm));
	
	if(daycnt != tdays)//����һ��
	{
		daycnt = tdays;
		tyear=1970;		//��1970�꿪ʼ
		while(tdays>=365)
		{
			if(CheckLeapYear(tyear))//������
			{
				if(tdays>=366)    
					tdays-=366;//�������������
				else
					break;
			}
			else
			{
				tdays-=365;//ƽ��
			}
			tyear++;
		}
		
		tm.tm_year = tyear;//�õ����
		tm.tm_yday = tdays;//�õ������ѹ�������
		tm.tm_wday = tdays / 7;//�õ������ѹ���������
		tm.tm_wday += (tdays % 7) ? 0 : 1;
		
		tmon = 0;
		while(tdays>=28)//����һ����
		{
			if(CheckLeapYear(tm.tm_year) && (1 == tmon))//��������������ѭ��2��
			{
				if(tdays >= 29)    
					tdays-=29;
				else
					break;
			}
			else
			{
				if(tdays >= mon_table[tmon])//ƽ��
					tdays -= mon_table[tmon];
				else
					break;
			}
			tmon++;
		}
		tm.tm_mon = tmon;//�õ��·�, tmon=0��ʾ1��
		tm.tm_mday = tdays + 1;    //�õ����ڣ���Ϊ��һ�컹û���꣬����tdaysֻ����ǰһ�죬������ʾ��ʱ��Ҫ��ʾ��������
	}
	tsecs = seconds % 86400;//�õ�������
	tm.tm_hour= tsecs / 3600;//Сʱ
	tm.tm_min = (tsecs % 3600) / 60;//����
	tm.tm_sec = (tsecs % 3600) % 60;//��
	tm.tm_isdst = -1;
	
	return &tm;
}

#endif //CFG_HAL_RTC
