#include "tetrisimageitem.h"
#include <QPixmap>

TetrisImageItem::TetrisImageItem(QRect r /*= 0*/, QPixmap p /*= 0*/):rect(r),img(p){
    if (img.isNull()){
        img = QPixmap(r.width(),r.height());
        img.fill(Qt::red);
    }
}
