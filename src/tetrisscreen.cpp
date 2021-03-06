#include "tetrisscreen.h"
#include "tetrisimageitem.h"
#include "blockinfo.h"
#include <QKeyEvent>
#include <QApplication>
#include <QDesktopWidget>
#include <QImage>
#include <QDateTime>
#include <QtCore/qmath.h>
//#define TESTBRUSHES

// CONSTRUCTORS
TetrisScreen::TetrisScreen():QOpenGLWindow(),menuSelection(None),difSelection(None),
                             sz(640,480),frame(640,480),images("TetrisMasterImagePNG.png"),backgroundLayer(640,480),foregroundLayer(640,480),activeBlockLayer(640,480),speed(1),playField(new quint16[20]()),filled(0)
                             {
    connect(this,SIGNAL(frameSwapped()),SLOT(update())); // enable vsync refresh
    connect(this,SIGNAL(menuSelectionChanged(TetrisImageKey,TetrisImageKey)),
                 SLOT(highlightMenuSelection(TetrisImageKey,TetrisImageKey)));
    connect(this,SIGNAL(difficultySelectionChanged(TetrisImageKey,TetrisImageKey)),
                 SLOT(highlightDifficultySelection(TetrisImageKey,TetrisImageKey)));
    connect(this,SIGNAL(stateChangeTriggered(GameState)),SLOT(setGameState(GameState)));
    connect(this,SIGNAL(exitPressed()),qApp,SLOT(quit()));
    connect(&activeBlock,SIGNAL(redrawRequested()),this,SLOT(handleRedrawRequest()));
    connect(this,SIGNAL(blockReachedObstacle()),SLOT(setBlock()));
    connect(this,SIGNAL(linesFilled(QVector<quint8>)),SLOT(clearFilledLines(QVector<quint8>)));
    connect(&timerDropDown,SIGNAL(timeout()),this,SLOT(dropDown()));
    connect(&timerLeft,SIGNAL(timeout()),this,SLOT(timerLeftTimeoutHandler()));
    connect(&timerRight,SIGNAL(timeout()),this,SLOT(timerRightTimeoutHandler()));
    connect(&timerUnpausedRemainingTime,SIGNAL(timeout()),this,SLOT(dropDown()));
    connect(&timerUnpausedNormal,SIGNAL(timeout()),this,SLOT(startDropDownTimerAtNormal()));
    connect(&timerUnpausedAccelerated,SIGNAL(timeout()),this,SLOT(startDropDownTimerAtAccelerated()));
    connect(&timerGameOverLine,SIGNAL(timeout()),this,SLOT(drawGameOverLine()));
    connect(this,SIGNAL(blockReachedObstacle()),SLOT(updateStats()));
    timerUnpausedNormal.setSingleShot(true);
    timerUnpausedAccelerated.setSingleShot(true);
    timerUnpausedRemainingTime.setSingleShot(true);

    imgCont = {  {Logo,  {QRect((frame.width()-350)/2,30,350,100),getTetrisPixmapFromMasterImage(Logo)}},
                 {Play,  {QRect((frame.width()-250)/2,170,250,70),getTetrisPixmapFromMasterImage(Play)}},
                 {Help,  {QRect((frame.width()-250)/2,270,250,70),getTetrisPixmapFromMasterImage(Help)}},
                 {Exit,  {QRect((frame.width()-250)/2,370,250,70),getTetrisPixmapFromMasterImage(Exit)}},
                 {Speed, {QRect(frame.width()/2-155,80,155,40),getTetrisPixmapFromMasterImage(Speed)}},
                 {SpeedLeftArrow,  {QRect(frame.width()/2+13,80,40,40),getTetrisPixmapFromMasterImage(SpeedLeftArrow)}},
                 {SpeedRightArrow,  {QRect(frame.width()/2+117,80,40,40),getTetrisPixmapFromMasterImage(SpeedRightArrow)}},
                 {Start,  {QRect((frame.width()-160)/2,360,160,80),getTetrisPixmapFromMasterImage(Start)}}
              };
    blockStyles = {  {BlockStyle1, getTetrisPixmapFromMasterImage(BlockStyle1)},
                     {BlockStyle2, getTetrisPixmapFromMasterImage(BlockStyle2)},
                     {BlockStyle3, getTetrisPixmapFromMasterImage(BlockStyle3)},
                     {BlockStyle4, getTetrisPixmapFromMasterImage(BlockStyle4)},
                     {BlockStyle5, getTetrisPixmapFromMasterImage(BlockStyle5)},
                     {BlockStyle6, getTetrisPixmapFromMasterImage(BlockStyle6)},
                     {BlockStyle7, getTetrisPixmapFromMasterImage(BlockStyle7)}
                  };
    characterImgs = { {Digit_0, getTetrisPixmapFromMasterImage(Digit_0)},
                      {Digit_1, getTetrisPixmapFromMasterImage(Digit_1)},
                      {Digit_2, getTetrisPixmapFromMasterImage(Digit_2)},
                      {Digit_3, getTetrisPixmapFromMasterImage(Digit_3)},
                      {Digit_4, getTetrisPixmapFromMasterImage(Digit_4)},
                      {Digit_5, getTetrisPixmapFromMasterImage(Digit_5)},
                      {Digit_6, getTetrisPixmapFromMasterImage(Digit_6)},
                      {Digit_7, getTetrisPixmapFromMasterImage(Digit_7)},
                      {Digit_8, getTetrisPixmapFromMasterImage(Digit_8)},
                      {Digit_9, getTetrisPixmapFromMasterImage(Digit_9)},
                      {A, getTetrisPixmapFromMasterImage(A)},
                      {B, getTetrisPixmapFromMasterImage(B)},
                      {C, getTetrisPixmapFromMasterImage(C)},
                      {D, getTetrisPixmapFromMasterImage(D)},
                      {E, getTetrisPixmapFromMasterImage(E)},
                      {F, getTetrisPixmapFromMasterImage(F)},
                      {G, getTetrisPixmapFromMasterImage(G)},
                      {H, getTetrisPixmapFromMasterImage(H)},
                      {I, getTetrisPixmapFromMasterImage(I)},
                      {J, getTetrisPixmapFromMasterImage(J)},
                      {K, getTetrisPixmapFromMasterImage(K)},
                      {L, getTetrisPixmapFromMasterImage(L)},
                      {M, getTetrisPixmapFromMasterImage(M)},
                      {N, getTetrisPixmapFromMasterImage(N)},
                      {O, getTetrisPixmapFromMasterImage(O)},
                      {P, getTetrisPixmapFromMasterImage(P)},
                      {Q, getTetrisPixmapFromMasterImage(Q)},
                      {R, getTetrisPixmapFromMasterImage(R)},
                      {S, getTetrisPixmapFromMasterImage(S)},
                      {T, getTetrisPixmapFromMasterImage(T)},
                      {U, getTetrisPixmapFromMasterImage(U)},
                      {V, getTetrisPixmapFromMasterImage(V)},
                      {W, getTetrisPixmapFromMasterImage(W)},
                      {X, getTetrisPixmapFromMasterImage(X)},
                      {Y, getTetrisPixmapFromMasterImage(Y)},
                      {Z, getTetrisPixmapFromMasterImage(Z)}};
    #ifdef TESTBRUSHES
    blockStyles[BlockStyle1].fill(Qt::red);
    blockStyles[BlockStyle2].fill(Qt::blue);
    blockStyles[BlockStyle3].fill(Qt::green);
    blockStyles[BlockStyle4].fill(Qt::white);
    blockStyles[BlockStyle5].fill(Qt::yellow);
    blockStyles[BlockStyle6].fill(Qt::cyan);
    blockStyles[BlockStyle7].fill(Qt::magenta);
    #endif
    setMaximumSize(sz); // preventing the window from being resized
    setMinimumSize(sz);

    QRect rec = QApplication::desktop()->geometry();

    qint16 width = rec.width(), height = rec.height();
    setGeometry((width-sz.width())/2,(height-sz.height())/2,sz.width(),sz.height());

    setTitle("Tetris");

    setGameState(Menu);
    setTimeState(Unpaused);
    setMenuSelection(Play);
    setDifficultySelection(SpeedLeftArrow);
    drawPlayfield();
    foregroundLayer.fill(Qt::transparent);
    qsrand(QDateTime::currentMSecsSinceEpoch());
    for (quint8 i = 0; i < pfieldWidth; i++)
      filled = filled | (1 << i);
}

