#ifndef MONITOREDDATAMAPPER_H
#define MONITOREDDATAMAPPER_H

#include <QDataWidgetMapper>

QT_BEGIN_NAMESPACE
class QWidget;
QT_END_NAMESPACE


class MonitoredDataMapper : public QDataWidgetMapper
{
    Q_OBJECT

public:
    explicit MonitoredDataMapper(QObject *parent=0);

    void addMapping(QWidget *widget, int section);
    void addMapping(QWidget *widget, int section, const QByteArray &propertyName);

private:
    void addChangeMonitor(QWidget *widget);

signals:
    void viewModified();
};


