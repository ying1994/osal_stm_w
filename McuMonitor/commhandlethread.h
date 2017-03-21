#ifndef COMMHANDLETHREAD_H
#define COMMHANDLETHREAD_H

#include <QThread>

class CommHandleThread : public QThread
{
    Q_OBJECT
public:
    CommHandleThread();
    ~CommHandleThread();
    void stop();

    void run();
signals:
    void timeout();
private:
    bool m_bThreadRun;
};

#endif // COMMHANDLETHREAD_H
