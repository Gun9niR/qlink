#ifndef UIMANAGER_H
#define UIMANAGER_H

#include "startwindow.h"
#include "gamewindow.h"
#include "uiconfig.h"

// UiManager handles window switching logic, conencts signals among various
// windows.
class UiManager: public QObject {
    Q_OBJECT
private:
    static const int kWindowWidth = 1200;
    static const int kWindowHeight = 630;
    static const unique_ptr<UiConfig> kUiConfig;

    StartWindow startWindow;
    GameWindow gameWindow;

    // A general function that hides <sender> and displays <receiver> at the
    // same screen position.
    void switchToWindow(QWidget *const sender, QWidget *const receiver);
public:
    UiManager();

    // Defualt window is StartWindow.
    void showDefaultWindow();

private slots:
    void switchToStartWindow(QWidget *const sender);
    void switchToNewGame(QWidget *const sender, const GameMode mode);
    void switchToLoadedGame(QWidget *const sender);
};

#endif // UIMANAGER_H
