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
 * All rights Reserved
 *
 */

#include "hal_rtc.h"
#include "hal_cpu.h"

#include "string.h"

#ifdef CFG_HAL_RTC

//平均的月份日期表
static const UINT8 mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};

static BOOL CheckLeapYear(UINT32 year)
{
    if(year%4==0)//首先需能被4整除
    {
        if(year%100==0)
        {
            if(year%400==0)
				return TRUE;//如果以00结尾，还要能被400整除
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
 * @brief 设置RTC时钟
 * @param year  年
 * @param mon   月
 * @param day   日
 * @param hour  时
 * @param min   分
 * @param sec   秒
 * @retval 返回当前时间
 */
static time_t rtc_set_time(int year, int mon, int day, int hour, int min, int sec)
{
	time_t t;
    time_t seconds=0;
    if(year<1970||year>2099)
        return 0;
	
    for(t=1970; t<year; t++)    //把所有年份的秒钟相加
    {
        if(CheckLeapYear(t))//闰年
            seconds += 31622400;//闰年的秒钟数
        else
            seconds += 31536000;    
    }
	
    mon-=1;//先减掉一个月再算秒数（如现在是5月10日，则只需要算前4个月的天数，再加上10天，然后计算秒数）
	
    for(t=0; t<mon; t++)
    {
        seconds += (u32)mon_table[t] * 86400;//月份秒钟数相加
        if(CheckLeapYear(year) && (1 == t))
            seconds += 86400;//闰年，2月份增加一天的秒钟数
    }
    
    seconds += ((day-1) * 86400);//把前面日期的秒钟数相加（这一天还没过完，所以-1）
    seconds += (hour * 3600);//小时秒钟数
    seconds += (min * 60);//分钟秒钟数
    seconds += sec;
	
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR    | RCC_APB1Periph_BKP,ENABLE);
//    PWR_BackupAccessCmd(ENABLE);

    RTC_SetCounter(seconds);//设置RTC计数器的值
    RTC_WaitForLastTask();    //等待最近一次对RTC寄存器的写操作完成
	
	return seconds;
}

/**
 * @brief RTC中断服务函数
 */
void RTC_IRQHandler(void)
{							    
	if(RTC->CRL & RTC_IT_SEC)//秒钟中断
	{							
		hal_rtc_get(NULL);//更新时间 	 
	}
	if(RTC->CRL & RTC_IT_ALR)//闹钟中断
	{
		RTC->CRL &= ~(RTC_IT_ALR);//清闹钟中断
		//TODO: 闹钟处理
	} 				  								 
	RTC->CRL &= 0X0FFA;         //清除溢出，秒钟中断标志
	while(!(RTC->CRL & BIT(5)));//等待RTC寄存器操作完成		   							 	   	 
}

/**
 * @brief RTC时钟初始化
 * @param None
 * @retval 返回初始化状态: 0 成功, -1 失败
 */
int hal_rtc_init(void)
{
	u8 temp=0;
	
    NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;		//RTC全局中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//先占优先级1位,从优先级3位
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	//先占优先级0位,从优先级4位
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//使能该通道中断
	NVIC_Init(&NVIC_InitStructure);		//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
	
	//检查是不是第一次配置时钟
	if (BKP_ReadBackupRegister(BKP_DR1) != 0x5050)		//从指定的后备寄存器中读出数据:读出了与写入的指定数据不相乎
	{	 			
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//使能PWR和BKP外设时钟   
		PWR_BackupAccessCmd(ENABLE);	//使能RTC和后备寄存器访问 
		BKP_DeInit();	//将外设BKP的全部寄存器重设为缺省值 	
		RCC_LSEConfig(RCC_LSE_ON);	//设置外部低速晶振(LSE),使用外设低速晶振
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)	//检查指定的RCC标志位设置与否,等待低速晶振就绪
		{
			temp++;
			msleep(10);
		}
		if(temp>=250)
			return 1;//初始化时钟失败,晶振有问题	    
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);		//设置RTC时钟(RTCCLK),选择LSE作为RTC时钟    
		RCC_RTCCLKCmd(ENABLE);	//使能RTC时钟  
		RTC_WaitForSynchro();		//等待最近一次对RTC寄存器的写操作完成
		RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成
		RTC_ITConfig(RTC_IT_SEC, ENABLE);		//使能RTC秒中断
		RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成
		RTC_SetPrescaler(32767); //设置RTC预分频的值
		RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成
		rtc_set_time(2016, 1, 1, 0, 0, 0);  //设置时间	  
		BKP_WriteBackupRegister(BKP_DR1, 0X5050);	//向指定的后备寄存器中写入用户程序数据
	}
	else//系统继续计时
	{
		RTC_WaitForSynchro();	//等待最近一次对RTC寄存器的写操作完成
		RTC_ITConfig(RTC_IT_SEC, ENABLE);	//使能RTC秒中断
		RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成
	}		    				     
	hal_rtc_get(NULL);//更新时间	
	RCC_ClearFlag();	//清除RCC的复位标志位
	
	return 0;
}

