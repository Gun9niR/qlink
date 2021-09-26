#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include "block.h"
#include "player.h"
#include "qlinkmap.h"
#include "types.h"
#include "uiconfig.h"

// Link list node that is used in dfs, to generate a path that connects two
// matching blocks.
typedef struct DirectionNode {
    const Direction d;
    const shared_ptr<DirectionNode> prev;
    DirectionNode(const Direction d,
            const shared_ptr<DirectionNode> &p = nullptr):
          d(d), prev(p) { }
} DirectionNode;

// Core class of the game that manages ui layout of the game, keeps track of
// game status, player status, time, items, core logic (item effect, block
// elimination, solvability) of the game.
class GameWindow: public QWidget
{
    Q_OBJECT

    friend class Player;

private:
    // Number of groups all the blocks fall into. Blocks of the same group can
    // eliminate one another.
    static const int kTypeNum = 5;

    // Initial remaining time when the game starts.
    static const int kInitialTime = 60;

    // Number of rows in the map.
    static const int kMaxRows = 15;

    // Number of columns in the map.
    static const int kMaxCols = 30;

    // Number of blocks in the map. It's value must be kMaxRows * kMaxCols.
    static const int kBlockNum = 200;

    // Number of blocks each group. Each group has equal number of blocks at the
    // beginning.
    static const int kBlocksPerType = kBlockNum / kTypeNum;

    // Maximum number of iterations for generating player position. A player
    // generation attempt might fail in the sense that it is surrounded by
    // blocks of different type, or is positioned on a block that is occupied.
    static const int kMaxGeneratePlayerTrials = 1000;

    // Ui size configurations.
    static const int kStatusBarHeight = 30;
    static const int kMapHeight = 600;
    static const int kMapWidth = 1200;

    // Number of pixels (in the sense of x, y) that the player moves on each
    // keypress detection.
    static const int kMoveStep = 2;

    // The number of scores that a player gets each time a match is found.
    static const int kScorePerMatch = 5;

    // Maximum number of turns for a link that connects two blocks of the same
    // type to eliminate them.
    static const int kMaxTurns = 2;

    // Number of milliseconds for which a link is displayed on each mathing.
    static const int kShowConnectionDurationMsec = 1000;

    // Interval of key press detection.
    static const int kKeyPressIntervalMSec = 20;

    // Number of milliseconds for which hints keep showing up. That means if
    // a highlighted pair is eliminated, another pair is chosen and highlighted.
    static const int kHintTimeMsec = 10000;

    // The probability (percent) that a new item is spawned at a random position
    // on the map each second.
    static const int kSpawnItemProbability = 40;

    // Player independednt key mappings.
    static const int kPauseKey = 'P';
    static const int kSaveKey = 'S';

    // Player specific key mappings.
    static const QMap<WhichPlayer, QMap<Direction, int>> kKeyMapping;

    // ============================================================
    //
    // Ui related fields and functions.
    //
    // ============================================================

    // Window UI configurations. Using a another class for extendibility.
    const unique_ptr<UiConfig> &WINDOW_CONFIG;

    // Block size.
    const int BLOCK_HEIGHT;
    const int BLOCK_WIDTH;

    // Layout of the top status bar.
    QHBoxLayout *statusLayout;

    // Widget of the map.
    QLinkMap *mapLayout;

    // The labels in status bar that displays score for each player.
    QMap<WhichPlayer, QLabel *> scoreLbls;

    // The label in status bar that displays remaining number.
    QLabel *timeLbl;

    // Must only be called once. Allocates status bar, map, set parent
    // relations.
    void initLayout();

    // Remove all widgets from status bar and map. Intended for restart.
    void resetLayout();

    // Draw top status bar depending on game <mode>.
    void drawStatusBar(const GameMode &mode);

    // Reposition each block depending on their row and column.
    // This funciton does not handle the drawing of a block, which is handled by
    // the block itself.
    // Must be called after invocation of generateMap().
    void drawMap();

    // Draw the conenction from a given block <from>, using the <path> parameter
    // that provides a sequence of directions of each step. Returns a uuid of
    // the set of lines created by this invocation, so that they can be erased
    // later.
    QUuid drawConnection(Block *const from,
                        const QList<Direction> &path);

