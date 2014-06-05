
#ifndef WALLETSTACK_H
#define WALLETSTACK_H

#include <QStackedWidget>
#include <QMap>
#include <boost/shared_ptr.hpp>

class BitcoinGUI;
class TransactionTableModel;
class ClientModel;
class WalletModel;
class WalletView;
class TransactionView;
class OverviewPage;
class AddressBookPage;
class SendCoinsDialog;
class SignVerifyMessageDialog;
class Notificator;
class RPCConsole;

class CWalletManager;

QT_BEGIN_NAMESPACE
class QLabel;
class QModelIndex;
QT_END_NAMESPACE


class WalletStack : public QStackedWidget
{
    Q_OBJECT

public:
    explicit WalletStack(QWidget *parent = 0);
    ~WalletStack();

    void setBitcoinGUI(BitcoinGUI *gui) { this->gui = gui; }

    void setClientModel(ClientModel *clientModel) { this->clientModel = clientModel; }

    bool addWallet(const QString& name, WalletModel *walletModel);
    bool removeWallet(const QString& name);

    void removeAllWallets();

    bool handleURI(const QString &uri);

    void showOutOfSyncWarning(bool fShow);

private:
    BitcoinGUI *gui;
    ClientModel *clientModel;
    QMap<QString, WalletView*> mapWalletViews;

    bool bOutOfSync;

public slots:
    void setCurrentWallet(const QString& name);

    
    void gotoOverviewPage();
    
    void gotoHistoryPage();
    
    void gotoAddressBookPage();
    
    void gotoReceiveCoinsPage();
    
    void gotoSendCoinsPage(QString addr = "");

    
    void gotoSignMessageTab(QString addr = "");
    
    void gotoVerifyMessageTab(QString addr = "");

    
    void encryptWallet(bool status);
    
    void backupWallet();
    
    void changePassphrase();
    
    void unlockWallet();

    
    void setEncryptionStatus();
};