QPixmap TetrisScreen::getTetrisPixmapFromMasterImage(quint8 identifier){
    QRect portion;
    switch(identifier){
    case Logo: portion = QRect(0,0,350,100); break;
    case Start: portion = QRect(0,100,160,80); break;
    case Play: portion = QRect(350,0,250,70); break;
    case Help: portion = QRect(600,0,250,70); break;
    case Exit: portion = QRect(850,0,250,70); break;
    case Speed: portion = QRect(1100,0,155,40); break;
    case SpeedLeftArrow: portion = QRect(1255,0,40,40); break;
    case SpeedRightArrow: portion = QRect(1295,0,40,40); break;
    case Digit_0: portion = QRect(160, 100, 15, 21); break;
    case Digit_1: portion = QRect(175, 100, 15, 21); break;
    case Digit_2: portion = QRect(190, 100, 15, 21); break;
    case Digit_3: portion = QRect(205, 100, 15, 21); break;
    case Digit_4: portion = QRect(220, 100, 15, 21); break;
    case Digit_5: portion = QRect(235, 100, 15, 21); break;
    case Digit_6: portion = QRect(250, 100, 15, 21); break;
    case Digit_7: portion = QRect(265, 100, 15, 21); break;
    case Digit_8: portion = QRect(280, 100, 15, 21); break;
    case Digit_9: portion = QRect(295, 100, 15, 21); break;
    case A: portion = QRect(160, 121, 15, 21); break;
    case B: portion = QRect(175, 121, 15, 21); break;
    case C: portion = QRect(190, 121, 15, 21); break;
    case D: portion = QRect(205, 121, 15, 21); break;
    case E: portion = QRect(220, 121, 15, 21); break;
    case F: portion = QRect(235, 121, 15, 21); break;
    case G: portion = QRect(250, 121, 15, 21); break;
    case H: portion = QRect(265, 121, 15, 21); break;
    case I: portion = QRect(280, 121, 15, 21); break;
    case J: portion = QRect(295, 121, 15, 21); break;
    case K: portion = QRect(160, 142, 15, 21); break;
    case L: portion = QRect(175, 142, 15, 21); break;
    case M: portion = QRect(190, 142, 15, 21); break;
    case N: portion = QRect(205, 142, 15, 21); break;
    case O: portion = QRect(220, 142, 15, 21); break;
    case P: portion = QRect(235, 142, 15, 21); break;
    case Q: portion = QRect(250, 142, 15, 21); break;
    case R: portion = QRect(265, 142, 15, 21); break;
    case S: portion = QRect(280, 142, 15, 21); break;
    case T: portion = QRect(295, 142, 15, 21); break;
    case U: portion = QRect(160, 163, 15, 21); break;
    case V: portion = QRect(175, 163, 15, 21); break;
    case W: portion = QRect(190, 163, 15, 21); break;
    case X: portion = QRect(205, 163, 15, 21); break;
    case Y: portion = QRect(220, 163, 15, 21); break;
    case Z: portion = QRect(235, 163, 15, 21); break;
    case BlockStyle1: portion = QRect(0,187,7,7); break;
    case BlockStyle2: portion = QRect(7,187,7,7); break;
    case BlockStyle3: portion = QRect(14,187,7,7); break;
    case BlockStyle4: portion = QRect(21,187,7,7); break;
    case BlockStyle5: portion = QRect(28,187,7,7); break;
    case BlockStyle6: portion = QRect(35,187,7,7); break;
    case BlockStyle7: portion = QRect(42,187,7,7); break;
    default: break;
    }
    QPixmap px(portion.width(),portion.height());
    px.fill(Qt::transparent);
    painter.begin(&px);
    painter.drawPixmap(0,0,px.width(),px.height(),images,portion.x(),portion.y(),portion.width(),portion.height());
    painter.end();
    return px;
}

