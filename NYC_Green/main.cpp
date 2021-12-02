#include "nyc_green.h"
#include <QtWidgets/QApplication>
#include <qlabel.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    NYC_Green w;
    w.show();
    return a.exec();
}
