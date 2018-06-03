#include <QVector>
#include <QPoint>
#include <QObject>
#ifndef BLOCKINFO_H
#define BLOCKINFO_H

/* This is the class responsible for keeping information about the active block's type, orientation and coordinates, and manipulating the block's contents */

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

#endif // BLOCKINFO_H
