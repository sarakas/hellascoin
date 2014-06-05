#include <QTest>
#include <QObject>

#include "uritests.h"


int main(int argc, char *argv[])
{
    bool fInvalid = false;

    URITests test1;
    if (QTest::qExec(&test1) != 0)
        fInvalid = true;

    return fInvalid;
}
