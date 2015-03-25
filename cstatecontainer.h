#ifndef CSTATECONTAINER_H
#define CSTATECONTAINER_H

#include <QWidget>

class CCubeState;

class CStateContainer : public QWidget
{
    Q_OBJECT
public:
    CCubeState *cubeState;
    explicit CStateContainer(QWidget *parent = 0);
    ~CStateContainer();

signals:

public slots:
};

#endif // CSTATECONTAINER_H
