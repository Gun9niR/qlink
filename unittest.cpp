#include "unittest.h"

UnitTest::UnitTest()
{

}

void UnitTest::clearBlockMap(GameWindow &w) {
    for (int r = 0; r < GameWindow::kMaxRows; ++r) {
        for (int c = 0; c < GameWindow::kMaxCols; ++c) {
            w.blockMap[r][c] = new Block(r, c, false, BlockType::kEmpty,
                                         Block::kEmptyBlock,
                                         WhichPlayer::kNoPlayer,
                                         &w);
        }
    }
}

void UnitTest::generateBlock(GameWindow &w, const int r, const int c,
                             const BlockType t,
                             const BlockContent bc,
                             const WhichPlayer which)
{
    Block *b = w.blockMap[r][c];
    b->t = t;
    b->bc = bc;
    b->p = which;
}

void UnitTest::testSuccess()
{
    GameWindow w(UiManager::kUiConfig);
    clearBlockMap(w);

    // Check success in no turn.
    generateBlock(w, 0, 0, BlockType::kBlock, 1, WhichPlayer::kPlayer1);
    generateBlock(w, 0, 1, BlockType::kBlock, 1, WhichPlayer::kPlayer1);
    QVERIFY(w.checkMatch(w.blockMap[0][0], w.blockMap[0][1], nullptr));

    // Check success in one turn.
    generateBlock(w, 1, 2, BlockType::kBlock, 1, WhichPlayer::kPlayer1);
    generateBlock(w, 2, 3, BlockType::kBlock, 1, WhichPlayer::kPlayer1);
    QVERIFY(w.checkMatch(w.blockMap[1][2], w.blockMap[2][3], nullptr));

    // Check success in two turns.
    generateBlock(w, 2, 2, BlockType::kBlock, 1, WhichPlayer::kPlayer1);
    generateBlock(w, 2, 4, BlockType::kBlock, 1, WhichPlayer::kPlayer1);
    QVERIFY(w.checkMatch(w.blockMap[2][2], w.blockMap[2][4], nullptr));
}

void UnitTest::testWrongContent()
{
    GameWindow w(UiManager::kUiConfig);
    clearBlockMap(w);

    generateBlock(w, 0, 0, BlockType::kBlock, 1, WhichPlayer::kPlayer1);
    generateBlock(w, 0, 1, BlockType::kBlock, 2, WhichPlayer::kPlayer1);
    QVERIFY(!w.checkMatch(w.blockMap[0][0], w.blockMap[0][1], nullptr));
}

void UnitTest::testWrongType()
{
    GameWindow w(UiManager::kUiConfig);
    clearBlockMap(w);

    generateBlock(w, 0, 0, BlockType::kItem, 1, WhichPlayer::kPlayer1);
    generateBlock(w, 0, 1, BlockType::kBlock, 1, WhichPlayer::kPlayer1);
    QVERIFY(!w.checkMatch(w.blockMap[0][0], w.blockMap[0][1], nullptr));

    generateBlock(w, 0, 0, BlockType::kEmpty, 1, WhichPlayer::kPlayer1);
    generateBlock(w, 0, 1, BlockType::kBlock, 1, WhichPlayer::kPlayer1);
    QVERIFY(!w.checkMatch(w.blockMap[0][0], w.blockMap[0][1], nullptr));

    generateBlock(w, 0, 0, BlockType::kEmpty, 1, WhichPlayer::kPlayer1);
    generateBlock(w, 0, 1, BlockType::kItem, 1, WhichPlayer::kPlayer1);
    QVERIFY(!w.checkMatch(w.blockMap[0][0], w.blockMap[0][1], nullptr));
}

void UnitTest::testExcessiveTurns()
{
    GameWindow w(UiManager::kUiConfig);
    clearBlockMap(w);

    generateBlock(w, 0, 0, BlockType::kBlock, 1, WhichPlayer::kPlayer1);
    generateBlock(w, 1, 0, BlockType::kBlock, 1, WhichPlayer::kPlayer1);
    generateBlock(w, 0, 2, BlockType::kBlock, 1, WhichPlayer::kPlayer1);
    generateBlock(w, 2, 1, BlockType::kBlock, 1, WhichPlayer::kPlayer1);
    generateBlock(w, 2, 2, BlockType::kBlock, 1, WhichPlayer::kPlayer1);
    generateBlock(w, 2, 3, BlockType::kBlock, 1, WhichPlayer::kPlayer1);
    generateBlock(w, 0, 3, BlockType::kBlock, 1, WhichPlayer::kPlayer1);

    QVERIFY(!w.checkMatch(w.blockMap[0][0], w.blockMap[0][3], nullptr));
}

void UnitTest::testMatchSelf()
{
    GameWindow w(UiManager::kUiConfig);
    clearBlockMap(w);

    generateBlock(w, 0, 0, BlockType::kBlock, 1, WhichPlayer::kPlayer1);
    QVERIFY(!w.checkMatch(w.blockMap[0][0], w.blockMap[0][0], nullptr));
}

void UnitTest::testChosenByDifferentPlayer()
{
    GameWindow w(UiManager::kUiConfig);
    clearBlockMap(w);

    generateBlock(w, 0, 0, BlockType::kBlock, 1, WhichPlayer::kPlayer1);
    generateBlock(w, 0, 1, BlockType::kBlock, 1, WhichPlayer::kPlayer2);
    QVERIFY(!w.checkMatch(w.blockMap[0][0], w.blockMap[0][1], nullptr));
}

