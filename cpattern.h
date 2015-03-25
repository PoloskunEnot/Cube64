#ifndef CPATTERN_H
#define CPATTERN_H

#include <QWidget>

class CCubeView;
class CCubeState;
class QPushButton;
class QSpinBox;
class QLabel;
class QTimer;
class QSerialPort;

class CPattern : public QWidget
{
    Q_OBJECT
public:
    QVector <CCubeState*> states;
    CCubeView *cubeView;
    int nState, showDur;
    QPushButton *prevBtn, *nextBtn, *newBtn, *delBtn, *runBtn, *stopBtn;
    QSpinBox *spinBox;
    QLabel *counter;
    QTimer *timer;
    QSerialPort *comPort;
    bool flag_mod;
    explicit CPattern(QWidget *parent = 0);
    ~CPattern();
    bool isModified();
    void newPattern();
    void loadPattern(QString fileName);
    void savePattern(QString fileName);
private:
    enum { MagicNumber = 0x7F51C883 };
signals:
    void showOn();
    void showOff();
public slots:
    void sendMessage();
    void setModified();
private slots:
    void setPrevState();
    void setNextState();
    void deleteState();
    void newState();
    void runPattern();
    void stopPattern();
    void showPattern();
};

#endif // CPATTERN_H