QPixmap TetrisScreen::getPixmapString(QString src){
    QPixmap pix(src.length()*15,21), charPix;
    pix.fill(Qt::transparent);
    quint64 xOffset = 0;
    QPainter p(&pix);
    QString s = src.toUpper();
    foreach (const QChar c, s){
        switch(c.toLatin1()){
        case '0': charPix = characterImgs[Digit_0]; break;
        case '1': charPix = characterImgs[Digit_1]; break;
        case '2': charPix = characterImgs[Digit_2]; break;
        case '3': charPix = characterImgs[Digit_3]; break;
        case '4': charPix = characterImgs[Digit_4]; break;
        case '5': charPix = characterImgs[Digit_5]; break;
        case '6': charPix = characterImgs[Digit_6]; break;
        case '7': charPix = characterImgs[Digit_7]; break;
        case '8': charPix = characterImgs[Digit_8]; break;
        case '9': charPix = characterImgs[Digit_9]; break;
        case 'A': charPix = characterImgs[A]; break;
        case 'B': charPix = characterImgs[B]; break;
        case 'C': charPix = characterImgs[C]; break;
        case 'D': charPix = characterImgs[D]; break;
        case 'E': charPix = characterImgs[E]; break;
        case 'F': charPix = characterImgs[F]; break;
        case 'G': charPix = characterImgs[G]; break;
        case 'H': charPix = characterImgs[H]; break;
        case 'I': charPix = characterImgs[I]; break;
        case 'J': charPix = characterImgs[J]; break;
        case 'K': charPix = characterImgs[K]; break;
        case 'L': charPix = characterImgs[L]; break;
        case 'M': charPix = characterImgs[M]; break;
        case 'N': charPix = characterImgs[N]; break;
        case 'O': charPix = characterImgs[O]; break;
        case 'P': charPix = characterImgs[P]; break;
        case 'Q': charPix = characterImgs[Q]; break;
        case 'R': charPix = characterImgs[R]; break;
        case 'S': charPix = characterImgs[S]; break;
        case 'T': charPix = characterImgs[T]; break;
        case 'U': charPix = characterImgs[U]; break;
        case 'V': charPix = characterImgs[V]; break;
        case 'W': charPix = characterImgs[W]; break;
        case 'X': charPix = characterImgs[X]; break;
        case 'Y': charPix = characterImgs[Y]; break;
        case 'Z': charPix = characterImgs[Z]; break;
        default:if (c.isSpace()) xOffset += 15;
                continue;
        }
        p.drawPixmap(xOffset,0,charPix);
        xOffset += 15;
    }
    return pix;
}

