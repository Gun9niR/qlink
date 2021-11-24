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

    // Logo and author
    QLabel *logoLbl = new QLabel("QLink");
    Utils::setWidgetFontSize(logoLbl, 100);
    logoLbl->adjustSize();
    logoLbl->setFixedHeight(logoLbl->geometry().height());
    logoLbl->setAlignment(Qt::AlignHCenter);


    QLabel *authorLbl = new QLabel("By GZD");
    Utils::setWidgetFontSize(authorLbl, 50);
    authorLbl->adjustSize();
    authorLbl->setFixedHeight(authorLbl->geometry().height());
    authorLbl->setAlignment(Qt::AlignHCenter);

    outmostLayout->addWidget(logoLbl);
    outmostLayout->addSpacing(-100);
    outmostLayout->addWidget(authorLbl);

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
    connect(quitBtn, &QPushButton::clicked,
            this, &StartWindow::onClickQuit);

    // The container for all four buttons.
    btnLayout->addWidget(singlePlayerBtn);
    btnLayout->addWidget(multiPlayerBtn);
    btnLayout->addWidget(loadBtn);
    btnLayout->addWidget(quitBtn);

    // Set layout relations.
    outmostLayout->addLayout(btnLayout);

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
    QApplication::exit();
}
