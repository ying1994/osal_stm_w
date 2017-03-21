#ifndef COMMTXTHREAD_H
#define COMMTXTHREAD_H

#include <QThread>
#include "../types.h"

/**
 * @brief COMM数据发送线程
 */
class CommTxThread : public QThread
{
public:
    CommTxThread();

    virtual ~CommTxThread();

    void setThreadRunFlag(void);
    BOOL getIsThreadRun(void);
    void stop();
    void run();

private:
    volatile BOOL m_bThreadRun;
};


#endif // COMMTXTHREAD_H
