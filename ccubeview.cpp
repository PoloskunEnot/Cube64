#include "ccubeview.h"
#include "ccubestate.h"
#include <QPainter>
#include <QPen>
#include <QMouseEvent>


CCubeView::CCubeView(QWidget *parent) : QWidget(parent)
{
    setMinimumSize(400,400);
}

CCubeView::~CCubeView()
{

}

void CCubeView::paintEvent(QPaintEvent * )
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    int side = qMin(width(), height());
    painter.setViewport((width() - side) / 2, (height() - side) / 2, side, side);
    painter.setWindow(0, 0, 200, 200);
    draw(&painter);
}

void CCubeView::draw(QPainter *painter)
{
    // вдоль i - направление по вертикали
    // вдоль j - по диагонали
    // вдоль k - по горизонтали
    // размер окна вывода в логических координатах: (0,0) - (200,200)
    // нижний левый угол куба: (20,170)
    // размер ячейки куба: 40, размер куба: 3*40 = 120
    // на слое отступ по x: 16, отступ по y: 10
    painter->setPen(Qt::DashLine); // рисуем пунктирные вертикали вдоль i (8штук)
    for (int k = 0; k < EDGE_SZ; k++){
        painter->drawLine(20+k*40,170,20+k*40,50);
        painter->drawLine(140+k*16,170-k*10,140+k*16,50-k*10);
    }
    for (int i = 0; i < EDGE_SZ; i++){
        for (int j = 0; j < EDGE_SZ; j++){
            painter->setPen(Qt::DashLine); // рисуем линию слоя вдоль k
            painter->drawLine(20+j*16,170-i*40-j*10,140+j*16,170-i*40-j*10);
            for (int k = 0; k < EDGE_SZ; k++){
                if (j == 0) { // рисуем линию слоя вдоль j
                    painter->setPen(Qt::DashLine);
                    painter->drawLine(20+k*40,170-i*40,20+k*40+3*16,140-i*40);
                }
                painter->setPen(Qt::black);
                if (cs->state[i][j][k] == 0)  painter->setBrush(Qt::darkGray);
                if (cs->state[i][j][k] == 1)  painter->setBrush(Qt::green);
                painter->drawEllipse(QPointF(20+k*40+j*16,170-i*40-j*10),3,3);
            }
        }
    }

}

void CCubeView::mousePressEvent(QMouseEvent *event)
{
    QPointF pnt = event->pos();
    // переход в логическую систему координат painter'a
    int side = qMin(width(), height());
    qreal x = 200.0 / side * (pnt.x() - (width()-side)/2) ;
    qreal y = 200.0 / side * (pnt.y() - (height()-side)/2);

    for (int i = 0; i < EDGE_SZ; i++){
        for (int j = 0; j < EDGE_SZ; j++){
            for (int k = 0; k < EDGE_SZ; k++){
                qreal xx = 20+k*40+j*16 - x;
                qreal yy = 170-i*40-j*10 - y;
                if (xx*xx + yy*yy <= 25.){
                    if (cs->state[i][j][k] == 0) cs->state[i][j][k] = 1;
                    else cs->state[i][j][k] = 0;
                    cs->state2msg();
                    emit ledChanged();
                    update();
                    return;
                }
            }
        }
    }
}
