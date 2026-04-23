#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    MainWindow w;
    
    w.setWindowTitle("Discrete Time PID Controller Simulation");
    
    w.show();

    return a.exec();
}