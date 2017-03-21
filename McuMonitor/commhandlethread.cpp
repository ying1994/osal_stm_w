#include "commhandlethread.h"

CommHandleThread::CommHandleThread()
{
    m_bThreadRun = true;

}

CommHandleThread::~CommHandleThread()
{
    stop();
    terminate();
    wait();
}

void CommHandleThread::stop()
{
    m_bThreadRun = false;
}

/**
 * @brief COMM数据发送线程
 */
void CommHandleThread::run()
{
    while (m_bThreadRun)
    {
        msleep(1000);
        emit timeout();
    }
}
