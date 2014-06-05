#ifndef BITCOINUNITS_H
#define BITCOINUNITS_H

#include <QString>
#include <QAbstractListModel>


class BitcoinUnits: public QAbstractListModel
{
    Q_OBJECT

public:
    explicit BitcoinUnits(QObject *parent);

    
    enum Unit
    {
        BTC,
        mBTC,
        uBTC
    };






    static QList<Unit> availableUnits();

    static bool valid(int unit);

    static QString name(int unit);

    static QString description(int unit);

    static qint64 factor(int unit);

    static int amountDigits(int unit);

    static int decimals(int unit);

    static QString format(int unit, qint64 amount, bool plussign=false);

    static QString formatWithUnit(int unit, qint64 amount, bool plussign=false);

    static bool parse(int unit, const QString &value, qint64 *val_out);





    enum RoleIndex {
        
        UnitRole = Qt::UserRole
    };
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;


private:
    QList<BitcoinUnits::Unit> unitlist;
};
typedef BitcoinUnits::Unit BitcoinUnit;


