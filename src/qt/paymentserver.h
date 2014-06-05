#ifndef PAYMENTSERVER_H
#define PAYMENTSERVER_H




























#include <QObject>
#include <QString>

class QApplication;
class QLocalServer;

class PaymentServer : public QObject
{
    Q_OBJECT

private:
    bool saveURIs;
    QLocalServer* uriServer;

public:



    static bool ipcSendCommandLine();

    PaymentServer(QApplication* parent);

    bool eventFilter(QObject *object, QEvent *event);

signals:
    void receivedURI(QString);

public slots:


    void uiReady();

private slots:
    void handleURIConnection();
};