// SLOTS
void TetrisScreen::beginMenu(){
    QFile scoreFile("topscore.bin");
    bool exists = scoreFile.exists();
    scoreFile.open(QIODevice::ReadWrite);
    QDataStream in(&scoreFile);
    if (!exists){
      topScore = 0;
    }else{
     in >> topScore;
    }
    scoreFile.close();
    frame.fill(Qt::black);
    painter.begin(&frame);
    painter.drawPixmap(imgCont[Logo].rect,imgCont[Logo].img);
    painter.drawPixmap(imgCont[Play].rect,imgCont[Play].img);
    painter.drawPixmap(imgCont[Help].rect,imgCont[Help].img);
    painter.drawPixmap(imgCont[Exit].rect,imgCont[Exit].img);
    painter.drawPixmap(frame.width()/2 + 145,frame.height()/2 - 60,getPixmapString("TOP SCORE").scaledToHeight(21*1.2));
    painter.drawPixmap(frame.width()/2 + 145,frame.height()/2 - 30, getPixmapString(QString("%1").arg(QString::number(topScore),10,'0')).scaledToHeight(21*1.1));
    if (isNewRecord) painter.drawPixmap(frame.width()/2 + 170,frame.height()/2,getPixmapString("NEW RECORD").scaledToHeight(21*0.8));
    painter.drawPixmap(0,frame.height()-20,getPixmapString("BY ARMAH1337").scaledToHeight(21*0.75));
    painter.end();
}

void TetrisScreen::beginHelp(){
    frame.fill(Qt::black);
    painter.begin(&frame);
    painter.drawPixmap(50,frame.height()/2 - 210,getPixmapString("USE THE LEFT AND RIGHT ARROW KEYS TO MOVE THE BLOCK").scaledToHeight(21*0.75));
    painter.drawPixmap(50,frame.height()/2 - 150,getPixmapString("PRESS UP TO INSTANTLY DROP THE BLOCK").scaledToHeight(21*0.75));
    painter.drawPixmap(50,frame.height()/2 - 90,getPixmapString("PRESS DOWN TO ACCELERATE THE DROP").scaledToHeight(21*0.75));
    painter.drawPixmap(50,frame.height()/2 - 30,getPixmapString("PRESS A TO ROTATE THE BLOCK COUNTER CLOCKWISE").scaledToHeight(21*0.75));
    painter.drawPixmap(50,frame.height()/2 + 30,getPixmapString("PRESS D TO ROTATE THE BLOCK CLOCKWISE").scaledToHeight(21*0.75));
    painter.drawPixmap(50,frame.height()/2 + 90,getPixmapString("PRESS P TO PAUSE THE GAME").scaledToHeight(21*0.75));
    painter.drawPixmap(50,frame.height()/2 + 180,getPixmapString("PRESS ENTER TO GO BACK").scaledToHeight(21*0.75));
    painter.end();
}

void TetrisScreen::beginDifficultySelect(){
    frame.fill(Qt::black);
    painter.begin(&frame);
    painter.setPen(QPen(Qt::white,5,Qt::SolidLine,Qt::FlatCap,Qt::MiterJoin));
    painter.drawPixmap(frame.width()/2 - 120, 10, getPixmapString("PICK YOUR POISON"));
    painter.drawRect(frame.width()/2 - 170, 50, 340, 400); // border around all the other parts
    painter.drawPixmap(imgCont[Speed].rect,imgCont[Speed].img);
    painter.drawPixmap(imgCont[SpeedLeftArrow].rect,imgCont[SpeedLeftArrow].img);
    painter.setPen(QPen(Qt::white,3,Qt::SolidLine,Qt::FlatCap,Qt::MiterJoin));
    painter.drawRect(frame.width()/2 + 65, 80, 40, 40); // border around the speed level, replace with image
    drawSpeed();
    painter.drawPixmap(imgCont[SpeedRightArrow].rect,imgCont[SpeedRightArrow].img);
    painter.drawPixmap(imgCont[Start].rect,imgCont[Start].img);
    painter.end();
}

void TetrisScreen::beginGameplay(){
        score = 0;
        lineCount = 0;
        blockCount = {{BlockInfo::BlockType::I, 0},
                      {BlockInfo::BlockType::J, 0},
                      {BlockInfo::BlockType::L, 0},
                      {BlockInfo::BlockType::O, 0},
                      {BlockInfo::BlockType::S, 0},
                      {BlockInfo::BlockType::T, 0},
                      {BlockInfo::BlockType::Z, 0}};
        blockQueue.clear();
        foregroundLayer.fill(Qt::transparent);
        painter.begin(&backgroundLayer);
        painter.drawPixmap(470,420,getPixmapString("SPEED").scaledToHeight(21*1.3));
        painter.drawPixmap(frame.width()/2 - 100,7,getPixmapString("LINES").scaledToHeight(21*1.2));
        painter.drawPixmap(50,7,getPixmapString("SCORE").scaledToHeight(21*1.2));
        painter.setPen(QPen(Qt::white,5,Qt::SolidLine,Qt::FlatCap,Qt::MiterJoin));
        painter.drawRect(10,80,180,375);
        for (BlockInfo::BlockType i = (BlockInfo::BlockType)0; i <= (BlockInfo::BlockType)6; i = (BlockInfo::BlockType)(i + 1)){
        BlockInfo b;
        b.setNewBlock(i);
        QVector<QPoint> blocks = b.getBlocks();
        BlockStyle style;
        switch(b.blockType()){
        case BlockInfo::I: style = BlockStyle1; break;
        case BlockInfo::J: style = BlockStyle2; break;
        case BlockInfo::L: style = BlockStyle3; break;
        case BlockInfo::O: style = BlockStyle4; break;
        case BlockInfo::S: style = BlockStyle5; break;
        case BlockInfo::T: style = BlockStyle6; break;
        case BlockInfo::Z: style = BlockStyle7; break;
        }
        foreach (QPoint p, blocks){
          painter.drawPixmap(-30 + cellWidth*p.x(),100 + i*50 + cellHeight*p.y(), cellWidth, cellHeight, blockStyles[style]);
        }
        }
        painter.end();
        updateStats();
        updateBlockQueue();
        for (quint8 i = 0; i < pfieldHeight; i++)
        playField[i] = 0;
        spawnBlock();
}

