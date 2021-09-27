#include "gamewindow.h"
#include "utils.h"

const QMap<WhichPlayer, QMap<Direction, int>> GameWindow::kKeyMapping = {
    {
        WhichPlayer::kPlayer1, {
            { Direction::kUp, 'W'  },
            { Direction::kDown, 'S'  },
            { Direction::kLeft, 'A'  },
            { Direction::kRight, 'D'  }
        },
    },
    {
        WhichPlayer::kPlayer2, {
            { Direction::kUp, Qt::Key_Up },
            { Direction::kDown, Qt::Key_Down },
            { Direction::kLeft, Qt::Key_Left },
            { Direction::kRight, Qt::Key_Right }
        }
    }
};

GameWindow::GameWindow(const unique_ptr<UiConfig> &config, QWidget *parent):
    QWidget(parent),
    WINDOW_CONFIG(config),
    BLOCK_HEIGHT(kMapHeight / kMaxRows),
    BLOCK_WIDTH(kMapWidth / kMaxCols),
    status(GameStatus::kUnprepared),
    blockMap(kMaxRows, QVector<Block *>(kMaxCols)),
    countDownTimer(new QTimer(this)),
    keyPressTimer(new QTimer(this)),
    hintTimer(new QTimer(this))
{

    // Check map size.
    assert(kMapHeight + kStatusBarHeight == config->windowHeight());
    assert(kMapWidth == config->windowWidth());

    // Check the number of blocks and block types.
    assert(kBlockNum < kMaxRows * kMaxCols - 1);
    assert(kBlocksPerType * kTypeNum == kBlockNum);
    assert(!(kBlocksPerType & 1));

    // Check block and height must be even numbers.
    assert(!(BLOCK_HEIGHT & 1));
    assert(!(BLOCK_WIDTH & 1));

    // Check block size has not been rounded.
    assert(BLOCK_HEIGHT * kMaxRows == kMapHeight);
    assert(BLOCK_WIDTH * kMaxCols == kMapWidth);

    // Check player configuration.
    assert(kKeyMapping.contains(WhichPlayer::kPlayer1) &&
           kKeyMapping.contains(WhichPlayer::kPlayer2));

    // Set fixed window size.
    setFixedSize(config->windowWidth(), config->windowHeight());

    // Center window.
    Utils::centerWindowInScreen(this);

    // Draw layout.
    initLayout();

    countDownTimer->setInterval(1000);
    keyPressTimer->setInterval(kKeyPressIntervalMSec);
    connect(countDownTimer, &QTimer::timeout,
            this, &GameWindow::handleCountDown);
    connect(keyPressTimer, &QTimer::timeout,
            this, &GameWindow::handleKeyPress);
    connect(hintTimer, &QTimer::timeout,
            this, &GameWindow::handleStopHint);

}

void GameWindow::initLayout()
{
    // Contents of status layout depends on game mode.
    QVBoxLayout *outmostLayout = new QVBoxLayout();
    outmostLayout->setAlignment(Qt::AlignTop);
    outmostLayout->setSpacing(0);
    outmostLayout->setContentsMargins(0, 0, 0, 0);

    // Contents of status bar to be set later.
    statusLayout = new QHBoxLayout();
    mapLayout = new QLinkMap();
    mapLayout->setFixedHeight(WINDOW_CONFIG->windowHeight() - kStatusBarHeight);

    // Set layout relations.
    outmostLayout->addLayout(statusLayout);
    outmostLayout->addWidget(mapLayout);

    this->setLayout(outmostLayout);
}

void GameWindow::resetLayout()
{
    // Clear all widgets in status bar and map.
    Utils::removeAllWidgets(statusLayout);
    Utils::removeAllWidgets(mapLayout);
}

void GameWindow::drawStatusBar(const GameMode &mode)
{
    for (int i = 0; i < (mode == GameMode::kSingle ? 1 : 2); ++i) {
        const WhichPlayer p = !i ? WhichPlayer::kPlayer1 : kPlayer2;
        QLabel *scoreLbl = new QLabel();
        scoreLbl->setText(getScoreString(players[p]->score));
        statusLayout->addWidget(scoreLbl);
        this->scoreLbls[p] = scoreLbl;
    }

    timeLbl = new QLabel();
    timeLbl->setText(getTimeString(timeRemaining));
    statusLayout->addWidget(timeLbl);
}

void GameWindow::drawMap()
{
    for (int row = 0; row < kMaxRows; ++row) {
        for (int col = 0; col < kMaxCols; ++col) {
            Block *block = blockMap[row][col];

            // Just to ensure correctness, set the row and col of button.
            block->setPosition(row, col);

            block->setText(QString::number(
                                  blockMap[row][col]->content()));
            block->setGeometry(col * BLOCK_WIDTH,
                               row * BLOCK_HEIGHT,
                               BLOCK_WIDTH,
                               BLOCK_HEIGHT);
        }
    }
}

