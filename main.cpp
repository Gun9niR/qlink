#include "uimanager.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    UiManager w;
    w.showDefaultWindow();

    return a.exec();
}
