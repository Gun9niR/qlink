#ifndef PLAYER_H
#define PLAYER_H

#include "block.h"
#include "types.h"
#include "includes.h"

class GameWindow;

// Manages both the ui and logic on a player.
class Player: public QWidget {
    Q_OBJECT

    friend class GameWindow;
    friend QDebug operator<<(QDebug dbg, const Player &s);
    friend QTextStream & operator<<(QTextStream &ds, const Player &p);

private:
    //  The size of the rectangle when the player is drawn.
    static const int SHAPE_SIZE = 20;

    // Identifies which player this is.
    const WhichPlayer kId;

    // x and y is the coordinates of the center of the character.
    int x;
    int y;

    // Score of this player.
    int score;

    // Color of this player.
    QColor color;

    // The block that this player has chosen and highlighted with the player's
    // own color.
    Block *chosenBlock;

    Player(const WhichPlayer &which, const int x, const int y, const int s = 0,
           QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

public:
    static Player * fromTextStream(QTextStream &s);

    void chooseBlock(Block *const b);
    void removeChosenBlock();
    void addScore(const int dscore);
    void setScore(const int score);

signals:
    void sendValidateBlock(const WhichPlayer which,
                           Block *const b1,
                           Block *const b2);
    void sendHighlightBlock(const WhichPlayer which,
                            Block *const block);
    void sendScoreChanged(const WhichPlayer which, const int newScore);
};
#endif // PLAYER_H
