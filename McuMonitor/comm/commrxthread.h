#ifndef COMMRXTHREAD_H
#define COMMRXTHREAD_H

#include <QThread>
#include "../types.h"


/**
 * @brief COMM数据接收线程
 */
class CommRxThread : public QThread
{
public:
    CommRxThread();

    virtual ~CommRxThread();

    void setThreadRunFlag(void);
    BOOL getIsThreadRun(void);
    void stop();

    void run();

private:
    volatile BOOL m_bThreadRun;
};

#endif // COMMRXTHREAD_H