QUuid GameWindow::drawConnection(Block *const from,
                                 const QList<Direction> &path)
{

    static QMap<Direction, QPair<int, int>> dPos = {
        { Direction::kUp, { 0, -BLOCK_HEIGHT } },
        { Direction::kDown, { 0, BLOCK_HEIGHT } },
        { Direction::kLeft, { -BLOCK_WIDTH, 0 } },
        { Direction::kRight, { BLOCK_WIDTH, 0 } }
    };
    QList<QLine> lines;
    QUuid uuid;
    auto &fromG = from->geometry();
    int startX = fromG.center().x();
    int startY = fromG.center().y();
    int endX = startX;
    int endY = startY;
    int dx;
    int dy;
    bool isFirstLine;
    bool isLastLine;

    for (auto it = path.begin(); it != path.end();) {
        Direction d = *it;
        dx = dPos[d].first;
        dy = dPos[d].second;

        while (*it == d && it != path.end()) {
            endX += dx;
            endY += dy;
            ++it;
        }

        isFirstLine = lines.empty();
        isLastLine = it == path.end();
//        lines.emplaceBack(startX + isFirstLine * (dx >> 1),
//                          startY + isFirstLine * (dy >> 1),
//                          endX - isLastLine * (dx >> 1),
//                          endY - isLastLine * (dy >> 1));
        lines.emplaceBack(startX,
                          startY,
                          endX,
                          endY);

        startX = endX;
        startY = endY;
    }

    uuid = mapLayout->addLines(lines);
    mapLayout->update();
    return uuid;
}

void GameWindow::clearConnection(const QUuid &uuid)
{
    mapLayout->removeLines(uuid);
    mapLayout->update();
}

void GameWindow::promptStart()
{
    QMessageBox::information(this, "QLink", "Press any key to start");
}

void GameWindow::promptPause()
{
    QMessageBox::information(this, "QLink", "Game paused");
}

void GameWindow::promtResumeFromLoad()
{
    QMessageBox::information(this, "QLink", "Press any key to resume");
}

void GameWindow::promptSuccess()
{
    QMessageBox::information(this, "QLink", "You have succeeded");
}

void GameWindow::promptStuck()
{
    QMessageBox::information(this, "QLink", "No more match");
}

void GameWindow::promptTimesUp()
{
    QMessageBox::information(this, "QLink", "Times up!");
}

void GameWindow::promptOnePlayerWins() {
    assert(this->mode == GameMode::kDouble);
    int s1 = players[WhichPlayer::kPlayer1]->score;
    int s2 = players[WhichPlayer::kPlayer2]->score;

    if (s1 > s2) {
        QMessageBox::information(this, "QLink", "Player 1 wins!");
    } else if (s1 == s2) {
        QMessageBox::information(this, "QLink", "Tie!");
    } else {
        QMessageBox::information(this, "QLink", "Player 2 wins!");
    }
}

QString GameWindow::getScoreString(const int score)
{
    return "Score: " + QString::number(score);
}

QString GameWindow::getTimeString(const int sec)
{
    return "Time: " + QString::number(sec);
}

void GameWindow::generateMap()
{
    // Generate a random sequence of one-dimensional indices.
    QVector<int> pos(kMaxRows * kMaxCols);
    std::iota(pos.begin(), pos.end(), 0);
    Utils::shuffle(pos);

    // Assign indices to ordered block.
    for (int i = 0; i < kMaxRows * kMaxCols; ++i) {
        // Set block status.
        int idx = pos[i];
        int row = idx / kMaxCols;
        int col = idx % kMaxCols;
        BlockType blockType = i < kBlockNum ?
                    BlockType::kBlock :
                    BlockType::kEmpty;
        BlockContent blockContent = i < kBlockNum ?
                    i / kBlocksPerType + 1 :
                    Block::kEmptyBlock;
        blockMap[row][col] = new Block(row, col, false, blockType, blockContent,
                                       WhichPlayer::kNoPlayer, mapLayout);
    }
}

bool GameWindow::generatePlayer(const WhichPlayer &which)
{
    for (int i = 0; i < kMaxGeneratePlayerTrials; ++i) {
        players.remove(which);

        int row = Utils::randomInt(0, kMaxRows);
        int col = Utils::randomInt(0, kMaxCols);

        // Check if current block is used.
        if (!blockMap[row][col]->isEmpty()) {
            continue;
        }

        // Check if current blocked is surrounded.
        bool upBlocked = !row ||
                !blockMap[row - 1][col]->isEmpty();
        bool downBlocked = (row == kMaxRows - 1) ||
                !blockMap[row + 1][col]->isEmpty();
        bool leftBlocked = !col ||
                !blockMap[row][col - 1]->isEmpty();
        bool rightBlocked = (col == kMaxCols - 1) ||
                !blockMap[row][col + 1]->isEmpty();

        // Retry on failure. Else generate character in the middle of the block.
        // Never generate player beside the edge.
        if ((upBlocked && downBlocked && leftBlocked && rightBlocked) ||
            !row || row == kMaxRows - 1 || !col || col == kMaxCols) {
            continue;
        } else {
            int x = col * BLOCK_WIDTH + (BLOCK_WIDTH >> 1);
            int y = row * BLOCK_HEIGHT + (BLOCK_HEIGHT >> 1);

            Player *newPlayer = new Player(which, x, y, 0, mapLayout);
            players.insert(which, newPlayer);

            return true;
        }
    }
    return false;
}

