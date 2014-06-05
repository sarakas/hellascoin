#ifndef BITCOINAMOUNTFIELD_H
#define BITCOINAMOUNTFIELD_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QDoubleSpinBox;
class QValueComboBox;
QT_END_NAMESPACE


class BitcoinAmountField: public QWidget
{
    Q_OBJECT

    Q_PROPERTY(qint64 value READ value WRITE setValue NOTIFY textChanged USER true)

public:
    explicit BitcoinAmountField(QWidget *parent = 0);

    qint64 value(bool *valid=0) const;
    void setValue(qint64 value);

    
    void setValid(bool valid);
    
    bool validate();

    
    void setDisplayUnit(int unit);

    
    void clear();


        in these cases we have to set it up manually.
    */
    QWidget *setupTabChain(QWidget *prev);

signals:
    void textChanged();

protected:
    
    bool eventFilter(QObject *object, QEvent *event);

private:
    QDoubleSpinBox *amount;
    QValueComboBox *unit;
    int currentUnit;

    void setText(const QString &text);
    QString text() const;

private slots:
    void unitChanged(int idx);

};