    // Clear the set of lines with previosuly described <uuid>.
    void clearConnection(const QUuid &uuid);

    // Prompt the user that a new game has started and he/she should press
    // any key to start.
    void promptStart();

    // Prompt the user that the game is paused.
    void promptPause();

    // Prompt the user that a saved game is loaded and he/she should press any
    // key to start.
    void promtResumeFromLoad();

    // Prompt the user that he/she has eliminated all blocks. Only used in
    // single player mode.
    void promptSuccess();

    // Prompt tha user that no more pair of blocks can be eliminated.
    void promptStuck();

    // Promt the user that time's up.
    void promptTimesUp();

    // Promt the users that one player has won, or there's a tie. Only used in
    // multiplayer mode.
    void promptOnePlayerWins();

    // ============================================================
    //
    // Game logic related fields and functions.
    //
    // ============================================================

    // Indicates whether the game is ready to start, started, paused, stopped,
    // stuck, etc.
    GameStatus status;

    // Single or double player (i.e. multiplayer) mode.
    GameMode mode;

    // A 2D vector that contains all block units.
    QVector<QVector<Block *>> blockMap;

    // Use the enum WhichPlayer to index player object for more clarity.
    QMap<WhichPlayer, Player *> players;

    // A timer that times out every one second, to handle events that happen
    // every second.
    QTimer *countDownTimer;

    // A timer that times out very rapidly, exclusively intended to check if any
    // key is pressed.
    QTimer *keyPressTimer;

    // A timer that times out when hint item has expired.
    QTimer *hintTimer;

    // Number of blocks that has not been eliminated.
    int blocksRemaining;

    // The number of seconds remaining before time runs out.
    int timeRemaining;

    // Indicates whether the hint item has been activated.
    bool hint;

    // The number of milliseconds remaining before hint item expires.
    int hintTimeRemaining;

    // Keeps track of the keys that has been pressed and not yet released.
    // Mainly used by keyPressTimer.
    QSet<int> pressedKeys;

    // The pair of blocks that are highlighted as hint. Mainly for removal of
    // hints that are caused by reasons other than block elimination, such as
    // a new hint item is activated while the previous one has not expired.
    QPair<Block *, Block *> hintPair;

    // Get the string to be displayed on score label from actual <score> value.
    static QString getScoreString(const int score);

    // Get the string to be displayed on time label from actual <sec> value.
    static QString getTimeString(const int sec);

    // Populateate map array. It doesn't check if the map if solvable
    // programmatically, the block number and map size should be designed to
    // ensure it.
    void generateMap();

    // Generate character position. Returns true if there's a valid position for
    // the character, i.e. the character is not surrounded by four distinct
    // blocks or is not on an occupied block. Player is always positioned at the
    // center of the block.
    bool generatePlayer(const WhichPlayer &which);

    // Connect signals related to player objects. Must be called after the
    // player(s) are generated.
    void connectPlayerSignals(const GameMode mode);

    // Game status control functions. Very self-explanatory.
    void startGame();
    void pauseGame();
    void resumeGame();
    void stopGame();

    // Spawn an item of random type at the center of random unoccupied block.
    void spawnItem();

    // Invoked when player has geometrically 'touched' the item in <block>,
    // remove this item visually and logically.
    void consumeItem(Block *const block);

    // Shuffle all the blocks and items on the map. If hint is enabled, a new
    // pair of hint will be generated.
    void shuffle();

    // Enable hint for <t> seconds, during which pairs of blocks will constantly
    // be highlighted.
    void enableHint(const int t);

    // Find a pair of blocks that can be matched and be reached by the player
    // (either player 1 or player 2).
    void generateHint();

    // Remove current highlighted blocks both visually and logically.
    void removeCurrentHint();

    // Move player 1 or player 2 decided by <which> in the direction of <d>.
    // When player hits two blocks at the same time, need to decide which block
    // it has actually hitten.
    void movePlayer(const WhichPlayer which, const Direction &d);

    // Invoked when player hits two blocks at the same time. In the case, the
    // player is considered to choose the block whose geometric center is closer
    // to the player's geometric center.
    Block * distinguishCollision(
            Player *player,
            Block *const block1,
            Block *const block2,
            const Direction d);