void TetrisScreen::pause(){
  if (gameState == Gameplay){
    painter.begin(&frame);
    painter.drawPixmap(frame.width()/2 + 165, 40, getPixmapString("PAUSED").scaledToHeight(21*1.25));
    painter.end();
    if (timerDropDown.remainingTime() != -1)
      dropDownTimeLeft = timerDropDown.remainingTime();
    else{
      dropDownTimeLeft = timerUnpausedRemainingTime.remainingTime();
      timerUnpausedNormal.stop();
      timerUnpausedAccelerated.stop();
    }
    timerDropDown.stop();
  }
}
void TetrisScreen::unpause(){
  if (gameState == Gameplay){
    drawBlock(activeBlockLayer);
    timerUnpausedRemainingTime.start(dropDownTimeLeft);
    if (timerDropDown.interval() != 40) timerUnpausedNormal.start(dropDownTimeLeft);
    else timerUnpausedAccelerated.start(dropDownTimeLeft);
  }
}

void TetrisScreen::startDropDownTimerAtNormal(){
    timerDropDown.start(1100 - speed*100);
}

void TetrisScreen::startDropDownTimerAtAccelerated(){
    timerDropDown.start(40);
}

void TetrisScreen::setGameState(GameState s){
    gameState = s;
    switch (gameState){
    case Menu: beginMenu(); break;
    case HelpMe: beginHelp(); break;
    case Difficulty: beginDifficultySelect(); break;
    case Gameplay: beginGameplay(); break;
    case GameOver: gameOver(); break;
    }
}

void TetrisScreen::setTimeState(TimeState s){
    timeState = s;
    if (timeState == Paused) pause();
    else if (timeState == Unpaused) unpause();
}

void TetrisScreen::setMenuSelection(TetrisImageKey s){
    TetrisImageKey old = menuSelection;
    menuSelection = s;
    emit menuSelectionChanged(old,menuSelection);
}

void TetrisScreen::setDifficultySelection(TetrisImageKey s){
    TetrisImageKey old = difSelection;
    difSelection = s;
    emit difficultySelectionChanged(old,difSelection);
}

void TetrisScreen::pressMenuButton(){
    switch(menuSelection){
    case Play: emit stateChangeTriggered(Difficulty); break;
    case Help: emit stateChangeTriggered(HelpMe); break;
    case Exit: emit exitPressed(); break;
    default: break;
    }
}

void TetrisScreen::pressDifficultyButton(){

    switch(difSelection){
    case SpeedLeftArrow: speed == 1 ? speed = 9 : speed--; drawSpeed(); break;
    case SpeedRightArrow: speed == 9 ? speed = 1 : speed++; drawSpeed();  break;
    case Start: emit stateChangeTriggered(Gameplay); break;
    default: break;
    }
}
//
// EVENTS
void TetrisScreen::keyPressEvent(QKeyEvent *event){
    if (!event->isAutoRepeat())
    switch(gameState){
    case Menu: menuKeyPressEvent(event); break;
    case Difficulty: difficultyKeyPressEvent(event); break;
    case Gameplay: gameplayKeyPressEvent(event); break;
    case HelpMe:
    case GameOver: if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) setGameState(Menu);
    }
}

void TetrisScreen::keyReleaseEvent(QKeyEvent *event){
    if (!event->isAutoRepeat())
    switch(gameState){
    case Menu:
    case HelpMe:
    case Difficulty:
    case GameOver: break;
    case Gameplay: gameplayKeyReleaseEvent(event); break;
    }
}

void TetrisScreen::menuKeyPressEvent(QKeyEvent *event){
    switch(event->key()){
    case Qt::Key_Up: if (menuSelection - 1 < Play) setMenuSelection(Exit);
                     else setMenuSelection(TetrisImageKey(menuSelection - 1));
                     break;
    case Qt::Key_Down: if (menuSelection + 1 > Exit) setMenuSelection(Play);
                       else setMenuSelection(TetrisImageKey(menuSelection + 1));
                       break;
    case Qt::Key_Enter:
    case Qt::Key_Return: pressMenuButton();
    }
}

void TetrisScreen::difficultyKeyPressEvent(QKeyEvent *event){ // rework the whole selection layout, this shit sucks
    switch(event->key()){
    case Qt::Key_Up: if (difSelection - 2 < SpeedLeftArrow) setDifficultySelection(Start);
                     else setDifficultySelection(TetrisImageKey(difSelection - 2));
                     break;
    case Qt::Key_Down: if (difSelection == Start) setDifficultySelection(SpeedLeftArrow);
                       else setDifficultySelection(Start);
                       break;
    case Qt::Key_Left:  if (difSelection == SpeedLeftArrow) setDifficultySelection(TetrisImageKey(difSelection + 1));
                   else if (difSelection == SpeedRightArrow) setDifficultySelection(TetrisImageKey(difSelection - 1));
                        break;
    case Qt::Key_Right: if (difSelection == SpeedRightArrow) setDifficultySelection(TetrisImageKey(difSelection - 1));
                   else if (difSelection == SpeedLeftArrow) setDifficultySelection(TetrisImageKey(difSelection + 1));
                        break;
    case Qt::Key_Enter:
    case Qt::Key_Return: pressDifficultyButton();
    }
}

