#include "uimanager.h"
#include "unittest.h"

int main(int argc, char *argv[])
{
    // To run test, uncomment next line and comment the rest of main().
    // QTEST_MAIN_IMPL(UnitTest);

    QApplication a(argc, argv);
    int id = QFontDatabase::addApplicationFont(
                ":/fonts/Montserrat-Regular.ttf");
    QString fontFamily = QFontDatabase::applicationFontFamilies(id).at(0);
    QFont montserrat(fontFamily);
    a.setFont(montserrat);

    UiManager w;
    w.showDefaultWindow();

    return a.exec();
}