void GameWindow::connectPlayerSignals(const GameMode mode)
{
    for (int i = 0; i < (mode == GameMode::kSingle ? 1 : 2); ++i) {
        WhichPlayer which = !i ? WhichPlayer::kPlayer1 : WhichPlayer::kPlayer2;
        assert(players.contains(which) && players[which] != nullptr);
        Player *player = players[which];
        connect(player, &Player::sendValidateBlock,
                this, &GameWindow::handleValidateBlock);
        connect(player, &Player::sendScoreChanged,
                this, &GameWindow::handleScoreChanged);
    }
}

void GameWindow::startGame()
{
    assert(status == GameStatus::kPreparedLoad ||
           status == GameStatus::kPreparedNew ||
           status == GameStatus::kPaused);
    this->status = GameStatus::kPlaying;
    countDownTimer->start();
    keyPressTimer->start();
}

void GameWindow::pauseGame()
{
    assert(status == GameStatus::kPlaying);

    this->status = GameStatus::kPaused;
    countDownTimer->stop();
    keyPressTimer->stop();

    if (this->hint) {
        this->hintTimeRemaining = hintTimer->remainingTime();
        qDebug() << this->hintTimeRemaining;
        hintTimer->stop();
    }
}

void GameWindow::resumeGame()
{
    assert(status == GameStatus::kPaused ||
           status == GameStatus::kPreparedLoad);
    this->status = GameStatus::kPlaying;
    countDownTimer->start();
    keyPressTimer->start();

    if (this->hint) {
        hintTimer->start(hintTimeRemaining);
    }
}

void GameWindow::stopGame()
{
    this->status = GameStatus::kStopped;
    countDownTimer->stop();
    keyPressTimer->stop();
    hintTimer->stop();
}

void GameWindow::spawnItem()
{
    const int rand = Utils::randomInt(0, 3);
    const ItemType t = (rand == 0) ? ItemType::kExtend30s :
                            (rand == 1) ? ItemType::kShuffle :
                                ItemType::kHint;
    int row;
    int col;

    while (true) {
        row = Utils::randomInt(0, kMaxRows);
        col = Utils::randomInt(0, kMaxCols);
        if (!blockMap[row][col]->isEmpty() || isPlayerAt(row, col)) {
            continue;
        }
        blockMap[row][col]->spawnItem(t);
        break;
    }
}

void GameWindow::consumeItem(const WhichPlayer which, Block *const block) {
    assert(block->type() == BlockType::kItem);
    switch (block->content()) {
    case ItemType::kExtend30s:
        changeTime(30);
        break;
    case ItemType::kShuffle:
        shuffle();
        break;
    case ItemType::kHint:
        enableHint(which, kHintTimeMsec);
        break;
    default:
        break;
    }

    block->consumeItem();
}

void GameWindow::shuffle()
{
    // Flatten 2D array to 1D array.
    QVector<Block *> flattendMap;
    QSet<int> playerBlocks;

    for (const auto& it: blockMap) {
        flattendMap.append(it);
    }

    // Shuffle 1D array. Has to shuffle twice. Otherwise the vector will become
    // what it's like before shuffling.
    Utils::shuffle(flattendMap);
    Utils::shuffle(flattendMap);

    // Swap the empty block with player's block, if it's not empty.
    // Note that the axes of the blocks have not been changed.
    // Need to acquire the block with row and col converted to 1D index.
    // Need to use a set to avoid swapping a block twice.
    const int maxIter = (mode == GameMode::kSingle) ? 1 : 2;
    for (int i = 0; i < maxIter; ++i) {
        const WhichPlayer which = !i ? WhichPlayer::kPlayer1 :
                                       WhichPlayer::kPlayer2;
        const auto &g = players[which]->geometry();
        playerBlocks.insert(rc2Idx(getRC(g.topLeft())));
        playerBlocks.insert(rc2Idx(getRC(g.topRight())));
        playerBlocks.insert(rc2Idx(getRC(g.bottomLeft())));
        playerBlocks.insert(rc2Idx(getRC(g.bottomRight())));
    }

    const int len = flattendMap.size();
    for (const int idx: playerBlocks) {
        Block *b = flattendMap[idx];
        if (!b->isEmpty()) {
            for (int i = 0; i < len; ++i) {
                if (flattendMap[i]->isEmpty()) {
                    flattendMap.swapItemsAt(idx, i);
                }
            }
        }
    }

    // Convert flattend 1D array back to 2D array.
    blockMap.clear();
    for (int i = 0; i < kMaxRows; ++i) {
        blockMap.push_back(QVector<Block *>(flattendMap.begin() + i * kMaxCols,
                            flattendMap.begin() + (i + 1) * kMaxCols));
    }

    drawMap();

    // Regenerate hint.
    if (this->hint) {
        generateHint();
    }
}

