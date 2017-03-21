#ifndef UPDATETHREAD_H
#define UPDATETHREAD_H
#include <QThread>
#include <QString>
#include <QVector>
#include "comm/bdcomm.h"

#include "types.h"

class UpdateDialog;

/**
 * @brief 程序升级线程
 */
class UpdateThread : public QThread
{
    Q_OBJECT
public:
    UpdateThread(QObject *parent = 0);
    virtual ~UpdateThread();
    void setIsThreadRun(void);
    BOOL getIsThreadRun(void);
    void stop();

    void run();

signals:
    void SendUpdate_start();
    void SendUpdate_ready();
    void SendUpdate_updating();
    void SendUpdate_result();
    void SendUpdate_lastresult();
    void SendUpdate_end();



private slots:

private:
    BOOL m_bThreadRun;
    //UpdateDialog* m_hUpdateDialog;

    qint32 m_nSendDataSize;
    quint16 m_uCount;

    BD_MsgTypeDef m_txMessage;
};

#endif // UPDATETHREAD_H
