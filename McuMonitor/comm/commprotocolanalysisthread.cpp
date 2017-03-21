#include "commprotocolanalysisthread.h"
#include "bdcomm.h"
#include "socket/osal_socket.h"


CommProtocolAnalysisThread::CommProtocolAnalysisThread()
{
    m_bThreadRun = FALSE;
}

CommProtocolAnalysisThread::~CommProtocolAnalysisThread()
{
    stop();
    terminate();
    wait();
}

void CommProtocolAnalysisThread::setThreadRunFlag(void)
{
    m_bThreadRun = TRUE;
}

BOOL CommProtocolAnalysisThread::getIsThreadRun(void)
{
    return m_bThreadRun;
}

void CommProtocolAnalysisThread::stop()
{
    m_bThreadRun = FALSE;
}

/**
 * @brief COMM协议解析线程
 */
void CommProtocolAnalysisThread::run()
{
    static UCHAR rxFlag = 0;
    static UINT16 uRxDataIndex = 0;
    static UINT16 uAddr = 0;
    static UINT16 uBlockID = 0;
    static UINT16 funcID = 0;
    static UINT16 msgLen = 0;
    UCHAR rxData = 0;
    while (m_bThreadRun)
    {
        BDComm::getInstance()->m_rxBufferMutex.lock();

        if (BDComm::getInstance()->m_uReadPriter == BDComm::getInstance()->m_uWritePriter)
        {
            BDComm::getInstance()->m_rxBufferMutex.unlock();
            continue;
        }
        rxData = BDComm::getInstance()->m_rxDataBuffer[BDComm::getInstance()->m_uReadPriter];
        //DBG(TRACE("readdata = %02x %c %d", rxData, rxData, rxFlag));
        BDComm::getInstance()->m_uReadPriter = (BDComm::getInstance()->m_uReadPriter + 1) % MAX_MSG_PHY_DATA_SIZE;
        switch(rxFlag)
        {
        case 0:			//起始标志1
            if (PHY_DATA_FLAG1 == rxData)
            {
                ++rxFlag;
            }
            break;
        case 1:			//起始标志2
            if (PHY_DATA_FLAG2 == rxData)
            {
                ++rxFlag;
            }
            else if (PHY_DATA_FLAG1 == rxData)
            {
                rxFlag = 0;
                uRxDataIndex = 0;
            }
            break;
        case 2:			//地址高8位
            uAddr = rxData;
            ++rxFlag;
            break;
        case 3:			//地址低8位
            uAddr = (uAddr << 8) | rxData;
            BDComm::getInstance()->m_rxMessage.uAddr = bd_sock_ntohs(uAddr);
            ++rxFlag;
            break;
        case 4:			//功能单元高8位
            uBlockID = rxData;
            ++rxFlag;
            break;
        case 5:			//功能单元低8位
            uBlockID = (uBlockID << 8) | rxData;
            BDComm::getInstance()->m_rxMessage.blockID = bd_sock_ntohs(uBlockID);
            ++rxFlag;
            break;
        case 6:			//功能ID高8位
            funcID = rxData;
            ++rxFlag;
            break;
        case 7:			//功能ID低8位
            funcID = (funcID << 8) | rxData;
            BDComm::getInstance()->m_rxMessage.functionID = bd_sock_ntohs(funcID);
            ++rxFlag;
            break;
        case 8:			//操作码
            //rxMsg.opType = rxData;
            BDComm::getInstance()->m_rxMessage.opType = rxData;
            ++rxFlag;
            break;
        case 9:			//数据长度高8位
            //rxMsg.len = rxData;
            msgLen = rxData;
            ++rxFlag;
            break;
        case 10:		//数据长度低8位
            msgLen = (msgLen << 8) | rxData;
            BDComm::getInstance()->m_rxMessage.len = bd_sock_ntohs(msgLen);
            if (BDComm::getInstance()->getCheckLenght(BDComm::getInstance()->m_rxMessage.len) == BDComm::getInstance()->m_rxMessage.len)
            {
                BDComm::getInstance()->m_rxMessage.len = BDComm::getInstance()->m_rxMessage.len & 0x0fff;
                ++rxFlag;
                uRxDataIndex = 0;
                //DBG(TRACE("CommProtocolAnalysisThread::GetCheckLenght OK!"));
            }
            else
            {
                rxFlag = 0;
                uRxDataIndex = 0;
                DBG(TRACE("CommProtocolAnalysisThread::GetCheckLenght Error: RX[%04X], CH[%04X]\r\n", BDComm::getInstance()->m_rxMessage.len, BDComm::getInstance()->getCheckLenght(BDComm::getInstance()->m_rxMessage.len)));
            }
            break;
        case 11:		//用户数据
            if (uRxDataIndex < BDComm::getInstance()->m_rxMessage.len)
            {
                BDComm::getInstance()->m_rxMessage.data[uRxDataIndex++] = rxData;
            }
            else		//数据接收完成
            {
                rxFlag = 0;
                uRxDataIndex = 0;
                if (rxData == BDComm::getInstance()->getCheckValue(&BDComm::getInstance()->m_rxMessage)) //接收数据校验
                {
                    //DBG(TRACE("CommProtocolAnalysisThread::GetCheckValue OK!"));
                    BDComm::getInstance()->setCommMsgEvent();
                }
                else		//数据校验失败
                {
                    DBG(TRACE("CommProtocolAnalysisThread::GetCheckValue Error: RX[%02X], CH[%02X]\r\n", rxData, BDComm::getInstance()->getCheckValue(&BDComm::getInstance()->m_rxMessage)));
                }
            }
            break;
        default:
            rxFlag = 0;
            uRxDataIndex = 0;
            break;
        }
        BDComm::getInstance()->m_rxBufferMutex.unlock();
    }
}

