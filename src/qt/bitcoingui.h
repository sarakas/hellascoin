#ifndef BITCOINGUI_H
#define BITCOINGUI_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QMap>

class TransactionTableModel;
class WalletFrame;
class WalletView;
class ClientModel;
class WalletModel;
class WalletStack;
class TransactionView;
class OverviewPage;
class AddressBookPage;
class SendCoinsDialog;
class SignVerifyMessageDialog;
class Notificator;
class RPCConsole;

class CWallet;

QT_BEGIN_NAMESPACE
class QLabel;
class QModelIndex;
class QProgressBar;
class QStackedWidget;
class QUrl;
class QListWidget;
class QPushButton;
class QAction;
QT_END_NAMESPACE


class BitcoinGUI : public QMainWindow
{
    Q_OBJECT

public:
    static const QString DEFAULT_WALLET;

    explicit BitcoinGUI(QWidget *parent = 0);
    ~BitcoinGUI();

    
    void setClientModel(ClientModel *clientModel);
    

    bool addWallet(const QString& name, WalletModel *walletModel);
    bool setCurrentWallet(const QString& name);

    void removeAllWallets();

    

    QAction * getOverviewAction() { return overviewAction; }
    QAction * getHistoryAction() { return historyAction; }
    QAction * getAddressBookAction() { return addressBookAction; }
    QAction * getReceiveCoinsAction() { return receiveCoinsAction; }
    QAction * getSendCoinsAction() { return sendCoinsAction; }

protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    bool eventFilter(QObject *object, QEvent *event);

private:
    ClientModel *clientModel;
    WalletFrame *walletFrame;

    QLabel *labelEncryptionIcon;
    QLabel *labelConnectionsIcon;
    QLabel *labelBlocksIcon;
    QLabel *progressBarLabel;
    QProgressBar *progressBar;

    QMenuBar *appMenuBar;
    QAction *overviewAction;
    QAction *historyAction;
    QAction *quitAction;
    QAction *sendCoinsAction;
    QAction *addressBookAction;
    QAction *signMessageAction;
    QAction *verifyMessageAction;
    QAction *aboutAction;
    QAction *receiveCoinsAction;
    QAction *optionsAction;
    QAction *toggleHideAction;
    QAction *encryptWalletAction;
    QAction *backupWalletAction;
    QAction *changePassphraseAction;
    QAction *aboutQtAction;
    QAction *openRPCConsoleAction;

    QSystemTrayIcon *trayIcon;
    Notificator *notificator;
    TransactionView *transactionView;
    RPCConsole *rpcConsole;

    QMovie *syncIconMovie;
    
    int prevBlocks;

    
    void createActions();
    
    void createMenuBar();
    
    void createToolBars();
    
    void createTrayIcon();
    
    void createTrayIconMenu();
    
    void saveWindowGeometry();
    
    void restoreWindowGeometry();
    
    void setWalletActionsEnabled(bool enabled);

public slots:
    
    void setNumConnections(int count);
    
    void setNumBlocks(int count, int nTotalBlocks);
    
    void setEncryptionStatus(int status);

    
    void message(const QString &title, const QString &message, unsigned int style, bool *ret = NULL);
    
    void askFee(qint64 nFeeRequired, bool *payFee);
    void handleURI(QString strURI);

    
    void incomingTransaction(const QString& date, int unit, qint64 amount, const QString& type, const QString& address);

private slots:
    
    void gotoOverviewPage();
    
    void gotoHistoryPage();
    
    void gotoAddressBookPage();
    
    void gotoReceiveCoinsPage();
    
    void gotoSendCoinsPage(QString addr = "");

    
    void gotoSignMessageTab(QString addr = "");
    
    void gotoVerifyMessageTab(QString addr = "");

    
    void optionsClicked();
    
    void aboutClicked();
#ifndef Q_OS_MAC
    
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
#endif

    
    void showNormalIfMinimized(bool fToggleHidden = false);
    
    void toggleHidden();

    
    void detectShutdown();
};


