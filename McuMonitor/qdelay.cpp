/**
 ******************************************************************************
 * @file    qdelay.cpp
 * @author  WSF
 * @version
 * @date    2017.01.16
 * @brief   QT ��ʱ����
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
 * @brief ���뼶��ʱ
 * @param ms ��ʱʱ��(��λ��ms)
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
 * @brief �뼶��ʱ
 * @param s ��ʱʱ��(��λ��s)
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