void GameWindow::enableHint(const WhichPlayer which, const int t)
{
    hintTimer->stop();
    hintTimer->start(t);

    this->hint = true;
    this->hintFor = which;
    generateHint();
}

void GameWindow::generateHint()
{
    Block *b1;
    Block *b2;

    // Remove current hint.
    removeCurrentHint();

    // Generate new hint.
    if (!hasNextStep(b1, b2)) {
        stopGame();
        promptStuck();
        return;
    }
    hintPair.first = b1;
    hintPair.second = b2;
    b1->markAsHint();
    b2->markAsHint();
}

void GameWindow::removeCurrentHint()
{
    if (hintPair.first != nullptr) {
        hintPair.first->unmarkAsHint();
    }
    if (hintPair.second != nullptr) {
        hintPair.second->unmarkAsHint();
    }
    hintPair.first = hintPair.second = nullptr;
}

void GameWindow::movePlayer(const WhichPlayer which,
                            const Direction &d)
{
    const int verticalRange = Player::SHAPE_SIZE >> 1;
    const int horizontalRange = Player::SHAPE_SIZE >> 1;

    Player *player = players[which];
    assert(player);

    int newX;
    int newY;
    bool collide1;
    bool collide2;
    QPair<int, int> rc;
    auto &g = player->geometry();
    Block *bBuffer1 = nullptr;
    Block *bBuffer2 = nullptr;
    Block *iBuffer1 = nullptr;
    Block *iBuffer2 = nullptr;

    // To make sure the character does not clip through blocks, need to check
    // two points for each direction. For example, if the character is moving
    // up, then top left and top right corner needs to be checked.
    switch (d) {
    case Direction::kUp:
        newY = std::max(player->y - kMoveStep, verticalRange);
        rc = getRC(player->x, newY - verticalRange);
        collide1 = checkCollision(g.left(), newY - verticalRange, bBuffer1);
        collide2 = checkCollision(g.right(), newY - verticalRange, bBuffer2);
        checkItem(g.left(), newY - verticalRange, iBuffer1);
        checkItem(g.right(), newY - verticalRange, iBuffer2);
        if (collide1 || collide2) {
            player->y = getBottom(rc.first) + verticalRange;
        } else {
            player->y = newY;
        }
        break;
    case Direction::kDown:
        newY = std::min(player->y + kMoveStep, kMapHeight - verticalRange);
        rc = getRC(player->x, newY + verticalRange);
        collide1 = checkCollision(g.left(), newY + verticalRange, bBuffer1);
        collide2 = checkCollision(g.right(), newY + verticalRange, bBuffer2);
        checkItem(g.left(), newY + verticalRange, iBuffer1);
        checkItem(g.right(), newY + verticalRange, iBuffer2);
        if (collide1 || collide2) {
            player->y = getTop(rc.first) - verticalRange;
        } else {
            player->y = newY;
        }
        break;
    case Direction::kLeft:
        newX = std::max(player->x - kMoveStep, horizontalRange);
        rc = getRC(newX - horizontalRange, player->y);
        collide1 = checkCollision(newX - horizontalRange, g.top(), bBuffer1);
        collide2 = checkCollision(newX - horizontalRange, g.bottom(), bBuffer2);
        checkItem(newX - horizontalRange, g.top(), iBuffer1);
        checkItem(newX - horizontalRange, g.bottom(), iBuffer2);
        if (collide1 || collide2) {
            player->x = getRight(rc.second) + horizontalRange;
        } else {
            player->x = newX;
        }
        break;
    case Direction::kRight:
        newX = std::min(player->x + kMoveStep, kMapWidth - horizontalRange);
        rc = getRC(newX + horizontalRange, player->y);
        collide1 = checkCollision(newX + horizontalRange, g.top(), bBuffer1);
        collide2 = checkCollision(newX + horizontalRange, g.bottom(), bBuffer2);
        checkItem(newX + horizontalRange, g.top(), iBuffer1);
        checkItem(newX + horizontalRange, g.bottom(), iBuffer2);
        if (collide1 || collide2) {
            player->x = getLeft(rc.second) - horizontalRange;
        } else {
            player->x = newX;
        }
        break;
    default:
        break;
    }

    if (bBuffer1 && bBuffer2 && bBuffer1 != bBuffer2) {
        auto whichBlock = distinguishCollision(player, bBuffer1, bBuffer2, d);
        if (whichBlock != nullptr) {
            player->chooseBlock(whichBlock);
        }
    } else if (bBuffer1) {
        player->chooseBlock(bBuffer1);
    } else if (bBuffer2) {
        player->chooseBlock(bBuffer2);
    }

    if (iBuffer1) {
        consumeItem(which, iBuffer1);
    }
    if (iBuffer2) {
        consumeItem(which, iBuffer2);
    }

    player->update();
}

