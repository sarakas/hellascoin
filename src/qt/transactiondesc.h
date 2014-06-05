#ifndef TRANSACTIONDESC_H
#define TRANSACTIONDESC_H

#include <QString>
#include <QObject>

class CWallet;
class CWalletTx;


class TransactionDesc: public QObject
{
    Q_OBJECT

public:
    static QString toHTML(CWallet *wallet, CWalletTx &wtx);

private:
    TransactionDesc() {}

    static QString FormatTxStatus(const CWalletTx& wtx);
};


