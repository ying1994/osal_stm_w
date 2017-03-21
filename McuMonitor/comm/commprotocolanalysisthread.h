#ifndef COMMPROTOCOLANALYSISTHREAD_H
#define COMMPROTOCOLANALYSISTHREAD_H

#include <QThread>
#include "../types.h"

/**
 * @brief COMM协议解析线程
 */
class CommProtocolAnalysisThread : public QThread
{
public:
    CommProtocolAnalysisThread();

    virtual ~CommProtocolAnalysisThread();

    void setThreadRunFlag(void);
    BOOL getIsThreadRun(void);
    void stop();
    void run();

private:
    volatile BOOL m_bThreadRun;
};



#endif // COMMPROTOCOLANALYSISTHREAD_H