void TetrisScreen::gameplayKeyPressEvent(QKeyEvent *event){
    switch(event->key()){
    case Qt::Key_Up:  while(tryToMove(0,1)); setBlock(); if (gameState != GameOver) startDropDownTimerAtNormal(); break;
    case Qt::Key_Left: timerLeft.start(40); break;
    case Qt::Key_Right: timerRight.start(40); break;
    case Qt::Key_Down: timerDropDown.start(40); timerDropDownCnt = 0;break;
    default:
      switch(event->nativeScanCode()){
        case 30: tryToRotate(activeBlock.blockOrientation() == BlockInfo::BlockOrientation::Up ? BlockInfo::BlockOrientation::Left : BlockInfo::BlockOrientation(activeBlock.blockOrientation()-1)); break;
        case 32: tryToRotate(activeBlock.blockOrientation() == BlockInfo::BlockOrientation::Left ? BlockInfo::BlockOrientation::Up : BlockInfo::BlockOrientation(activeBlock.blockOrientation()+1)); break;
        case 25: setTimeState(timeState == Paused ? Unpaused : Paused); break;
    }
    }
}

void TetrisScreen::gameplayKeyReleaseEvent(QKeyEvent *event){
    switch(event->key()){
    case Qt::Key_Left: if (!timerLeftCnt){ tryToMove(-1,0); } timerLeft.stop(); timerLeftCnt = 0; break;
    case Qt::Key_Right:if (!timerRightCnt){ tryToMove(1,0); } timerRight.stop(); timerRightCnt = 0; break;
    case Qt::Key_Down: if (!timerDropDownCnt){ dropDown(); } startDropDownTimerAtNormal();
    }
}

//

void TetrisScreen::highlightMenuSelection(TetrisImageKey Old, TetrisImageKey New){
    highlight(Old,New);
    if (gameState == Menu){
      painter.begin(&frame);
      painter.drawPixmap(imgCont[Old].rect,imgCont[Old].img);
      painter.drawPixmap(imgCont[New].rect,imgCont[New].img);
      painter.end();
    }
}
void TetrisScreen::highlightDifficultySelection(TetrisImageKey Old, TetrisImageKey New){
    highlight(Old,New);
    if (gameState == Difficulty){
      painter.begin(&frame);
      painter.drawPixmap(imgCont[Old].rect,imgCont[Old].img);
      painter.drawPixmap(imgCont[New].rect,imgCont[New].img);
      painter.end();
    }
}

void TetrisScreen::highlight(TetrisImageKey Old, TetrisImageKey New){
    QImage image;
    if (Old != None){
      image = imgCont[Old].img.toImage();
      image.invertPixels();
      imgCont[Old].img.convertFromImage(image);
    }
    image = imgCont[New].img.toImage();
    image.invertPixels();
    imgCont[New].img.convertFromImage(image);
}

void TetrisScreen::drawSpeed(){
    bool wasNotActive = false;
    if (!painter.isActive()){
      wasNotActive = true;
      painter.begin(&frame);
      painter.setPen(Qt::white);
    }
    painter.setBrush(Qt::black);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.drawPixmap(frame.width()/2 + 75, 85, getPixmapString(QString::number(speed)).scaledToHeight(21*1.5));
    if (wasNotActive) painter.end();
}

void TetrisScreen::drawPlayfield(){
    backgroundLayer.fill(Qt::black);
    painter.begin(&backgroundLayer);
    painter.setPen(QPen(Qt::gray,0,Qt::SolidLine,Qt::FlatCap,Qt::MiterJoin));
    painter.setBrush(Qt::transparent);
    for (int i = 0; i < pfieldHeight; i++)
      for (int j = 0; j < pfieldWidth; j++)
        painter.drawRect(PFIELD_ORIGIN_X + cellWidth*j,PFIELD_ORIGIN_Y + cellHeight*i, cellWidth, cellHeight);
    painter.setPen(QPen(Qt::white,5,Qt::SolidLine,Qt::FlatCap,Qt::MiterJoin));
    painter.drawRect(470, 130, cellWidth*7, 5*55);
    painter.drawPixmap(500, 90, getPixmapString("NEXT").scaledToHeight(21*1.5));
    painter.end();
}

void TetrisScreen::mergeLayersToFrame(){
    frame.fill(Qt::black);
    painter.begin(&frame);
    painter.drawPixmap(0,0,backgroundLayer);
    painter.drawPixmap(0,0,foregroundLayer);
    painter.drawPixmap(0,0,activeBlockLayer);
    painter.end();
}

// BLOCK RELATED FUNCTIONS

