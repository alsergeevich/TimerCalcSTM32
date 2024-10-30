#include "timercalculation.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TimerCalculation w;
    w.show();
    return a.exec();
}