/**
 * @brief 设置RTC时钟
 * @param t 设置时间
 * @retval 返回当前时间
 */
time_t hal_rtc_set(struct tm* t)
{
	return rtc_set_time(t->tm_year, t->tm_mon, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);;
}

/**
 * @brief 更新RTC时钟
 * @param t 读取的时间缓冲区
 * @retval 返回当前时间
 */
time_t hal_rtc_get(time_t* t)
{
	time_t seconds = RTC_GetCounter();
	if (t != NULL)
		*t = seconds;
	return seconds;
}

/**
 * @brief 设置时间
 * @param t 设置时间
 * @retval 返回当前时间
 */
time_t settime(time_t t)
{
    RTC_SetCounter(t);//设置RTC计数器的值
    RTC_WaitForLastTask();    //等待最近一次对RTC寄存器的写操作完成
	return t;
}

/**
 * @brief 获取最新时间
 * @param t 读取的时间缓冲区
 * @retval 返回当前时间
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
	
	tdays=seconds / 86400;//得到天数
	memset(&tm, 0, sizeof(tm));
	
	if(daycnt != tdays)//超过一天
	{
		daycnt = tdays;
		tyear=1970;		//从1970年开始
		while(tdays>=365)
		{
			if(CheckLeapYear(tyear))//是闰年
			{
				if(tdays>=366)    
					tdays-=366;//减掉闰年的天数
				else
					break;
			}
			else
			{
				tdays-=365;//平年
			}
			tyear++;
		}
		
		tm.tm_year = tyear;//得到年份
		tm.tm_yday = tdays;//得到当年已过的天数
		tm.tm_wday = tdays / 7;//得到当年已过的星期数
		tm.tm_wday += (tdays % 7) ? 0 : 1;
		
		tmon = 0;
		while(tdays>=28)//超过一个月
		{
			if(CheckLeapYear(tm.tm_year) && (1 == tmon))//当年是闰年且轮循到2月
			{
				if(tdays >= 29)    
					tdays-=29;
				else
					break;
			}
			else
			{
				if(tdays >= mon_table[tmon])//平年
					tdays -= mon_table[tmon];
				else
					break;
			}
			tmon++;
		}
		tm.tm_mon = tmon;//得到月份, tmon=0表示1月
		tm.tm_mday = tdays + 1;    //得到日期，因为这一天还没过完，所以tdays只到其前一天，但是显示的时候要显示正常日期
	}
	tsecs = seconds % 86400;//得到秒钟数
	tm.tm_hour= tsecs / 3600;//小时
	tm.tm_min = (tsecs % 3600) / 60;//分钟
	tm.tm_sec = (tsecs % 3600) % 60;//秒
	tm.tm_isdst = -1;
	
	return &tm;
}

#endif //CFG_HAL_RTC