void TetrisScreen::updateBlockQueue(){
    if (blockQueue.empty())
      for (quint8 i = 0; i < 5; i++)
        blockQueue.enqueue(BlockInfo::BlockType(qrand() % 7));
    else blockQueue.enqueue(BlockInfo::BlockType(qrand() % 7));
    painter.begin(&foregroundLayer);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(450,150,cellWidth*7,blockQueue.size()*50,Qt::transparent);
    QQueue<BlockInfo::BlockType>blockQueueDrawCopy = blockQueue;
    for (quint8 i = 0; i < 5; i++){
      BlockInfo b;
      b.setNewBlock(blockQueueDrawCopy.dequeue());
      QVector<QPoint> blocks = b.getBlocks();
      BlockStyle style;
      switch(b.blockType()){
      case BlockInfo::I: style = BlockStyle1; break;
      case BlockInfo::J: style = BlockStyle2; break;
      case BlockInfo::L: style = BlockStyle3; break;
      case BlockInfo::O: style = BlockStyle4; break;
      case BlockInfo::S: style = BlockStyle5; break;
      case BlockInfo::T: style = BlockStyle6; break;
      case BlockInfo::Z: style = BlockStyle7; break;
      }
      foreach (QPoint p, blocks){
        painter.drawPixmap(450 + cellWidth*p.x(),150 + i*50 + cellHeight*p.y(), cellWidth, cellHeight, blockStyles[style]);
      }
    }
    painter.end();
}

void TetrisScreen::updateStats(){ //activate every time a block is set
    painter.begin(&foregroundLayer);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.drawPixmap(580,420,getPixmapString(QString::number(speed)).scaledToHeight(21*1.3));
    painter.drawPixmap(10, 40, getPixmapString(QString("%1").arg(QString::number(score),10,'0')).scaledToHeight(21*1.2));
    for (BlockInfo::BlockType i = (BlockInfo::BlockType)0; i <= (BlockInfo::BlockType)6; i = (BlockInfo::BlockType)(i + 1)){
      painter.drawPixmap(115, 100 + i*50, getPixmapString(QString("%1").arg(QString::number(blockCount[i]),4,'0')));
    }
    //amount of each block set
    painter.drawPixmap(frame.width()/2 + 30, 7, getPixmapString(QString("%1").arg(QString::number(lineCount),4,'0')).scaledToHeight(21*1.2));
    painter.end();
}

void TetrisScreen::spawnBlock(){
    //drawLogicalPlayfield();
    activeBlock.setNewBlock(blockQueue.dequeue());
    updateBlockQueue();
    if (!score) startDropDownTimerAtNormal();
    if(!tryToMove(0,0)){
      timerLeft.stop();
      timerRight.stop();
      timerDropDown.stop();
      emit stateChangeTriggered(GameOver);
      return;
    }
    drawBlock(activeBlockLayer);
}

void TetrisScreen::handleRedrawRequest(){
    drawBlock(activeBlockLayer);
}

void TetrisScreen::drawBlock(QPixmap& layer){
    activeBlockLayer.fill(Qt::transparent);
    painter.begin(&layer);
    QVector<QPoint> blocks = activeBlock.getBlocks(), ghostBlocks = blocks;
    quint8 ghostYOffset = 0;
    bool ghostFoundObstacle = false;
    do{
      for (auto p : ghostBlocks){
        p.setY(p.y() + ghostYOffset + 1);
        if (isCellOccupied(p)){ ghostFoundObstacle = true; break; }
      }
      if (!ghostFoundObstacle) ghostYOffset++;
    }while(!ghostFoundObstacle);
    for (auto &p : ghostBlocks)
      p.setY(p.y() + ghostYOffset);
    BlockStyle style;
    switch(activeBlock.blockType()){
    case BlockInfo::I: style = BlockStyle1; break;
    case BlockInfo::J: style = BlockStyle2; break;
    case BlockInfo::L: style = BlockStyle3; break;
    case BlockInfo::O: style = BlockStyle4; break;
    case BlockInfo::S: style = BlockStyle5; break;
    case BlockInfo::T: style = BlockStyle6; break;
    case BlockInfo::Z: style = BlockStyle7; break;
    }
    QImage ghostStyleImg = blockStyles[style].toImage().convertToFormat(QImage::Format_ARGB32_Premultiplied);
    for (quint8 y = 0; y < ghostStyleImg.height(); y++)
      for (quint8 x = 0; x < ghostStyleImg.width(); x++){
        QColor ghostColor = ghostStyleImg.pixelColor(x,y);
        ghostColor.setAlpha(127);
        ghostStyleImg.setPixelColor(x,y,ghostColor);
      }
    QPixmap ghostStylePx;
    ghostStylePx.convertFromImage(ghostStyleImg);
    foreach (QPoint p, ghostBlocks){
      if (p.y() >= 0) painter.drawPixmap(PFIELD_ORIGIN_X + cellWidth*p.x(),PFIELD_ORIGIN_Y + cellHeight*p.y(), cellWidth, cellHeight, ghostStylePx);
    }
    foreach (QPoint p, blocks){
      if (p.y() >= 0) painter.drawPixmap(PFIELD_ORIGIN_X + cellWidth*p.x(),PFIELD_ORIGIN_Y + cellHeight*p.y(), cellWidth, cellHeight, blockStyles[style]);
    }
    painter.end();
    mergeLayersToFrame();
}

