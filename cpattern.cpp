#include "cpattern.h"
#include "ccubestate.h"
#include "ccubeview.h"
#include "cstatecontainer.h"
#include <QPushButton>
#include <QSpinBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QAction>
#include <QtSerialPort/QSerialPort>
#include <QMessageBox>
#include <QFile>
#include <QDataStream>
#include <QApplication>

CPattern::CPattern(QWidget *parent) : QWidget(parent)
{
    flag_mod = false;
    cubeView = new CCubeView;
    CCubeState *tempstate = new CCubeState;
    states.append(tempstate);
    nState = 0;
    cubeView->cs = states[nState];
    prevBtn = new QPushButton("<");
    nextBtn = new QPushButton(">");
    newBtn = new QPushButton("New");
    delBtn = new QPushButton("Delete");
    delBtn->setDisabled(true);
    runBtn = new QPushButton("Run");
    stopBtn = new QPushButton("Stop");
    stopBtn->setDisabled(true);
    spinBox = new QSpinBox;
    spinBox->setMinimum(100);
    spinBox->setMaximum(10000);
    spinBox->setSingleStep(100);
    spinBox->setMaximumWidth(60);
    spinBox->setMinimumWidth(60);
    spinBox->setValue(cubeView->cs->duration);
    counter = new QLabel("State "+QString::number(nState+1)+"/"+QString::number(states.size()));
    QLabel *label = new QLabel("ms");
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(counter);
    hLayout->addWidget(spinBox);
    hLayout->addWidget(label);
    hLayout->addWidget(prevBtn);
    hLayout->addWidget(nextBtn);
    hLayout->addWidget(newBtn);
    hLayout->addWidget(delBtn);
    hLayout->addStretch();
    hLayout->addWidget(runBtn);
    hLayout->addWidget(stopBtn);
    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->addWidget(cubeView);
    vLayout->addLayout(hLayout);
    setLayout(vLayout);
    connect(prevBtn, SIGNAL(clicked()), this, SLOT(setPrevState()));
    connect(nextBtn, SIGNAL(clicked()), this, SLOT(setNextState()));
    connect(newBtn,  SIGNAL(clicked()), this, SLOT(newState()));
    connect(delBtn,  SIGNAL(clicked()), this, SLOT(deleteState()));
    connect(runBtn,  SIGNAL(clicked()), this, SLOT(runPattern()));
    connect(stopBtn, SIGNAL(clicked()), this, SLOT(stopPattern()));
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(showPattern()));

    comPort = new QSerialPort;
    connect(cubeView, SIGNAL(ledChanged()), this, SLOT(sendMessage()));
    connect(cubeView, SIGNAL(ledChanged()), this, SLOT(setModified()));
}

CPattern::~CPattern()
{

}

bool CPattern::isModified()
{
    return flag_mod;
}

void CPattern::sendMessage()
{
    if (comPort->isOpen())  comPort->write(states[nState]->message);
    else {
        QMessageBox::warning(this, tr("Warning"),
                             tr("Select COM Port."),
                             QMessageBox::Ok );
    }
}

void CPattern::newPattern()
{
    foreach (CCubeState* st, states) delete st;
    states.clear();
    CCubeState *tempstate = new CCubeState;
    states.append(tempstate);
    nState = 0;
    spinBox->setValue(tempstate->duration);
    setNextState();
    flag_mod = false;
}

void CPattern::loadPattern(QString fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("Pattern"),
             tr("Cannot read file.") );
        return;
    }
    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_4_3);
    quint32 magic;
    in >> magic;
    if (magic != MagicNumber) {
        QMessageBox::warning(this, tr("Pattern"),
            tr("The file is not a *Cube64* file."));
        return;
    }
    foreach (CCubeState* st, states) delete st;
    states.clear();
    QApplication::setOverrideCursor(Qt::WaitCursor);
    quint16 dur;
    quint8 led;
    while (!in.atEnd()) {
        CCubeState *tempstate = new CCubeState;
        in >> dur;
        tempstate->duration = int(dur);
        for (int i = 0; i < CUBE_SZ+1; i++){
            in >> led;
            tempstate->message[i] = char(led);
        }
        tempstate->msg2state();
        states.append(tempstate);
    }
    QApplication::restoreOverrideCursor();
    nState = 0;
    cubeView->cs = states[nState];
    spinBox->setValue(cubeView->cs->duration);
    cubeView->update();
    counter->setText("State "+QString::number(nState+1)+"/"+QString::number(states.size()));
    sendMessage();
    flag_mod = false;
}


void CPattern::savePattern(QString fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, tr("Pattern"),
                tr("Cannot write file.") );
        return;
    }
    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_4_3);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << quint32(MagicNumber);
    foreach (CCubeState *st, states) {
        out<<quint16(st->duration);
        for (int i = 0; i < CUBE_SZ+1; i++)
            out<<quint8(st->message[i]);
    }
    QApplication::restoreOverrideCursor();
    flag_mod = false;
}


void CPattern::setModified()
{
    flag_mod = true;
}

void CPattern::setPrevState()
{
    states[nState]->duration = spinBox->value();
    nState--;
    if (nState < 0) nState = states.size()-1;
    cubeView->cs = states[nState];
    spinBox->setValue(cubeView->cs->duration);
    cubeView->update();
    counter->setText("State "+QString::number(nState+1)+"/"+QString::number(states.size()));
    sendMessage();
}

void CPattern::setNextState()
{
    states[nState]->duration = spinBox->value();
    nState++;
    if (nState == states.size()) nState = 0;
    cubeView->cs = states[nState];
    spinBox->setValue(cubeView->cs->duration);
    cubeView->update();
    counter->setText("State "+QString::number(nState+1)+"/"+QString::number(states.size()));
    sendMessage();
}

void CPattern::deleteState()
{
    delete states[nState];
    states.remove(nState);
    if (nState == states.size()) nState--;
    if (states.size() == 1) delBtn->setDisabled(true);
    cubeView->cs = states[nState];
    spinBox->setValue(cubeView->cs->duration);
    cubeView->update();
    counter->setText("State "+QString::number(nState+1)+"/"+QString::number(states.size()));
    sendMessage();
    flag_mod = true;
}

void CPattern::newState()
{
    CCubeState *tempstate = new CCubeState;
    states.insert(nState+1, tempstate);
    delBtn->setDisabled(false);
    setNextState();
    flag_mod = true;
}

void CPattern::runPattern()
{
    prevBtn->setDisabled(true);
    nextBtn->setDisabled(true);
    delBtn->setDisabled(true);
    runBtn->setDisabled(true);
    stopBtn->setDisabled(false);
    spinBox->setReadOnly(true);
    showDur = 0;
    timer->start(100);
    emit showOn();
}

void CPattern::stopPattern()
{
    prevBtn->setDisabled(false);
    nextBtn->setDisabled(false);
    if (states.size() != 1) delBtn->setDisabled(false);
    runBtn->setDisabled(false);
    stopBtn->setDisabled(true);
    spinBox->setReadOnly(false);
    timer->stop();
    sendMessage();
    emit showOff();
}

void CPattern::showPattern()
{
    showDur -= 100;
    if (showDur <= 0) {
        setNextState();
        showDur = states[nState]->duration;
    }
}

