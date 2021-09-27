#include "uimanager.h"
#include "unittest.h"

int main(int argc, char *argv[])
{
    // To run test, uncomment next line and comment the rest of main().
    // QTEST_MAIN_IMPL(UnitTest);

    QApplication a(argc, argv);
    UiManager w;
    w.showDefaultWindow();

    return a.exec();
}