Block * GameWindow::distinguishCollision(
        Player *player,
        Block *const block1,
        Block *const block2,
        const Direction d)
{
    bool chooseX = (d == Direction::kUp || d == Direction::kDown);
    int playerPos = chooseX ? player->x : player->y;
    int block1Pos = chooseX ? block1->geometry().center().x() :
                              block1->geometry().center().y();
    int block2Pos = chooseX ? block2->geometry().center().x() :
                              block2->geometry().center().y();

    int distToBlock1 = std::abs(playerPos - block1Pos);
    int distToBlock2 = std::abs(playerPos - block2Pos);

    return distToBlock1 < distToBlock2 ? block1 :
                distToBlock1 > distToBlock2 ? block2 :
                    nullptr;
}

bool GameWindow::checkCollision(const int x, const int y,
                                Block *&buffer)
{
    // Set the buffer to null, just in case it is not initially null.
    buffer = nullptr;

    if (x >= kMapWidth || y >= kMapHeight) {
        return true;
    }

    const auto rc = getRC(x, y);
    const auto block = blockMap[rc.first][rc.second];
    if (block->isBlock()) {
        buffer = blockMap[rc.first][rc.second];
        return true;
    }
    return false;
}

bool GameWindow::checkItem(const int x, const int y, Block *&buffer)
{
    buffer = nullptr;

    if (x >= kMapWidth || y >= kMapHeight) {
        return true;
    }

    const auto rc = getRC(x, y);
    const auto block = blockMap[rc.first][rc.second];
    if (block->isInItemRange(x, y)) {
        buffer = blockMap[rc.first][rc.second];
        return true;
    }
    return false;
}

bool GameWindow::checkMatch(Block *const b1, Block *const b2,
                            QList<Direction> *path)
{
    return b1 != b2 &&
           b1->type() == BlockType::kBlock &&
           b2->type() == BlockType::kBlock &&
           b1->content() == b2->content() &&
           b1->chosenBy() == b2->chosenBy() &&
           checkConnectivity(b1, b2, path);
}

bool GameWindow::checkConnectivity(Block *const from,
                                   Block *const to,
                                   QList<Direction> *path)
{
    QVector<QVector<bool>> visited(kMaxRows, QVector<bool>(kMaxCols, false));
    visited[from->row()][from->col()] = true;

    // Recreate the path with DirectionNode, like a linked list.
    auto lastDNode = dfs(0, from, to, visited, nullptr);
    bool found = lastDNode != nullptr;

    if (found && path != nullptr) {
        path->clear();
        while (lastDNode) {
            path->push_front(lastDNode->d);
            lastDNode = lastDNode->prev;
        }
    }

    return found;
}

shared_ptr<DirectionNode> GameWindow::dfs(const int turns,
                                          Block *const current,
                                          Block *const to,
                                          QVector<QVector<bool>> &visited,
                                          shared_ptr<DirectionNode> dNode)
{
    static const QMap<Direction, QPair<int, int>> move = {
        { Direction::kUp, { -1, 0 } },
        { Direction::kDown, { 1, 0 } },
        { Direction::kLeft, { 0, -1 } },
        { Direction::kRight, { 0, 1 } }
    };

    QVector<Direction> searchD;
    sortSearchDirections(current->row(), current->col(),
                         to->row(), to->col(),
                         searchD);
    for (auto it = searchD.begin(); it != searchD.end(); ++it) {
        const Direction d = *it;
        const int dr = move[d].first;
        const int dc = move[d].second;
        const int newR = current->row() + dr;
        const int newC = current->col() + dc;
        int newTurns = (dNode == nullptr) ? 0 : d == dNode->d ?
                                turns : turns + 1;

        // Check index out of bound or too many turns or circular visit.
        if (newR < 0 || newR >= kMaxRows || newC < 0 || newC >= kMaxCols ||
            newTurns > kMaxTurns ||
            visited[newR][newC]) {
            continue;
        }

        auto newBlockPtr = blockMap[newR][newC];

        // Check correct answer.
        if (newBlockPtr == to) {
            return make_shared<DirectionNode>(d, dNode);
        }

        // Check block occupied.
        if (newBlockPtr->isBlock()) {
            continue;
        }

        visited[newR][newC] = true;

        shared_ptr<DirectionNode> ret = dfs(newTurns,
                                            blockMap[newR][newC],
                                            to,
                                            visited,
                                            make_shared<DirectionNode>(d, dNode)
                                            );
        if (ret) {
            return ret;
        }

        visited[newR][newC] = false;
    }

    return nullptr;
}

void GameWindow::sortSearchDirections(const int fromR, const int fromC,
                                      const int toR, const int toC,
                                      QVector<Direction> &directions) {
    assert(fromR != toR || fromC != toC);
    directions.clear();
    // Up.
    if (toR <  fromR) {
        // Up left and straight up.
        if (toC <= fromC) {
            directions.append({ Direction::kUp, Direction::kLeft,
                                Direction::kRight, Direction::kDown});
        }
        // Up right.
        else {
            directions.append({ Direction::kRight, Direction::kUp,
                                Direction::kLeft, Direction::kDown});
        }
    }
    // At the same row.
    else if (toR == fromR) {
        // Straight left.
        if (toC < fromC) {
            directions.append({ Direction::kLeft, Direction::kUp,
                                Direction::kDown, Direction::kRight});
        }
        // Straight right.
        else {
            directions.append({ Direction::kRight, Direction::kUp,
                                Direction::kDown, Direction::kLeft});
        }
    }
    // Down.
    else {
        // Down left and straight down.
        if (toC <= fromC) {
            directions.append({ Direction::kDown, Direction::kLeft,
                                Direction::kRight, Direction::kUp});
        }
        // Down right.
        else {
            directions.append({ Direction::kDown, Direction::kRight,
                                Direction::kLeft, Direction::kUp});
        }
    }
}


