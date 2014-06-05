#ifndef MACNOTIFICATIONHANDLER_H
#define MACNOTIFICATIONHANDLER_H
#include <QObject>


class MacNotificationHandler : public QObject
{
    Q_OBJECT

public:
    
    void showNotification(const QString &title, const QString &text);

    
    void sendAppleScript(const QString &script);

    
    bool hasUserNotificationCenterSupport(void);
    static MacNotificationHandler *instance();
};



