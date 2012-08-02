#include <QtGui/QApplication>
#include "workbreak.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setApplicationName(APP_NAME);
    a.setApplicationVersion(APP_VERSION);
    a.setOrganizationDomain("yarpen.cz");
    a.setOrganizationName("Petr Vanek");

    a.setQuitOnLastWindowClosed(false);

    WorkBreak w;
    w.show();
    return a.exec();
}