bool GameWindow::hasNextStep(Block *&b1, Block *&b2)
{
    int maxIter = (mode == GameMode::kSingle) ? 1 : 2;
    auto findAndCheck = [&](Block *const playerBlock,
            const int playerR, const int playerC,
            const int fromR, const int fromC) -> bool {

        Block *fromBlock = blockMap[fromR][fromC];
        BlockContent fromContent = fromBlock->content();
        // Check if from block has content and can be reached by player.
        if (!fromBlock->isBlock() ||
            !checkConnectivity(playerBlock, fromBlock, nullptr) ||
            (fromR == playerR && fromC == playerC)) {
            return false;
        }

        for (int r = playerR; r >= 0; --r) {
            for (int c = playerC; c >= 0; --c) {
                if ((r == fromR && c == fromC) ||
                    (playerR == r && playerC == c)) {
                    continue;
                }
                Block *toBlock = blockMap[r][c];
                if (toBlock->isBlock() && toBlock->content() == fromContent &&
                    checkConnectivity(fromBlock, toBlock, nullptr) &&
                    checkConnectivity(playerBlock, toBlock, nullptr)) {
                    b1 = fromBlock;
                    b2 = toBlock;
                    return true;
                }
            }

            for (int c = playerC + 1; c < kMaxCols ; ++c) {
                if ((r == fromR && c == fromC) ||
                    (playerR == r && playerC == c)) {
                    continue;
                }
                Block *toBlock = blockMap[r][c];
                if (toBlock->isBlock() && toBlock->content() == fromContent &&
                    checkConnectivity(fromBlock, toBlock, nullptr) &&
                    checkConnectivity(playerBlock, toBlock, nullptr)) {
                    b1 = fromBlock;
                    b2 = toBlock;
                    return true;
                }
            }
        }

        for (int r = playerR + 1; r < kMaxRows; ++r) {
            for (int c = playerC; c >= 0; --c) {
                if ((r == fromR && c == fromC) ||
                    (playerR == r && playerC == c)) {
                    continue;
                }
                Block *toBlock = blockMap[r][c];
                if (toBlock->isBlock() && toBlock->content() == fromContent &&
                    checkConnectivity(fromBlock, toBlock, nullptr) &&
                    checkConnectivity(playerBlock, toBlock, nullptr)) {
                    b1 = fromBlock;
                    b2 = toBlock;
                    return true;
                }
            }

            for (int c = playerC + 1; c < kMaxCols ; ++c) {
                if ((r == fromR && c == fromC) ||
                    (playerR == r && playerC == c)) {
                    continue;
                }
                Block *toBlock = blockMap[r][c];
                if (toBlock->isBlock() && toBlock->content() == fromContent &&
                    checkConnectivity(fromBlock, toBlock, nullptr) &&
                    checkConnectivity(playerBlock, toBlock, nullptr)) {
                    b1 = fromBlock;
                    b2 = toBlock;
                    return true;
                }
            }
        }

        return false;
    };

    for (int i = 0; i < maxIter; ++i) {
        const WhichPlayer which =
                (!i && hintFor != WhichPlayer::kPlayer2) ||
                (i && hintFor == WhichPlayer::kPlayer2) ?
                    WhichPlayer::kPlayer1 :
                    WhichPlayer::kPlayer2;
        Player *player = players[which];
        const auto &rc = getRC(player->geometry().center());
        const int playerR = rc.first;
        const int playerC = rc.second;
        Block *playerBlock = blockMap[playerR][playerC];
        for (int r = playerR; r >= 0; --r) {
            for (int c = playerC; c >= 0; --c) {
                if (findAndCheck(playerBlock, playerR, playerC, r, c)) {
                     return true;
                }
            }

            for (int c = playerC + 1; c < kMaxCols ; ++c) {
                if (findAndCheck(playerBlock, playerR, playerC, r, c)) {
                     return true;
                }
            }
        }

        for (int r = playerR + 1; r < kMaxRows; ++r) {
            for (int c = playerC; c >= 0; --c) {
                if (findAndCheck(playerBlock, playerR, playerC, r, c)) {
                     return true;
                }
            }

            for (int c = playerC + 1; c < kMaxCols ; ++c) {
                if (findAndCheck(playerBlock,
                                 playerR, playerC, r, c)) {
                     return true;
                }
            }
        }
    }

    return false;
}

bool GameWindow::hasNextStep()
{
    Block *b1;
    Block *b2;
    bool f = hasNextStep(b1, b2);
    return f;
}

