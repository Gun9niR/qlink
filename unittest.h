#ifndef UNITTEST_H
#define UNITTEST_H

#include "includes.h"
#include "uimanager.h"

class UnitTest: public QObject
{
    Q_OBJECT
private:
    // Utility function to set <blockMap> in GameWindow to
    // all empty.
    void clearBlockMap(GameWindow &w);

    // Utility function to generate a block at <r>, <c> on blockMap of <w>,
    // with type being <t>, content being <bc> and choosing player being <which>.
    void generateBlock(GameWindow &w, const int r, const int c,
                       const BlockType t,
                       const BlockContent bc,
                       const WhichPlayer which);

private slots:
    void testSuccess();

    void testWrongContent();

    void testWrongType();

    void testExcessiveTurns();

    void testMatchSelf();

    void testChosenByDifferentPlayer();

public:
    UnitTest();
};

#endif // UNITTEST_H
