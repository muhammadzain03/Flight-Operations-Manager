#include "gui/MainWindow.h"

#include <QApplication>

// This is the main entry point for the Flight Operations Manager application.
// It initializes the QApplication, sets application metadata, creates and shows
// the main window, and starts the application's event loop.

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
