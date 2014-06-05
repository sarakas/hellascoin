#ifndef ADDRESSBOOKPAGE_H
#define ADDRESSBOOKPAGE_H

#include <QDialog>

namespace Ui {
    class AddressBookPage;
}
class AddressTableModel;
class OptionsModel;

QT_BEGIN_NAMESPACE
class QTableView;
class QItemSelection;
class QSortFilterProxyModel;
class QMenu;
class QModelIndex;
QT_END_NAMESPACE


class AddressBookPage : public QDialog
{
    Q_OBJECT

public:
    enum Tabs {
        SendingTab = 0,
        ReceivingTab = 1
    };

    enum Mode {
        ForSending, 
        ForEditing  
    };

    explicit AddressBookPage(Mode mode, Tabs tab, QWidget *parent = 0);
    ~AddressBookPage();

    void setModel(AddressTableModel *model);
    void setOptionsModel(OptionsModel *optionsModel);
    const QString &getReturnValue() const { return returnValue; }

public slots:
    void done(int retval);

private:
    Ui::AddressBookPage *ui;
    AddressTableModel *model;
    OptionsModel *optionsModel;
    Mode mode;
    Tabs tab;
    QString returnValue;
    QSortFilterProxyModel *proxyModel;
    QMenu *contextMenu;

    QString newAddressToSelect;

private slots:
    
    void on_deleteAddress_clicked();
    
    void on_newAddress_clicked();
    
    void on_copyAddress_clicked();
    
    void on_signMessage_clicked();
    
    void on_verifyMessage_clicked();
    
    void onSendCoinsAction();
    
    void on_showQRCode_clicked();
    
    void onCopyLabelAction();
    
    void onEditAction();
    
    void on_exportButton_clicked();

    
    void selectionChanged();
    
    void contextualMenu(const QPoint &point);
    
    void selectNewAddress(const QModelIndex &parent, int begin, int );

signals:
    void signMessage(QString addr);
    void verifyMessage(QString addr);
    void sendCoins(QString addr);
};