bool TetrisScreen::isCellOccupied(const QPoint& p){
    return (p.x() < 0 || p.x() >= pfieldWidth || p.y() >= pfieldHeight) || (playField[p.y()] & (1 << (pfieldWidth - p.x() - 1)));
}

bool TetrisScreen::tryToMove(qint8 xOffset, qint8 yOffset){
    if (timeState == Paused) return false;
    auto blocks = activeBlock.getBlocks();
    for (auto &p : blocks){
        if (isCellOccupied({p.x() + xOffset, p.y() + yOffset})) return false;
    }
    activeBlock.moveBlock(xOffset,yOffset);
    return true;
}

void TetrisScreen::dropDown(){
    if(!tryToMove(0,1)) emit blockReachedObstacle();
    timerDropDownCnt++;
}

void TetrisScreen::timerLeftTimeoutHandler(){
  tryToMove(-1,0);
  timerLeftCnt++;
}
void TetrisScreen::timerRightTimeoutHandler(){
  tryToMove(1,0);
  timerRightCnt++;
}

bool TetrisScreen::tryToRotate(BlockInfo::BlockOrientation orient){
    if (timeState == Paused) return false;
    auto rotBlocks = activeBlock.getRotatedBlocks(orient);
    for (auto &p : rotBlocks){
        if (isCellOccupied(p)) return false;
    }
    activeBlock.rotateBlock(orient, rotBlocks);
    return true;
}

void TetrisScreen::setBlock(){
    if (timeState == Paused) return;
    quint8 highestPoint = pfieldHeight;
    activeBlockLayer.fill(Qt::transparent);
    drawBlock(foregroundLayer);
    auto blocks = activeBlock.getBlocks();
    for (auto &p : blocks){
      playField[p.y()] = playField[p.y()] | (1 << (pfieldWidth - p.x() - 1));
      if (p.y() + 1 < highestPoint) highestPoint = p.y() + 1;
    }
    checkForFilledLines();
    score += (pfieldHeight - highestPoint) * (1 + 0.15*(speed-1));
    blockCount[activeBlock.blockType()]++;
    updateStats();
    spawnBlock();
}

void TetrisScreen::clearFilledLines(QVector<quint8> fLineNums){
    for (auto i : fLineNums){
      QPixmap movedArea = foregroundLayer.copy(PFIELD_ORIGIN_X,PFIELD_ORIGIN_Y,pfieldWidth*cellWidth,i*cellHeight);
      painter.begin(&foregroundLayer);
      painter.setCompositionMode(QPainter::CompositionMode_Source);
      painter.fillRect(PFIELD_ORIGIN_X,PFIELD_ORIGIN_Y,movedArea.width(),movedArea.height(),Qt::transparent);
      painter.drawPixmap(PFIELD_ORIGIN_X,PFIELD_ORIGIN_Y+cellHeight,movedArea);
      painter.end();
      for (quint8 j = i; j > 0; j--)
        playField[j] = playField[j-1];
      playField[0] = 0;
    }
}

void TetrisScreen::checkForFilledLines(){
    QVector<quint8> fLineNums;
    for (quint8 l = 0; l < pfieldHeight; l++){
      if (playField[l] == filled) fLineNums << l;
    }
    lineCount += fLineNums.size();
    if (!fLineNums.empty()){
        score += 100*qPow(2,fLineNums.size()) * (1 + 0.15*(speed-1));
        emit linesFilled(fLineNums);
    }
}

void TetrisScreen::gameOver(){
    if (score > topScore){
      topScore = score;
      QFile scoreFile("topscore.bin");
      scoreFile.open(QIODevice::WriteOnly);
      QDataStream out(&scoreFile);
      out << topScore;
      scoreFile.close();
      isNewRecord = true;
    }else isNewRecord = false;
    gameOverLineY = pfieldHeight - 1;
    timerGameOverLine.start(60);
}

void TetrisScreen::drawGameOverLine(){
    if (gameState == GameOver){
      if (gameOverLineY < 0){
      timerGameOverLine.stop();
      return;
      }
      painter.begin(&frame);
      gameOverLine.clear();
      for (int x = 0; x < pfieldWidth; x++)
        gameOverLine << QPoint(x,gameOverLineY);
      foreach (QPoint p, gameOverLine)
        painter.drawPixmap(PFIELD_ORIGIN_X + cellWidth*p.x(),PFIELD_ORIGIN_Y + cellHeight*p.y(), cellWidth, cellHeight, blockStyles[BlockStyle((qrand() % 7) + BlockStyle1)]);
      painter.end();
      --gameOverLineY;
    }
}

void TetrisScreen::drawLogicalPlayfield(){
    painter.begin(&foregroundLayer);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(100,250,100,200,Qt::black);
    painter.setPen(Qt::white);
    for (quint8 i = 0; i < pfieldHeight; i++)
      for (qint8 j = pfieldWidth - 1; j >= 0; j--)
        painter.drawText(100 + 10*(pfieldWidth - j - 1),250 + 10*i, 10, 10, Qt::AlignCenter, (playField[i] & (1 << j) ? "1" : "0"));
    painter.end();
}

//
void TetrisScreen::paintGL(){
    painter.begin(this);
    painter.drawPixmap(0,0,width(),height(),frame);
    painter.end();
}
