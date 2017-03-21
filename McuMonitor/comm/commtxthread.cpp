#include "commtxthread.h"
#include "bdcomm.h"


CommTxThread::CommTxThread()
{
    m_bThreadRun = FALSE;
}

CommTxThread::~CommTxThread()
{
    stop();
    terminate();
    wait();
}

void CommTxThread::setThreadRunFlag(void)
{
    m_bThreadRun = TRUE;
}

BOOL CommTxThread::getIsThreadRun(void)
{
    return m_bThreadRun;
}

void CommTxThread::stop()
{
    m_bThreadRun = FALSE;
}

/**
 * @brief COMM数据发送线程
 */
void CommTxThread::run()
{
    while (m_bThreadRun)
    {
        msleep(100);//100ms sleep
        if (BDComm::getInstance()->readTxFIFO(&BDComm::getInstance()->m_txMessage))
        {
            BDComm::getInstance()->transmit(&BDComm::getInstance()->m_txMessage);
        }
    }
}


