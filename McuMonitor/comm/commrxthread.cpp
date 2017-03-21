#include "commrxthread.h"
#include "bdcomm.h"


CommRxThread::CommRxThread()
{
    m_bThreadRun = FALSE;
}

CommRxThread::~CommRxThread()
{
    stop();
    terminate();
    wait();
}

void CommRxThread::setThreadRunFlag(void)
{
    m_bThreadRun = TRUE;
}

BOOL CommRxThread::getIsThreadRun(void)
{
    return m_bThreadRun;
}

void CommRxThread::stop()
{
    m_bThreadRun = FALSE;
}


/**
 * @brief COMM数据接收线程
 */
void CommRxThread::run()
{
    int count = 0;
    static char rxbuffer[1000] = {0};
    while (m_bThreadRun)
    {
        BDComm::getInstance()->m_rxBufferMutex.lock();
        if (BDComm::getInstance()->m_hSerialPort != NULL)
        {
            count = BDComm::getInstance()->m_hSerialPort->read(&rxbuffer[0], 1);
            //count = BDComm::getInstance()->m_hSerialPort->read(&rxbuffer[0], 1000);
            if (count > 0)
            {
                /*rxbuffer[count] = '\0';
                DBG(TRACE("msg: %s\r", rxbuffer));*/

                BDComm::getInstance()->m_rxDataBuffer[BDComm::getInstance()->m_uWritePriter] = rxbuffer[0];
                //DBG(TRACE("writedata = %02x",BDComm::getInstance()->m_rxDataBuffer[BDComm::getInstance()->m_uWritePriter]));

                BDComm::getInstance()->m_uWritePriter = (BDComm::getInstance()->m_uWritePriter + 1) % MAX_MSG_PHY_DATA_SIZE;

                if (BDComm::getInstance()->m_uWritePriter == BDComm::getInstance()->m_uReadPriter)
                {
                    BDComm::getInstance()->m_uReadPriter = (BDComm::getInstance()->m_uReadPriter + 1) % MAX_MSG_PHY_DATA_SIZE;
                    //DBG(TRACE("2. %04x",BDComm::getInstance()->m_uReadPriter));
                }
            }//if (count > 0)
        }//if (BDComm::getInstance()->m_hSerialPort != NULL)
        BDComm::getInstance()->m_rxBufferMutex.unlock();
    }

}
