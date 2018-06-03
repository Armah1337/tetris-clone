#include "blockinfo.h"
#include <QVector>
#include <QPoint>

void BlockInfo::setNewBlock(BlockType t){
    blocks.clear();
    b_type = t;
    b_orient = Up;
    switch(b_type){
    case I: blocks << QPoint(3,0) << QPoint(4,0) << QPoint(5,0) << QPoint(6,0); break;
    case J: blocks << QPoint(3,0) << QPoint(4,0) << QPoint(5,0) << QPoint(5,1); break;
    case L: blocks << QPoint(3,1) << QPoint(3,0) << QPoint(4,0) << QPoint(5,0); break;
    case O: blocks << QPoint(4,0) << QPoint(5,0) << QPoint(5,1) << QPoint(4,1); break;
    case S: blocks << QPoint(3,1) << QPoint(4,1) << QPoint(4,0) << QPoint(5,0); break;
    case T: blocks << QPoint(3,0) << QPoint(4,1) << QPoint(5,0) << QPoint(4,0); break;
    case Z: blocks << QPoint(3,0) << QPoint(4,0) << QPoint(4,1) << QPoint(5,1); break;
    }
}

void BlockInfo::moveBlock(qint8 xOffset, qint8 yOffset){
    for (auto &p : blocks){
      p.setX(p.x()+xOffset);
      p.setY(p.y()+yOffset);
    }
    emit redrawRequested();
}


void BlockInfo::rotateBlock(BlockOrientation orient, const QVector<QPoint>& rotBlocks){
    b_orient = orient;
    blocks = rotBlocks;
    emit redrawRequested();
}

