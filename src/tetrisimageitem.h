#include <QRect>
#include <QPixmap>
#ifndef TETRISIMAGEITEM_H
#define TETRISIMAGEITEM_H

/* This is the struct that contains information about the necessary image's geometrical parameters and visual representation */

struct TetrisImageItem{
      TetrisImageItem(QRect = QRect(), QPixmap = QPixmap());
      QRect rect;
      QPixmap img;
};

#endif // TETRISIMAGEITEM_H
