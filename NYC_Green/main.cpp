#include "nyc_green_view.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    NYC_Green w;
    w.show();
    return a.exec();
}