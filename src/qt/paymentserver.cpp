



#include <QApplication>

#include "paymentserver.h"

#include "guiconstants.h"
#include "ui_interface.h"
#include "util.h"

#include <QByteArray>
#include <QDataStream>
#include <QDebug>
#include <QFileOpenEvent>
#include <QHash>
#include <QLocalServer>
#include <QLocalSocket>
#include <QStringList>
#if QT_VERSION < 0x050000
#include <QUrl>
#endif

using namespace boost;


const QString BITCOIN_IPC_PREFIX("litecoin:");






static QString ipcServerName()
{
    QString name("BitcoinQt");




    QString ddir(GetDataDir(true).string().c_str());
    name.append(QString::number(qHash(ddir)));

    return name;
}






static QStringList savedPaymentRequests;







bool PaymentServer::ipcSendCommandLine()
{
    bool fResult = false;

    const QStringList& args = qApp->arguments();
    for (int i = 1; i < args.size(); i++)
    {
        if (!args[i].startsWith(BITCOIN_IPC_PREFIX, Qt::CaseInsensitive))
            continue;
        savedPaymentRequests.append(args[i]);
    }

    foreach (const QString& arg, savedPaymentRequests)
    {
        QLocalSocket* socket = new QLocalSocket();
        socket->connectToServer(ipcServerName(), QIODevice::WriteOnly);
        if (!socket->waitForConnected(BITCOIN_IPC_CONNECT_TIMEOUT))
            return false;

        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_0);
        out << arg;
        out.device()->seek(0);
        socket->write(block);
        socket->flush();

        socket->waitForBytesWritten(BITCOIN_IPC_CONNECT_TIMEOUT);
        socket->disconnectFromServer();
        delete socket;
        fResult = true;
    }
    return fResult;
}

PaymentServer::PaymentServer(QApplication* parent) : QObject(parent), saveURIs(true)
{

    parent->installEventFilter(this);

    QString name = ipcServerName();


    QLocalServer::removeServer(name);

    uriServer = new QLocalServer(this);

    if (!uriServer->listen(name))
        qDebug() << tr("Cannot start litecoin: click-to-pay handler");
    else
        connect(uriServer, SIGNAL(newConnection()), this, SLOT(handleURIConnection()));
}

bool PaymentServer::eventFilter(QObject *object, QEvent *event)
{

    if (event->type() == QEvent::FileOpen)
    {
        QFileOpenEvent* fileEvent = static_cast<QFileOpenEvent*>(event);
        if (!fileEvent->url().isEmpty())
        {

                savedPaymentRequests.append(fileEvent->url().toString());
            else
                emit receivedURI(fileEvent->url().toString());
            return true;
        }
    }
    return false;
}

void PaymentServer::uiReady()
{
    saveURIs = false;
    foreach (const QString& s, savedPaymentRequests)
        emit receivedURI(s);
    savedPaymentRequests.clear();
}

void PaymentServer::handleURIConnection()
{
    QLocalSocket *clientConnection = uriServer->nextPendingConnection();

    while (clientConnection->bytesAvailable() < (int)sizeof(quint32))
        clientConnection->waitForReadyRead();

    connect(clientConnection, SIGNAL(disconnected()),
            clientConnection, SLOT(deleteLater()));

    QDataStream in(clientConnection);
    in.setVersion(QDataStream::Qt_4_0);
    if (clientConnection->bytesAvailable() < (int)sizeof(quint16)) {
        return;
    }
    QString message;
    in >> message;

    if (saveURIs)
        savedPaymentRequests.append(message);
    else
        emit receivedURI(message);
}
