#ifndef NOTIFICATOR_H
#define NOTIFICATOR_H

#include <QObject>
#include <QIcon>

QT_BEGIN_NAMESPACE
class QSystemTrayIcon;
#ifdef USE_DBUS
class QDBusInterface;
#endif
QT_END_NAMESPACE


class Notificator: public QObject
{
    Q_OBJECT

public:
    
    Notificator(const QString &programName=QString(), QSystemTrayIcon *trayIcon=0, QWidget *parent=0);
    ~Notificator();


    enum Class
    {
        Information,    
        Warning,        
        Critical        
    };

public slots:
    
    void notify(Class cls, const QString &title, const QString &text,
                const QIcon &icon = QIcon(), int millisTimeout = 10000);

private:
    QWidget *parent;
    enum Mode {
        None,                       
        Freedesktop,                
        QSystemTray,                
        Growl12,                    
        Growl13,                    
        UserNotificationCenter      
    };
    QString programName;
    Mode mode;
    QSystemTrayIcon *trayIcon;
#ifdef USE_DBUS
    QDBusInterface *interface;

    void notifyDBus(Class cls, const QString &title, const QString &text, const QIcon &icon, int millisTimeout);
#endif
    void notifySystray(Class cls, const QString &title, const QString &text, const QIcon &icon, int millisTimeout);
#ifdef Q_OS_MAC
    void notifyGrowl(Class cls, const QString &title, const QString &text, const QIcon &icon);
    void notifyMacUserNotificationCenter(Class cls, const QString &title, const QString &text, const QIcon &icon);
#endif
};


