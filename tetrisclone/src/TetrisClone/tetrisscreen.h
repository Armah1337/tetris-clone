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

/*
 * Things to add:
 * Improved controls maybe
 * Visuals CHECK
 * Queue-based spawning CHECK
 * Pausing
 * Settings?
 * Stats CHECK
*/

struct TetrisImageItem;

class BlockInfo : public QObject{
    Q_OBJECT
public:
    enum BlockType {I = 0, J = 1, L = 2, O = 3, S = 4, T = 5, Z = 6};
    enum BlockOrientation {Up, Right, Down, Left};
signals:
    void redrawRequested();
private:
    BlockType b_type;
    BlockOrientation b_orient;
    QVector<QPoint> blocks;
public:
    BlockInfo() = default;
    BlockType blockType(){return b_type;}
    void setNewBlock(BlockType);
    BlockOrientation blockOrientation(){return b_orient;}
    const QVector<QPoint>& getBlocks(){return blocks;}
    void moveBlock(qint8 xOffset, qint8 yOffset);
    void rotateBlock(BlockOrientation orient, const QVector<QPoint>&);
    QVector<QPoint> getRotatedBlocks(BlockOrientation);
};

class TetrisScreen : public QOpenGLWindow{
   Q_OBJECT
   public:
     TetrisScreen();
     enum GameState {Menu, Difficulty, Gameplay};
     enum TetrisImageKey {Logo, Play, Help, Exit, Speed, SpeedLeftArrow, SpeedRightArrow, Start, None,
                          };
     enum AlnumImageKey {Digit_0 = None + 1, Digit_1, Digit_2, Digit_3, Digit_4, Digit_5, Digit_6, Digit_7, Digit_8, Digit_9,
                         A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z};
     enum BlockStyle {BlockStyle1 = Z + 1, BlockStyle2, BlockStyle3, BlockStyle4, BlockStyle5, BlockStyle6, BlockStyle7};
   private slots:
     void beginMenu();
     void beginDifficultySelect();
     void beginGameplay();
     void highlightMenuSelection(TetrisImageKey,TetrisImageKey);
     void highlightDifficultySelection(TetrisImageKey,TetrisImageKey);
     void setGameState(GameState);
     void spawnBlock();
     void handleRedrawRequest();
     void setBlock();
     void dropDown();
     void clearFilledLines(QVector<quint8>);
     void updateBlockQueue();
     void updateStats();
   signals:
     void playPressed(GameState newState);
     void helpPressed();
     void exitPressed();
     void menuSelectionChanged(TetrisImageKey,TetrisImageKey);
     void difficultySelectionChanged(TetrisImageKey,TetrisImageKey);
     void startPressed(GameState newState);
     void blockReachedObstacle();
     void linesFilled(QVector<quint8>);
     void gameOver(GameState newState);
     // void speedLeftArrowPressed(GameState newState);
     // void speedRightArrowPressed(GameState newState);
     // maybe add a gameStateChanged signal
     // signal for changing difficulty
     // signal for changing the selection in the difficulty screen
     // more signals for the difficulty selection screen
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
     bool isCellOccupied(const QPoint&);
     void checkForFilledLines();
     void drawLogicalPlayfield();
     QPixmap getTetrisPixmapFromMasterImage(quint8 identifier);
     QPixmap getPixmapString(QString src);
   private:
     QHash<TetrisImageKey,TetrisImageItem> imgCont;
     QHash<BlockStyle,QPixmap> blockStyles;
     QHash<AlnumImageKey,QPixmap> characterImgs;
     GameState state;
     TetrisImageKey menuSelection;
     TetrisImageKey difSelection;
     QTimer timer;
     QSize sz;
     QPixmap frame, images, backgroundLayer, foregroundLayer, activeBlockLayer;
     QPainter painter;
     quint32 speed, lineCount;
     quint64 score;
     QHash<BlockInfo::BlockType,quint32> blockCount;
     const quint8 cellWidth = 20, cellHeight = 20, pfieldWidth = 10, pfieldHeight = 20;
     quint16 *playField, filled;
     BlockInfo activeBlock;
     QQueue<BlockInfo::BlockType> blockQueue;
};

struct TetrisImageItem{
      TetrisImageItem(QRect = QRect(), QPixmap = QPixmap());
      QRect rect;
      QPixmap img;
};
#endif // TETRISSCREEN_H