QVector<QPoint> BlockInfo::getRotatedBlocks(BlockOrientation orient){
    QVector<QPoint> rotBlocks = blocks;
    switch(b_type){
    case I: if (orient == Left || orient == Right){
              for (quint8 i = 0; i < 4; i++){
                rotBlocks[i].setX(rotBlocks[2].x());
                rotBlocks[i].setY(rotBlocks[2].y() + (i-2));
              }
            }
            else{
              for (quint8 i = 0; i < 4; i++){
                rotBlocks[i].setY(rotBlocks[2].y());
                rotBlocks[i].setX(rotBlocks[2].x() + (i-2));
              }
            }
            break;
    case J: switch(orient){
            case Up: rotBlocks[0].setX(rotBlocks[1].x()-1); rotBlocks[0].setY(rotBlocks[1].y());
                     rotBlocks[2].setX(rotBlocks[1].x()+1); rotBlocks[2].setY(rotBlocks[1].y());
                     rotBlocks[3].setX(rotBlocks[2].x()); rotBlocks[3].setY(rotBlocks[2].y()+1);
                     break;
            case Right: rotBlocks[0].setX(rotBlocks[1].x()); rotBlocks[0].setY(rotBlocks[1].y()-1);
                        rotBlocks[2].setX(rotBlocks[1].x()); rotBlocks[2].setY(rotBlocks[1].y()+1);
                        rotBlocks[3].setX(rotBlocks[2].x()-1); rotBlocks[3].setY(rotBlocks[2].y());
                        break;
            case Down: rotBlocks[0].setX(rotBlocks[1].x()+1); rotBlocks[0].setY(rotBlocks[1].y());
                       rotBlocks[2].setX(rotBlocks[1].x()-1); rotBlocks[2].setY(rotBlocks[1].y());
                       rotBlocks[3].setX(rotBlocks[2].x()); rotBlocks[3].setY(rotBlocks[2].y()-1);
                       break;
            case Left:  rotBlocks[0].setX(rotBlocks[1].x()); rotBlocks[0].setY(rotBlocks[1].y()+1);
                        rotBlocks[2].setX(rotBlocks[1].x()); rotBlocks[2].setY(rotBlocks[1].y()-1);
                        rotBlocks[3].setX(rotBlocks[2].x()+1); rotBlocks[3].setY(rotBlocks[2].y());
                        break;
            }
            break;
    case L: switch(orient){
            case Up: rotBlocks[3].setX(rotBlocks[2].x()+1); rotBlocks[3].setY(rotBlocks[2].y());
                     rotBlocks[1].setX(rotBlocks[2].x()-1); rotBlocks[1].setY(rotBlocks[2].y());
                     rotBlocks[0].setX(rotBlocks[1].x()); rotBlocks[0].setY(rotBlocks[1].y()+1);
                     break;
            case Right: rotBlocks[3].setX(rotBlocks[2].x()); rotBlocks[3].setY(rotBlocks[2].y()+1);
                        rotBlocks[1].setX(rotBlocks[2].x()); rotBlocks[1].setY(rotBlocks[2].y()-1);
                        rotBlocks[0].setX(rotBlocks[1].x()-1); rotBlocks[0].setY(rotBlocks[1].y());
                        break;
            case Down: rotBlocks[3].setX(rotBlocks[2].x()-1); rotBlocks[3].setY(rotBlocks[2].y());
                       rotBlocks[1].setX(rotBlocks[2].x()+1); rotBlocks[1].setY(rotBlocks[2].y());
                       rotBlocks[0].setX(rotBlocks[1].x()); rotBlocks[0].setY(rotBlocks[1].y()-1);
                       break;
            case Left:  rotBlocks[3].setX(rotBlocks[2].x()); rotBlocks[3].setY(rotBlocks[2].y()-1);
                        rotBlocks[1].setX(rotBlocks[2].x()); rotBlocks[1].setY(rotBlocks[2].y()+1);
                        rotBlocks[0].setX(rotBlocks[1].x()+1); rotBlocks[0].setY(rotBlocks[1].y());
                        break;
            }
            break;
    case S: if (orient == Up || orient == Down){
              rotBlocks[3].setX(rotBlocks[2].x()+1); rotBlocks[3].setY(rotBlocks[2].y());
              rotBlocks[1].setX(rotBlocks[2].x()); rotBlocks[1].setY(rotBlocks[2].y()+1);
              rotBlocks[0].setX(rotBlocks[1].x()-1); rotBlocks[0].setY(rotBlocks[1].y());
            }
            else {
              rotBlocks[3].setX(rotBlocks[2].x()); rotBlocks[3].setY(rotBlocks[2].y()-1);
              rotBlocks[1].setX(rotBlocks[2].x()+1); rotBlocks[1].setY(rotBlocks[2].y());
              rotBlocks[0].setX(rotBlocks[1].x()); rotBlocks[0].setY(rotBlocks[1].y()+1);
            }
            break;
    case Z: if (orient == Up || orient == Down){
              rotBlocks[3].setX(rotBlocks[2].x()+1); rotBlocks[3].setY(rotBlocks[2].y());
              rotBlocks[1].setX(rotBlocks[2].x()); rotBlocks[1].setY(rotBlocks[2].y()-1);
              rotBlocks[0].setX(rotBlocks[1].x()-1); rotBlocks[0].setY(rotBlocks[1].y());
            }
            else {
              rotBlocks[3].setX(rotBlocks[2].x()); rotBlocks[3].setY(rotBlocks[2].y()+1);
              rotBlocks[1].setX(rotBlocks[2].x()+1); rotBlocks[1].setY(rotBlocks[2].y());
              rotBlocks[0].setX(rotBlocks[1].x()); rotBlocks[0].setY(rotBlocks[1].y()-1);
            }
            break;
    case T: switch(orient){
            case Up: rotBlocks[0].setX(rotBlocks[3].x()-1); rotBlocks[0].setY(rotBlocks[3].y());
                     rotBlocks[1].setX(rotBlocks[3].x()); rotBlocks[1].setY(rotBlocks[3].y()+1);
                     rotBlocks[2].setX(rotBlocks[3].x()+1); rotBlocks[2].setY(rotBlocks[3].y());
                     break;
            case Right: rotBlocks[0].setX(rotBlocks[3].x()); rotBlocks[0].setY(rotBlocks[3].y()-1);
                        rotBlocks[1].setX(rotBlocks[3].x()-1); rotBlocks[1].setY(rotBlocks[3].y());
                        rotBlocks[2].setX(rotBlocks[3].x()); rotBlocks[2].setY(rotBlocks[3].y()+1);
                        break;
            case Down: rotBlocks[0].setX(rotBlocks[3].x()+1); rotBlocks[0].setY(rotBlocks[3].y());
                       rotBlocks[1].setX(rotBlocks[3].x()); rotBlocks[1].setY(rotBlocks[3].y()-1);
                       rotBlocks[2].setX(rotBlocks[3].x()-1); rotBlocks[2].setY(rotBlocks[3].y());
                       break;
            case Left: rotBlocks[0].setX(rotBlocks[3].x()); rotBlocks[0].setY(rotBlocks[3].y()+1);
                       rotBlocks[1].setX(rotBlocks[3].x()+1); rotBlocks[1].setY(rotBlocks[3].y());
                       rotBlocks[2].setX(rotBlocks[3].x()); rotBlocks[2].setY(rotBlocks[3].y()-1);
                       break;
            }
            break;
    case O: break;
    }
    return rotBlocks;
}
