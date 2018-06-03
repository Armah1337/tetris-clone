#include <QApplication>
#include "tetrisscreen.h"

int main(int argc, char* argv[]){
    QApplication app(argc,argv);
    TetrisScreen* win = new TetrisScreen();
    win->show();
    app.exec();
    return 0;
}