    // Returns true if the given block at point (<x>, <y>) is in an occupied
    // block. If true, <buffer> will be set to the occupying block.
    // A special case is when the character hits the boundary, where the return
    // value is true, but buffer is still null.
    bool checkCollision(const int x, const int y, Block *&buffer);

    // Returns true if the given block at point (<x>, <y>) is in an item. If
    // true, <buffer> will be set to that item block.
    bool checkItem(const int x, const int y, Block *&buffer);

    // Given to blocks, check if they can be connected within <kMaxTurns> turns,
    // and if path is not null, return the path that connects two blocks.
    bool checkConnectivity(Block *const from,
                           Block *const to,
                           QList<Direction> *path);

    // dfs algorithm that `checkConnectivity` uses.
    // Constucts a list of DirectionNode, which can be used to recover a path
    // from <from> block to <to> block.
    shared_ptr<DirectionNode> dfs(const int turns,
                                  Block *const from,
                                  Block *const to,
                                  QVector<QVector<bool>> &visited,
                                  shared_ptr<DirectionNode> prev);

    // Sort the direction of search, given row and column of two blocks, so that
    // blocks that are closer to destinatino are favored.
    void sortSearchDirections(const int fromR, const int fromC,
                              const int toR, const int toC,
                              QVector<Direction> &directions);

    // Iterate through all posibilities to check if there's still a pair of
    // blocks that can be matched. Returns true and populates <b1> and <b2> with
    // the two blocks if such a pair exists.
    bool hasNextStep(Block *&b1, Block *&b2);

    // A wrapper for `hasNextStep` in case the two conencting blocks are not
    // needed.
    bool hasNextStep();

    // Save current game information to a file.
    void saveToFile();

    // ============================================================
    //
    // Utility functions.
    //
    // ============================================================

    // Get row and column index from x and y coordinates.
    QPair<int, int> getRC(const int x, const int y);
    QPair<int, int> getRC(const QPoint &p);

    // Convert row and col to 1D index. Used in shuffle, where 2D array is
    // flattened.
    int rc2Idx(const QPair<int, int> &p);

    // Returns the y coordinate of the top of a row.
    int getTop(const int r);

    // Returns the y coordinate of the bottom of a row.
    int getBottom(const int r);

    // Returns the x coordinate of the left of a column.
    int getLeft(const int c);

    // Returns the x coordinate of the right of a column.
    int getRight(const int c);

    // Returns true only if the geometric center of the player is with the range
    // of the block at row <r>, column <c>.
    bool isPlayerAt(const int r, const int c);

    // Add <dsec> to the time remaining, and update status bar.
    void changeTime(const int dsec);

public:
    GameWindow(const unique_ptr<UiConfig> &config, QWidget *parent = nullptr);

    // Initialize ui, time and block number, generate map and player,
    // connect signals, set stauts and mode for a new game.
    void prepareNewGame(const GameMode mode);

    // Does the same for a game that is loaded from a save file.
    void prepareSavedGame();

protected:
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;
    virtual void showEvent(QShowEvent *event) override;

private slots:
    // Receives signal when a player <which> has chosen two blocks.
    // Check if <b1>, <b2> can be connected with in <kMaxTurns>, if true, draws
    // the link, add score, eliminate the two blocks, check for solvability,
    // game end, and decides whether to generate a new pair of hints.
    void handleValidateBlock(const WhichPlayer which,
                             Block *const b1,
                             Block *const b2);

    // Receives signal when a match is found, and a player's score is changed. Just
    // change the score label.
    void handleScoreChanged(const WhichPlayer which, const int newScore);

    // Receives signals from countdown timer, which are sent per second, to
    // handle decrementing of time and spawning of items.
    void handleCountDown();

    // Receives sigal every <kKeyPressIntervalMSec>, checks if any movement key
    // is pressed.
    void handleKeyPress();

    // Receives signal when hint item expires, unset the flag and remove current
    // hint visually and logically.
    void handleStopHint();

signals:
    // Emitted when the game ends and user presses any key to return to start
    // window.
    void sendBackToMenu(QWidget *sender);
};
#endif // GAMEWINDOW_H
