#include "prhp.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("kaniol");
    QCoreApplication::setOrganizationDomain("kaniol.net");
    QCoreApplication::setApplicationName("Preferences Helper");
    PrHp w;
    w.show();
    return a.exec();
}
