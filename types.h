#ifndef TYPES_H
#define TYPES_H

typedef enum {
    kSingle, kDouble
} GameMode;

typedef enum {
    kNoPlayer, kPlayer1, kPlayer2
} WhichPlayer;

typedef enum {
    kUp, kDown, kLeft, kRight
} Direction;

typedef enum {
    kUnprepared, kPreparedNew, kPreparedLoad, kPlaying, kPaused, kStopped
} GameStatus;

typedef enum {
    kEmpty, kBlock, kItem
} BlockType;

typedef enum {
    kExtend30s, kShuffle, kHint
} ItemType;

typedef int BlockContent;
#endif // TYPES_H
