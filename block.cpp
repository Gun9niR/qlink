#include "block.h"

const QMap<WhichPlayer, QColor> Block::kHighlightColor = {
    { WhichPlayer::kNoPlayer, Qt::white },
    { WhichPlayer::kPlayer1, Qt::cyan },
    { WhichPlayer::kPlayer2, Qt::red }
};

Block::Block(const int r, const int c, const bool markedAsHint,
             const BlockType t, const BlockContent bc, const WhichPlayer p,
             QWidget *parent):
    QPushButton(parent), r(r), c(c), markedAsHint(markedAsHint), t(t),
    bc(bc), p(p)
{

}

Block * Block::fromTextStream(QTextStream &s) {
    int r;
    int c;
    int markedAsHint;
    int t;
    int bc;
    int p;
    Block *b;
    s >> r >> c >> markedAsHint >> t >> bc >> p;
    b = new Block(r, c, markedAsHint, static_cast<BlockType>(t), bc,
                  static_cast<WhichPlayer>(p), nullptr);
    return b;
}

bool Block::isEmpty() const
{
    return t == BlockType::kEmpty;
}

bool Block::isBlock() const
{
    return t == BlockType::kBlock;
}

bool Block::isChosen() const
{
    return p != WhichPlayer::kNoPlayer;
}

bool Block::isInItemRange(const int x, const int y) const {
    const auto &g = this->geometry();
    return t == BlockType::kItem &&
           QRect(g.center().x() - (kItemSize >> 1),
                 g.center().y() - (kItemSize >> 1),
                 kItemSize,
                 kItemSize).contains(x, y);
}

int Block::row() const
{
    return this->r;
}

int Block::col() const
{
    return this->c;
}

BlockType Block::type() const {
    return this->t;
}

BlockContent Block::content() const
{
    return this->bc;
}

WhichPlayer Block::chosenBy() const
{
    return this->p;
}

int Block::boundary(const Direction d) const
{
    auto &g = this->geometry();
    switch (d) {
    case Direction::kUp:
        return g.top();
    case Direction::kDown:
        return g.bottom();
    case Direction::kLeft:
        return g.left();
    case Direction::kRight:
        return g.right();
    }
}

void Block::setPosition(const int r, const int c)
{
    this->r = r;
    this->c = c;
}

void Block::setChosenBy(const WhichPlayer p)
{
    this->p = p;
    this->update();
}

void Block::eliminateSelf()
{
    this->markedAsHint = false;
    this->p = WhichPlayer::kNoPlayer;
    this->bc = kEmptyBlock;
    this->t = BlockType::kEmpty;
    this->update();
}

void Block::paintEvent(QPaintEvent *event)
{
    if (t == BlockType::kBlock) {
        QColor backgroundColor;
        if (this->p != WhichPlayer::kNoPlayer) {
            backgroundColor = kHighlightColor[p];
        } else {
            backgroundColor = Qt::white;
        }
        QString borderStyle = (this->markedAsHint ? "10px solid green; " :
                                                    "1px solid black; ");
        this->setStyleSheet("border: " + borderStyle +
                            "background: " + backgroundColor.name() + "; "
                            "color: black");

        QPushButton::paintEvent(event);
        this->setText(QString::number(this->bc));
    } else if (t == BlockType::kItem) {
        assert(this->geometry().height() >= kItemSize &&
               this->geometry().width() >= kItemSize);
        QPainter painter(this);
        QString text;
        painter.setPen(QPen(Qt::black, 1));
        painter.setBrush(QBrush(Qt::yellow));
        painter.drawRect((this->geometry().width() >> 1) - (kItemSize >> 1),
                         (this->geometry().height() >> 1) - (kItemSize >> 1),
                         kItemSize,
                         kItemSize);

        switch (this->bc) {
        case ItemType::kExtend30s:
            text = "E";
            break;
        case ItemType::kShuffle:
            text = "S";
            break;
        case ItemType::kHint:
            text = "H";
            break;
        }

        painter.drawText(0, 0, this->width(), this->height(), Qt::AlignCenter,
                         text);
    }
    // Do nothing if it is empty.
}

void Block::spawnItem(const ItemType type)
{
    this->markedAsHint = false;
    this->t = BlockType::kItem;
    this->bc = type;
    this->update();
}

void Block::consumeItem() {
    this->markedAsHint = false;
    this->p = WhichPlayer::kNoPlayer;
    this->t = BlockType::kEmpty;
    this->bc = kEmptyBlock;
    this->update();
}

void Block::markAsHint() {
    this->markedAsHint = true;
    this->update();
}

void Block::unmarkAsHint() {
    this->markedAsHint = false;
    try {
        this->update();
    }  catch (...) {
        return;
    }
}

QDebug operator<<(QDebug dbg, const Block &b) {
    dbg << b.r << b.c << b.markedAsHint << b.t << b.bc << b.p;
    return dbg;
}

QTextStream &operator<<(QTextStream &ds, const Block &b)
{
    ds << b.r << ' ' << b.c << ' ' << b.markedAsHint << ' ' << b.t << ' '
       << b.bc << ' ' << b.p;
    return ds;
}

