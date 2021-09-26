#include "uimanager.h"

const unique_ptr<UiConfig> UiManager::kUiConfig =
        UiConfigBuilder::windowSize(kWindowHeight, kWindowWidth)
        ->build();

UiManager::UiManager(): startWindow(kUiConfig), gameWindow(kUiConfig)
{
    connect(&startWindow, &StartWindow::sendStartGame,
            this, &UiManager::switchToNewGame);
    connect(&startWindow, &StartWindow::sendLoadGame,
            this, &UiManager::switchToLoadedGame);
    connect(&gameWindow, &GameWindow::sendBackToMenu,
            this, &UiManager::switchToStartWindow);
}

void UiManager::switchToWindow(QWidget *const sender, QWidget *const receiver)
{
    sender->hide();
    receiver->setGeometry(sender->geometry());
    receiver->show();
}

void UiManager::showDefaultWindow()
{
    startWindow.show();
}

void UiManager::switchToStartWindow(QWidget *const sender)
{
    switchToWindow(sender, &startWindow);
}

void UiManager::switchToNewGame(QWidget *const sender, const GameMode mode)
{
    gameWindow.prepareNewGame(mode);
    switchToWindow(sender, &gameWindow);
}

void UiManager::switchToLoadedGame(QWidget *const sender)
{
    gameWindow.prepareSavedGame();
    switchToWindow(sender, &gameWindow);
}
