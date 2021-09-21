#ifndef STARTWINDOW_H
#define STARTWINDOW_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class StartWindow; }
QT_END_NAMESPACE

class StartWindow : public QWidget
{
    Q_OBJECT

public:
    StartWindow(QWidget *parent = nullptr);
    ~StartWindow();

private:
    Ui::StartWindow *ui;
};
#endif // STARTWINDOW_H
