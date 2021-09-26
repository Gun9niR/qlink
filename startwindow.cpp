#include "startwindow.h"
#include "utils.h"

StartWindow::StartWindow(const unique_ptr<UiConfig> &config, QWidget *parent):
QWidget(parent), config(config)
{
    // Set fixed window size.
    setFixedSize(config->windowWidth(), config->windowHeight());

    // Center window.
    Utils::centerWindowInScreen(this);

    // Draw layout.
    initLayout();
}

void StartWindow::initLayout()
{
    QVBoxLayout *outmostLayout = new QVBoxLayout();
    QHBoxLayout *btnLayout = new QHBoxLayout();

    // Set layout relations.
    outmostLayout->addLayout(btnLayout);

    // Declare the four buttons.
    QPushButton *singlePlayerBtn = new QPushButton("Single player");
    QPushButton *multiPlayerBtn = new QPushButton("Multiplayer");
    QPushButton *loadBtn = new QPushButton("Load");
    QPushButton *quitBtn = new QPushButton("Quit");

    // Add handlers for four buttons.
    connect(singlePlayerBtn, &QPushButton::clicked,
            this, &StartWindow::onClickSinglePlayer);
    connect(multiPlayerBtn, &QPushButton::clicked,
            this, &StartWindow::onClickMultiPlayer);
    connect(loadBtn, &QPushButton::clicked,
            this, &StartWindow::onClickLoad);

    // The container for all four buttons.
    btnLayout->addWidget(singlePlayerBtn);
    btnLayout->addWidget(multiPlayerBtn);
    btnLayout->addWidget(loadBtn);
    btnLayout->addWidget(quitBtn);

    this->setLayout(outmostLayout);
}

void StartWindow::onClickSinglePlayer()
{
    emit sendStartGame(this, GameMode::kSingle);
}

void StartWindow::onClickMultiPlayer()
{
    emit sendStartGame(this, GameMode::kDouble);
}

void StartWindow::onClickLoad()
{
    emit sendLoadGame(this);
}

void StartWindow::onClickQuit()
{
    QApplication::quit();
}
