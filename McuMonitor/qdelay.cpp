/**
 ******************************************************************************
 * @file    qdelay.cpp
 * @author  WSF
 * @version
 * @date    2017.01.16
 * @brief   QT 延时程序
 ******************************************************************************
 * @attention
 *
 *
 * @note
 *
 ******************************************************************************
 */
#include "qdelay.h"

#include <QTime>
#include <QCoreApplication>

/**
 * @brief 毫秒级延时
 * @param ms 延时时间(单位：ms)
 * @return None
 */
void delayms(int ms)
{
    QTime t;
    t.start();
    while(t.elapsed() < ms)
        QCoreApplication::processEvents();
}

/**
 * @brief 秒级延时
 * @param s 延时时间(单位：s)
 * @return None
 */
void delayms(int s)
{
	int t = 1000*s;
    QTime t;
    t.start();
    while(t.elapsed() < t)
        QCoreApplication::processEvents();
}

