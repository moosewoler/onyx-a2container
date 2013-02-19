#ifndef _GAME_WIDGET_H_
#define _GAME_WIDGET_H_

#include <QWidget>
#include <Qt/qstack.h>

#include "board.h"

// 棋盘表现
class GameWidget : public QWidget
{
    Q_OBJECT

private:
    int currentX;
    int currentY;
    int wygral;
    bool flashScreen;
    bool isEnd;
    QString fontName;
    QStack<int> ruchy;
    Board board;

private:
    bool findFont(QString name);

public:
    GameWidget(QWidget *parent = 0);

    void refreshScreen();
    void newGame();

Q_SIGNALS:
    void popupMenu();

public Q_SLOTS:
    void onMouseLongPress(QPoint, QSize);

private:
    int cellSize();
    int getSize() { return board.size() * cellSize(); }
    int fromTop() { return ((((height() - getSize()) / 2)>200 || width()>height()) ?((height() - getSize()) / 2) :150); }
    int fromLeft();
    int cellTop(int y) { return fromTop() + y * cellSize(); }
    int cellLeft(int x) { return fromLeft() + x * cellSize(); }
    void paintEvent(QPaintEvent *e);
    void move();
    void undo();
    bool event(QEvent * event);
    void keyReleaseEvent(QKeyEvent *ke);
    void mousePressEvent(QMouseEvent *event);
};

#endif // _GAME_WIDGET_H_
