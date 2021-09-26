#include "player.h"

Player::Player(const WhichPlayer &which, const int x, const int y, const int s,
               QWidget *parent):
    QWidget(parent), kId(which), x(x), y(y), score(s),
    color(Block::kHighlightColor[which]), chosenBlock(nullptr)
{
    setFixedSize(SHAPE_SIZE, SHAPE_SIZE);
}

void Player::paintEvent(QPaintEvent *)
{
    // TODO: change to something nicer.
    this->move(x - (SHAPE_SIZE >> 1), y - (SHAPE_SIZE >> 1));
    QPainter painter(this);
    painter.setBrush(this->color);
    painter.setPen(this->color);
    painter.drawRect(0, 0, SHAPE_SIZE, SHAPE_SIZE);
}

Player * Player::fromTextStream(QTextStream &s)
{
    int id;
    int x;
    int y;
    int score;
    Player *p;
    s >> id >> x >> y >> score;
    p = new Player(static_cast<WhichPlayer>(id), x, y, score);
    return p;
}

void Player::chooseBlock(Block *const b)
{
    // Check if the block is already chosen by another player.
    if (b->isChosen() && b->chosenBy() != kId) {
        return;
    }

    b->setChosenBy(this->kId);
    if (chosenBlock == nullptr) {
        chosenBlock = b;
        chosenBlock->setChosenBy(this->kId);
    } else if (b != chosenBlock) {
        emit sendValidateBlock(kId, chosenBlock, b);
        removeChosenBlock();
    }
}

void Player::removeChosenBlock()
{
    chosenBlock = nullptr;
}

void Player::addScore(const int dscore)
{
    this->score += dscore;
    emit sendScoreChanged(this->kId, this->score);
}

void Player::setScore(const int score)
{
    this->score = score;
    emit sendScoreChanged(this->kId, this->score);
}

QDebug operator<<(QDebug dbg, const Player &s)
{
    dbg.nospace() << "x: " << s.x << ", y: " << s.y;
    return dbg;
}

QTextStream & operator<<(QTextStream &ds, const Player &p)
{
    ds << p.kId << ' ' << p.x << ' ' << p.y << ' ' << p.score;
    return ds;
}

