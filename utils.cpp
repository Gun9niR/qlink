#include "utils.h"

#include "includes.h"

void Utils::centerWindowInScreen(QWidget *widget)
{
    widget->move(QGuiApplication::primaryScreen()->geometry().center() -
                 widget->frameGeometry().center());
}

int Utils::randomInt(int min, int max)
{
    static std::random_device rd;
    static std::mt19937 rng(rd());
    std::uniform_int_distribution<int> uni(min, max - 1);
    return uni(rng);
}

void Utils::removeAllWidgets(QLayout *const layout)
{
    QLayoutItem *item;
    while ((item = layout->takeAt(0))) {
        delete item->widget();
        delete item;
    }
}

void Utils::removeAllWidgets(QWidget *const widget)
{
    qDeleteAll(widget->findChildren<QWidget*>("", Qt::FindDirectChildrenOnly));
}
