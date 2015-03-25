#ifndef CCUBEVIEW_H
#define CCUBEVIEW_H

#include <QWidget>

class CCubeState;

class CCubeView : public QWidget
{
    Q_OBJECT
protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *event);
private:
    void draw(QPainter *painter);
public:
    CCubeState *cs;
    CCubeView(QWidget *parent = 0);
    ~CCubeView();
signals:
    void ledChanged();
public slots:
};

#endif // CCUBEVIEW_H
