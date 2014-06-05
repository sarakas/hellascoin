#ifndef ADDRESSTABLEMODEL_H
#define ADDRESSTABLEMODEL_H

#include <QAbstractTableModel>
#include <QStringList>

class AddressTablePriv;
class CWallet;
class WalletModel;


class AddressTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit AddressTableModel(CWallet *wallet, WalletModel *parent = 0);
    ~AddressTableModel();

    enum ColumnIndex {
        Label = 0,   
        Address = 1  
    };

    enum RoleIndex {
        TypeRole = Qt::UserRole 
    };

    
    enum EditStatus {
        OK,                     
        NO_CHANGES,             
        INVALID_ADDRESS,        
        DUPLICATE_ADDRESS,      
        WALLET_UNLOCK_FAILURE,  
        KEY_GENERATION_FAILURE  
    };

    static const QString Send;      
    static const QString Receive;   

    
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    Qt::ItemFlags flags(const QModelIndex &index) const;
    

    
    QString addRow(const QString &type, const QString &label, const QString &address);

    
    QString labelForAddress(const QString &address) const;

    
    int lookupAddress(const QString &address) const;

    EditStatus getEditStatus() const { return editStatus; }

private:
    WalletModel *walletModel;
    CWallet *wallet;
    AddressTablePriv *priv;
    QStringList columns;
    EditStatus editStatus;

    
    void emitDataChanged(int index);

signals:
    void defaultAddressChanged(const QString &address);

public slots:
    
    void updateEntry(const QString &address, const QString &label, bool isMine, int status);

    friend class AddressTablePriv;
};


