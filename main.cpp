#include "oasis_pro.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    oasis_pro w;
    w.show();
    return a.exec();
}
