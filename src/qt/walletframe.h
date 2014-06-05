
#ifndef WALLETFRAME_H
#define WALLETFRAME_H

#include <QFrame>

class BitcoinGUI;
class ClientModel;
class WalletModel;
class WalletStack;
class WalletView;

class WalletFrame : public QFrame
{
    Q_OBJECT

public:
    explicit WalletFrame(BitcoinGUI *_gui = 0);
    ~WalletFrame();

    void setClientModel(ClientModel *clientModel);

    bool addWallet(const QString& name, WalletModel *walletModel);
    bool setCurrentWallet(const QString& name);

    void removeAllWallets();

    bool handleURI(const QString &uri);

    void showOutOfSyncWarning(bool fShow);

private:
    BitcoinGUI *gui;
    ClientModel *clientModel;
    WalletStack *walletStack;

    WalletView *currentWalletView();

public slots:
    
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