void GameWindow::saveToFile()
{
    QFile file("save.txt");
    file.open(QIODevice::WriteOnly);
    QTextStream s(&file);

    // Save mode.
    s << mode << '\n';

    // Save blockMap.
    for (int r = 0; r < kMaxRows; ++r) {
        for (int c = 0; c < kMaxCols; ++c) {
            s << *blockMap[r][c] << ' ';
        }
        s << '\n';
    }

    // Save players
    s << *players[WhichPlayer::kPlayer1] << '\n';
    if (mode == GameMode::kDouble) {
        s << *players[WhichPlayer::kPlayer2] << '\n';
    }

    // Save blocks remaining.
    s << blocksRemaining << '\n';

    // Save time remaining.
    s << timeRemaining << '\n';

    // Save hint status.
    s << hint << '\n';

    // Save hint for.
    s << hintFor << '\n';

    // Save remaining hint time.
    s << hintTimeRemaining << '\n';

    // Save hint pair.
    if (hintPair.first) {
        Block *b = hintPair.first;
        s << b->row() << ' ' << b->col() << '\n';
    }
    if (hintPair.second) {
        Block *b = hintPair.second;
        s << b->row() << ' ' << b->col() << '\n';
    }

    file.close();
}


QPair<int, int> GameWindow::getRC(const int x, const int y)
{
    assert(x >= 0 && x <= mapLayout->geometry().width() &&
           y >= 0 && y <= mapLayout->geometry().height());
    return qMakePair(y / BLOCK_HEIGHT, x / BLOCK_WIDTH);
}

QPair<int, int> GameWindow::getRC(const QPoint &p)
{
    return getRC(p.x(), p.y());
}

int GameWindow::rc2Idx(const QPair<int, int> &p)
{
    return p.first * kMaxCols + p.second;
}

int GameWindow::getTop(const int r)
{
    return r * BLOCK_HEIGHT;
}

int GameWindow::getBottom(const int r)
{
    return r * BLOCK_HEIGHT + BLOCK_HEIGHT;
}

int GameWindow::getLeft(const int c)
{
    return c * BLOCK_WIDTH;
}

int GameWindow::getRight(const int c)
{
    return c * BLOCK_WIDTH + BLOCK_WIDTH;
}

bool GameWindow::isPlayerAt(const int r, const int c) {
    const int maxIter = (mode == GameMode::kSingle) ? 1 : 2;
    for (int i = 0; i < maxIter; ++i) {
        const WhichPlayer which = !i ? WhichPlayer::kPlayer1 :
                                       WhichPlayer::kPlayer2;
        const auto &rc = getRC(players[which]->x, players[which]->y);
        if (rc.first == r && rc.second == c) {
            return true;
        }
    }
    return false;
}

void GameWindow::changeTime(const int dsec)
{
    assert(this->timeLbl);
    this->timeRemaining += dsec;
    this->timeLbl->setText(getTimeString(this->timeRemaining));

    if (!timeRemaining) {
        stopGame();
        if (this->mode == GameMode::kSingle) {
            promptTimesUp();
        } else {
            promptOnePlayerWins();
        }
    }
}

void GameWindow::prepareNewGame(const GameMode mode)
{
    resetLayout();

    this->mode = mode;
    // Generate map and player position.
    // Players are drawn automatically.
    while (true) {
        generateMap();
        if (generatePlayer(WhichPlayer::kPlayer1) &&
            (mode == GameMode::kSingle || generatePlayer(WhichPlayer::kPlayer2))
                ) {
            break;
        }
    }

    // Reset time.
    this->timeRemaining = kInitialTime;
    this->blocksRemaining = kBlockNum;

    // Draw status bar.
    drawStatusBar(mode);

    // Draw map.
    drawMap();

    // Connect player logic with Ui.
    connectPlayerSignals(mode);

    status = GameStatus::kPreparedNew;
}

void GameWindow::prepareSavedGame()
{
    QFile file("save.txt");
    file.open(QIODevice::ReadOnly);
    QTextStream s(&file);
    int x;

    // Load mode.
    s >> x;
    this->mode = static_cast<GameMode>(x);

    // Load map.
    for (int r = 0; r < kMaxRows; ++r) {
        for (int c = 0; c < kMaxCols; ++c) {
            Block *b = Block::fromTextStream(s);
            b->setParent(mapLayout);
            blockMap[r][c] = b;
        }
    }

    // Load player.
    players[WhichPlayer::kPlayer1] = Player::fromTextStream(s);
    players[WhichPlayer::kPlayer1]->setParent(mapLayout);
    if (mode == GameMode::kDouble) {
         players[WhichPlayer::kPlayer2] = Player::fromTextStream(s);
         players[WhichPlayer::kPlayer2]->setParent(mapLayout);
    }

    // Load time.
    s >> this->blocksRemaining;
    s >> this->timeRemaining;
    s >> x;
    this->hint = static_cast<bool>(x);
    s >> x;
    this->hintFor = static_cast<WhichPlayer>(x);
    s >> this->hintTimeRemaining;

    // Load hint pair.
    int r, c;
    if (!s.atEnd()) {
        s >> r >> c;
        hintPair.first = blockMap[r][c];
        s >> r >> c;
        hintPair.second = blockMap[r][c];
    }

    // Draw status bar.
    drawStatusBar(mode);

    // Draw map.
    drawMap();

    file.close();

    connectPlayerSignals(mode);

    status = GameStatus::kPreparedLoad;
}

