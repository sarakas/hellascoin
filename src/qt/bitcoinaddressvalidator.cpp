#include "bitcoinaddressvalidator.h"



BitcoinAddressValidator::BitcoinAddressValidator(QObject *parent) :
    QValidator(parent)
{
}

QValidator::State BitcoinAddressValidator::validate(QString &input, int &pos) const
{

    for(int idx=0; idx<input.size();)
    {
        bool removeChar = false;
        QChar ch = input.at(idx);



        switch(ch.unicode())
        {



            removeChar = true;
            break;
        default:
            break;
        }

        if(ch.isSpace())
            removeChar = true;

        if(removeChar)
            input.remove(idx, 1);
        else
            ++idx;
    }


    QValidator::State state = QValidator::Acceptable;
    for(int idx=0; idx<input.size(); ++idx)
    {
        int ch = input.at(idx).unicode();

        if(((ch >= '0' && ch<='9') ||
           (ch >= 'a' && ch<='z') ||
           (ch >= 'A' && ch<='Z')) &&
           ch != 'l' && ch != 'I' && ch != '0' && ch != 'O')
        {

        }
        else
        {
            state = QValidator::Invalid;
        }
    }


    if(input.isEmpty())
    {
        state = QValidator::Intermediate;
    }

    return state;
}
