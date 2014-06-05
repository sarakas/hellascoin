
#ifndef WALLETVIEW_H
#define WALLETVIEW_H

#include <QStackedWidget>

class BitcoinGUI;
class ClientModel;
class WalletModel;
class TransactionView;
class OverviewPage;
class AddressBookPage;
class SendCoinsDialog;
class SignVerifyMessageDialog;
class RPCConsole;

QT_BEGIN_NAMESPACE
class QLabel;
class QModelIndex;
QT_END_NAMESPACE


class WalletView : public QStackedWidget
{
    Q_OBJECT

public:
    explicit WalletView(QWidget *parent, BitcoinGUI *_gui);
    ~WalletView();

    void setBitcoinGUI(BitcoinGUI *gui);
    
    void setClientModel(ClientModel *clientModel);
    
    void setWalletModel(WalletModel *walletModel);

    bool handleURI(const QString &uri);

    void showOutOfSyncWarning(bool fShow);

private:
    BitcoinGUI *gui;
    ClientModel *clientModel;
    WalletModel *walletModel;

    OverviewPage *overviewPage;
    QWidget *transactionsPage;
    AddressBookPage *addressBookPage;
    AddressBookPage *receiveCoinsPage;
    SendCoinsDialog *sendCoinsPage;
    SignVerifyMessageDialog *signVerifyMessageDialog;

    TransactionView *transactionView;

public slots:
    
    void gotoOverviewPage();
    
    void gotoHistoryPage();
    
    void gotoAddressBookPage();
    
    void gotoReceiveCoinsPage();
    
    void gotoSendCoinsPage(QString addr = "");

    
    void gotoSignMessageTab(QString addr = "");
    
    void gotoVerifyMessageTab(QString addr = "");

    
    void incomingTransaction(const QModelIndex& parent, int start, int );
    
    void encryptWallet(bool status);
    
    void backupWallet();
    
    void changePassphrase();
    
    void unlockWallet();

    void setEncryptionStatus();

signals:
    
    void showNormalIfMinimized();
};


