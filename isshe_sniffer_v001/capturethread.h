#ifndef CAPTURETHREAD_H
#define CAPTURETHREAD_H
#include <QThread>

class CaptureThread : public QThread
{
public:
    CaptureThread();

    void run();
};

#endif // CAPTURETHREAD_H
