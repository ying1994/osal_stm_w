#include "updatethread.h"
#include "comm/bdcomm.h"
#include "unit.h"
#include "function.h"
#include "optype.h"
#include "errorcode.h"
#include "socket/osal_socket.h"
#include "updatedialog.h"
#include "updatedatainfo.h"

UpdateThread::UpdateThread(QObject *parent) : QThread(parent)
{
    //m_hUpdateDialog = hUpdateDialog;
    m_bThreadRun = TRUE;
    m_uCount = 0;
}

UpdateThread::~UpdateThread()
{
    stop();
    terminate();
    wait();
}


void UpdateThread::setIsThreadRun()
{
    m_bThreadRun = TRUE;
}

BOOL UpdateThread::getIsThreadRun(void)
{
    return m_bThreadRun;
}


void UpdateThread::stop()
{
    m_bThreadRun = FALSE;
}

/**
 * @brief COMM数据发送线程
 */
void UpdateThread::run()
{
    while (m_bThreadRun)
    {
        msleep(50);
        if (STOP == UpdateDataInfo::getInstance()->getUpdateStage())
            continue;

        UpdateDataInfo::getInstance()->m_FileMutex.lock();
        if (UpdateDataInfo::getInstance()->getMsgSendFlag())
        {
            if (m_uCount++ < 10)
            {
                UpdateDataInfo::getInstance()->m_FileMutex.unlock();
                continue;
            }
        }
        m_uCount = 0;
        UpdateDataInfo::getInstance()->setMsgSendFlag(true);

        if (START == UpdateDataInfo::getInstance()->getUpdateStage())
        {
            //this->OnUpdate_start();
            emit SendUpdate_start();
        }
        else if (READY == UpdateDataInfo::getInstance()->getUpdateStage())
        {
            //this->OnUpdate_ready();
            emit SendUpdate_ready();
        }
        else if (UPDATING == UpdateDataInfo::getInstance()->getUpdateStage())
        {
            //this->OnUpdate_updating();
            emit SendUpdate_updating();
        }
        else if (RESULT == UpdateDataInfo::getInstance()->getUpdateStage())
        {
            //this->OnUpdate_result();
            emit SendUpdate_result();
        }
        else if (LAST_RESULT == UpdateDataInfo::getInstance()->getUpdateStage())
        {
            //this->OnUpdate_lastresult();
            emit SendUpdate_lastresult();
        }
        else if (END == UpdateDataInfo::getInstance()->getUpdateStage())
        {
            //this->OnUpdate_end();
            emit SendUpdate_end();
        }
        UpdateDataInfo::getInstance()->m_FileMutex.unlock();
    }
    //DBG(TRACE("UpdateThread stop"));
}
