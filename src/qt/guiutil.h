#ifndef GUIUTIL_H
#define GUIUTIL_H

#include <QString>
#include <QObject>
#include <QMessageBox>

class SendCoinsRecipient;

QT_BEGIN_NAMESPACE
class QFont;
class QLineEdit;
class QWidget;
class QDateTime;
class QUrl;
class QAbstractItemView;
QT_END_NAMESPACE


namespace GUIUtil
{

    QString dateTimeStr(const QDateTime &datetime);
    QString dateTimeStr(qint64 nTime);


    QFont bitcoinAddressFont();


    void setupAddressWidget(QLineEdit *widget, QWidget *parent);
    void setupAmountWidget(QLineEdit *widget, QWidget *parent);



    bool parseBitcoinURI(const QUrl &uri, SendCoinsRecipient *out);
    bool parseBitcoinURI(QString uri, SendCoinsRecipient *out);


    QString HtmlEscape(const QString& str, bool fMultiLine=false);
    QString HtmlEscape(const std::string& str, bool fMultiLine=false);

    
    void copyEntryData(QAbstractItemView *view, int column, int role=Qt::EditRole);
    
    void setClipboard(const QString& str);
    
    
    QString getSaveFileName(QWidget *parent=0, const QString &caption=QString(),
                                   const QString &dir=QString(), const QString &filter=QString(),
                                   QString *selectedSuffixOut=0);

    
    Qt::ConnectionType blockingGUIThreadConnection();


    bool isObscured(QWidget *w);


    void openDebugLogfile();

    
    class ToolTipToRichTextFilter : public QObject
    {
        Q_OBJECT

    public:
        explicit ToolTipToRichTextFilter(int size_threshold, QObject *parent = 0);

    protected:
        bool eventFilter(QObject *obj, QEvent *evt);

    private:
        int size_threshold;
    };

    bool GetStartOnSystemStartup();
    bool SetStartOnSystemStartup(bool fAutoStart);

    
    class HelpMessageBox : public QMessageBox
    {
        Q_OBJECT

    public:
        HelpMessageBox(QWidget *parent = 0);

        
        void showOrPrint();

        
        void printToConsole();

    private:
        QString header;
        QString coreOptions;
        QString uiOptions;
    };




