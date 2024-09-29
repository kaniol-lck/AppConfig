#include "configexample.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("kaniol");
    QCoreApplication::setOrganizationDomain("kaniol.net");
    QCoreApplication::setApplicationName("Config Example");
    ConfigExample w;
    w.show();
    return a.exec();
}
