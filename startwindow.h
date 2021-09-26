#ifndef STARTWINDOW_H
#define STARTWINDOW_H

#include "types.h"
#include "uiconfig.h"

// The window that gets displayed right after program starts.
class StartWindow : public QWidget
{
    Q_OBJECT
private:
    const unique_ptr<UiConfig> &config;

public:
    StartWindow(const unique_ptr<UiConfig> &config, QWidget *parent = nullptr);
    void initLayout();

private slots:
    void onClickSinglePlayer();
    void onClickMultiPlayer();
    void onClickLoad();
    void onClickQuit();

signals:
    void sendStartGame(QWidget *const sender, const GameMode mode);
    void sendLoadGame(QWidget *const sender);
};
#endif // STARTWINDOW_H
