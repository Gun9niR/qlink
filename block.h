#ifndef BLOCK_H
#define BLOCK_H

#include "includes.h"
#include "types.h"

class GameWindow;
class UnitTest;

// The basic element that constitutes the map. Manages both the logic and ui of
// map unit operations.
class Block: public QPushButton {
    friend class UnitTest;
    friend class GameWindow;
    friend QDebug operator<<(QDebug dbg, const Block &b);
    friend QTextStream &operator<<(QTextStream &ds, const Block &b);

private:
    // Row of this block in the map.
    int r;

    // Column of this block in the map.
    int c;

    // Indicates if this block has been chosen to be highlighted as hint.
    bool markedAsHint;

    // Type of this block. Decides how the value of bc is interpreted.
    BlockType t;

    // When t == kEmpty, the value is always 0, when t == kBlock, the value
    // indicates which group this block belongs to, when t == kItem, the value
    // indicates what kind of item this is.
    BlockContent bc;

    // The player that has chosen this block.
    WhichPlayer p;

public:
    static const BlockContent kEmptyBlock = 0;
    static const int kItemSize = 20;
    static const QMap<WhichPlayer, QColor> kHighlightColor;

    Block(const int r,
          const int c,
          const bool markedAsHint = false,
          const BlockType t = BlockType::kEmpty,
          const BlockContent bc = Block::kEmptyBlock,
          const WhichPlayer p = WhichPlayer::kNoPlayer,
          QWidget *parent = nullptr);
    static Block * fromTextStream(QTextStream &s);

    // Returns true only if this block is neither block (type) or item.
    bool isEmpty() const;

    // Returns true only if this block is block (type).
    bool isBlock() const;

    // Returns true only if this block is chosen by player 1 or player 2.
    bool isChosen() const;

    // Return true only if given point (x, y) in in the geometric range of the item
    // drawn on this block.
    bool isInItemRange(const int x, const int y) const;

    // Returns r member.
    int row() const;

    // Returns c member.
    int col() const;

    // Returns t member.
    BlockType type() const;

    // Returns bc member.
    BlockContent content() const;

    // Returns p member.
    WhichPlayer chosenBy() const;

    // A wrapping function for geometry().left()/right()/top()/bottom() that
    // accepts custom Direction type as argument.
    int boundary(const Direction) const;

    // Sets the row and column of this block in block map.
    void setPosition(const int r, const int c);

    // Sets the p field.
    void setChosenBy(const WhichPlayer p);

    // Mark this block as empty, clear its contents.
    void eliminateSelf();

    // Mark this block as item, given the item type.
    void spawnItem(const ItemType type);

    // Just an alias for eliminate self for readability.
    void consumeItem();

    // Mark this block as being highlighted for hint, main purpose is to
    // encapsulate invocation of update().
    void markAsHint();

    // Mark this block as not being highlighted for hint, main purpose is to
    // encapsulate invocation of update().
    void unmarkAsHint();

protected:
    // Draw the widget depending on the combination of its fields.
    void paintEvent(QPaintEvent *event) override;
};

#endif // BLOCK_H
