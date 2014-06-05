



#ifndef BITCOIN_UI_INTERFACE_H
#define BITCOIN_UI_INTERFACE_H

#include <string>

#include <boost/signals2/signal.hpp>
#include <boost/signals2/last_value.hpp>

class CBasicKeyStore;
class CWallet;
class uint256;


enum ChangeType
{
    CT_NEW,
    CT_UPDATED,
    CT_DELETED
};


class CClientUIInterface
{
public:
    
    enum MessageBoxFlags
    {
        ICON_INFORMATION    = 0,
        ICON_WARNING        = (1U << 0),
        ICON_ERROR          = (1U << 1),
        
        ICON_MASK = (ICON_INFORMATION | ICON_WARNING | ICON_ERROR),

        












        
        BTN_MASK = (BTN_OK | BTN_YES | BTN_NO | BTN_ABORT | BTN_RETRY | BTN_IGNORE |
                    BTN_CLOSE | BTN_CANCEL | BTN_DISCARD | BTN_HELP | BTN_APPLY | BTN_RESET),

        
        MODAL               = 0x10000000U,

        
        MSG_INFORMATION = ICON_INFORMATION,
        MSG_WARNING = (ICON_WARNING | BTN_OK | MODAL),
        MSG_ERROR = (ICON_ERROR | BTN_OK | MODAL)
    };

    
    boost::signals2::signal<bool (const std::string& message, const std::string& caption, unsigned int style), boost::signals2::last_value<bool> > ThreadSafeMessageBox;

    
    boost::signals2::signal<bool (int64 nFeeRequired), boost::signals2::last_value<bool> > ThreadSafeAskFee;

    
    boost::signals2::signal<void (const std::string& strURI)> ThreadSafeHandleURI;

    
    boost::signals2::signal<void (const std::string &message)> InitMessage;

    
    boost::signals2::signal<std::string (const char* psz)> Translate;

    
    boost::signals2::signal<void ()> NotifyBlocksChanged;

    
    boost::signals2::signal<void (int newNumConnections)> NotifyNumConnectionsChanged;

    
    boost::signals2::signal<void (const uint256 &hash, ChangeType status)> NotifyAlertChanged;
};

extern CClientUIInterface uiInterface;


inline std::string _(const char* psz)
{
    boost::optional<std::string> rv = uiInterface.Translate(psz);
    return rv ? (*rv) : psz;
}

#endif
