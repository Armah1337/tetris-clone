#include "blockinfo.h"
#include "tetrisimageitem.h"
#include <QOpenGLWindow>
#include <QPixmap>
#include <QPainter>
#include <QTimer>
#include <QSize>
#include <QQueue>
#ifndef TETRISSCREEN_H
#define TETRISSCREEN_H
#define PFIELD_ORIGIN_X frame.width()/2 - cellWidth*pfieldWidth/2
#define PFIELD_ORIGIN_Y 40

/* This is the class which is responsible for most of the game's functionality -
 * display, timing, key event handilding, block-related operations, etc.
 */



class TetrisScreen : public QOpenGLWindow{
   Q_OBJECT
   public:
     TetrisScreen();
     enum GameState {Menu, HelpMe, Difficulty, Gameplay, GameOver};
     enum TimeState {Paused, Unpaused};
     enum TetrisImageKey {Logo, Play, Help, Exit, Speed, SpeedLeftArrow, SpeedRightArrow, Start, None,
                          };
     enum AlnumImageKey {Digit_0 = None + 1, Digit_1, Digit_2, Digit_3, Digit_4, Digit_5, Digit_6, Digit_7, Digit_8, Digit_9,
                         A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z};
     enum BlockStyle {BlockStyle1 = Z + 1, BlockStyle2, BlockStyle3, BlockStyle4, BlockStyle5, BlockStyle6, BlockStyle7};
   private slots:
     void beginMenu();
     void beginHelp();
     void beginDifficultySelect();
     void beginGameplay();
     void highlightMenuSelection(TetrisImageKey,TetrisImageKey);
     void highlightDifficultySelection(TetrisImageKey,TetrisImageKey);
     void setGameState(GameState);
     void spawnBlock();
     void handleRedrawRequest();
     void setBlock();
     void timerLeftTimeoutHandler();
     void timerRightTimeoutHandler();
     void startDropDownTimerAtNormal();
     void startDropDownTimerAtAccelerated();
     void dropDown();
     void clearFilledLines(QVector<quint8>);
     void updateBlockQueue();
     void updateStats();
     void drawGameOverLine();
   signals:
     void stateChangeTriggered(GameState newState);
     void exitPressed();
     void menuSelectionChanged(TetrisImageKey,TetrisImageKey);
     void difficultySelectionChanged(TetrisImageKey,TetrisImageKey);
     void blockReachedObstacle();
     void linesFilled(QVector<quint8>);
    protected:
     virtual void paintGL();
     virtual void keyPressEvent(QKeyEvent *event);
     virtual void keyReleaseEvent(QKeyEvent *event);
     virtual void menuKeyPressEvent(QKeyEvent *event);
     virtual void difficultyKeyPressEvent(QKeyEvent *event);
     virtual void gameplayKeyPressEvent(QKeyEvent *event);
     virtual void gameplayKeyReleaseEvent(QKeyEvent *event);
     virtual void setMenuSelection(TetrisImageKey);
     virtual void setDifficultySelection(TetrisImageKey);
     virtual void pressMenuButton();
     virtual void pressDifficultyButton();
     virtual void highlight(TetrisImageKey, TetrisImageKey);
     virtual void drawSpeed();
     virtual void drawPlayfield();
     void drawBlock(QPixmap&);
     virtual void mergeLayersToFrame();
     bool tryToMove(qint8 xOffset, qint8 yOffset);
     bool tryToRotate(BlockInfo::BlockOrientation);
     void setTimeState(TimeState);
     void pause();
     void unpause();
     bool isCellOccupied(const QPoint&);
     void checkForFilledLines();
     void drawLogicalPlayfield();
     void gameOver();
     QPixmap getTetrisPixmapFromMasterImage(quint8 identifier);
     QPixmap getPixmapString(QString src);
   private:
     QHash<TetrisImageKey,TetrisImageItem> imgCont;
     QHash<BlockStyle,QPixmap> blockStyles;
     QHash<AlnumImageKey,QPixmap> characterImgs;
     GameState gameState;
     TimeState timeState;
     TetrisImageKey menuSelection;
     TetrisImageKey difSelection;
     QTimer timerDropDown, timerLeft, timerRight, timerUnpausedRemainingTime, timerUnpausedNormal, timerUnpausedAccelerated, timerGameOverLine;
     qint32 dropDownTimeLeft = 0;
     QSize sz;
     QPixmap frame, images, backgroundLayer, foregroundLayer, activeBlockLayer;
     QPainter painter;
     quint32 speed, lineCount;
     quint64 score, topScore;
     bool isNewRecord = false;
     quint8 timerLeftCnt = 0, timerRightCnt = 0, timerDropDownCnt = 0;
     qint8 gameOverLineY;
     QHash<BlockInfo::BlockType,quint32> blockCount;
     const quint8 cellWidth = 20, cellHeight = 20, pfieldWidth = 10, pfieldHeight = 20;
     quint16 *playField, filled;
     BlockInfo activeBlock;
     QQueue<BlockInfo::BlockType> blockQueue;
     QVector<QPoint> gameOverLine;
};

#endif // TETRISSCREEN_H