// ============================================================
//
// Events
//
// ============================================================

void GameWindow::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();

    switch (this->status) {
    case GameStatus::kPlaying: {
        this->pressedKeys.insert(key);
        if (key == kPauseKey) {
            promptPause();
            pauseGame();
        }
        break;
    }
    case GameStatus::kStopped:
        emit sendBackToMenu(this);
        break;
    case GameStatus::kUnprepared:
        break;
    case GameStatus::kPreparedNew:
        startGame();
        break;
    case GameStatus::kPreparedLoad:
        resumeGame();
        break;
    case GameStatus::kPaused:
        if (key == kPauseKey) {
            resumeGame();
        } else if (key == kSaveKey) {
            saveToFile();
        }
        break;
    }
}

void GameWindow::keyReleaseEvent(QKeyEvent *event) {
    int key = event->key();
    this->pressedKeys.remove(key);
}

void GameWindow::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    switch (this->status) {
    case GameStatus::kPreparedNew:
        promptStart();
        break;
    case GameStatus::kPreparedLoad:
        promtResumeFromLoad();
        break;
    default:
        break;
    }
}

// ============================================================
//
// Slots
//
// ============================================================
void GameWindow::handleValidateBlock(const WhichPlayer which,
                                     Block *const b1,
                                     Block *const b2)
{
    Player *player = players[which];
    QList<Direction> path;
    QUuid uuid;
    assert(player);

    if (b1 != b2 && b1->content() == b2->content() &&
        checkConnectivity(b1, b2, &path)) {
        // Draw connection for 1 sec.
        uuid = drawConnection(b1, path);
        QTimer::singleShot(kShowConnectionDurationMsec,
                           this, [=](){
            clearConnection(uuid);
        });

        player->addScore(kScorePerMatch);

        b1->eliminateSelf();
        b2->eliminateSelf();
        this->blocksRemaining -= 2;
        assert(!(this->blocksRemaining & 1) && blocksRemaining >= 0);

        // Check for game end.
        if (!blocksRemaining) {
            stopGame();
            if (mode == GameMode::kSingle) {
                promptSuccess();
            } else {
                promptOnePlayerWins();
            }
        } else if (!hasNextStep()) {
            stopGame();
            promptStuck();
        }

        // Check for hint.
        if (hint && ((b1 == hintPair.first || b1 == hintPair.second) ||
                (b2 == hintPair.first && b2 == hintPair.second))) {
            generateHint();
        }

    } else {
        b1->setChosenBy(WhichPlayer::kNoPlayer);
        b2->setChosenBy(WhichPlayer::kNoPlayer);
    }
    player->removeChosenBlock();
}

void GameWindow::handleScoreChanged(const WhichPlayer which,
                                    const int newScore) {
    assert(scoreLbls.contains(which));
    scoreLbls[which]->setText(getScoreString(newScore));
}

void GameWindow::handleCountDown()
{
    // Reduce time remaining.
    changeTime(-1);

    // Possibly generate new item.
    if (Utils::randomInt(0, 101) <= kSpawnItemProbability) {
        spawnItem();
    }
}

void GameWindow::handleKeyPress() {
    if (this->status != GameStatus::kPlaying) {
        return;
    }

    int maxIter = (mode == GameMode::kSingle) ? 1 : 2;
    for (int i = 0; i < maxIter; ++i) {
        const WhichPlayer which = !i ?
                    WhichPlayer::kPlayer1 : WhichPlayer::kPlayer2;
        const auto &keyMapping = this->kKeyMapping[which];
        const int upKey = keyMapping[Direction::kUp];
        const int downKey = keyMapping[Direction::kDown];
        const int leftKey = keyMapping[Direction::kLeft];
        const int rightKey = keyMapping[Direction::kRight];

        // If keys of opposite directions are pressed, do nothing.
        if ((pressedKeys.contains(downKey) &&
            pressedKeys.contains(upKey)) ||
            (pressedKeys.contains(leftKey) &&
            pressedKeys.contains(rightKey))) {
            continue;
        }

        if (pressedKeys.contains(upKey)) {
            movePlayer(which, Direction::kUp);
        }
        if (pressedKeys.contains(downKey)) {
            movePlayer(which, Direction::kDown);
        }
        if (pressedKeys.contains(leftKey)) {
            movePlayer(which, Direction::kLeft);
        }
        if (pressedKeys.contains(rightKey)) {
            movePlayer(which, Direction::kRight);
        }
    }
}

void GameWindow::handleStopHint()
{
    this->hint = false;
    this->hintFor = WhichPlayer::kNoPlayer;

    // If game stopps, just leave the hint highlight be.aaa
    if (this->status != GameStatus::kStopped) {
        removeCurrentHint();
    }
}
