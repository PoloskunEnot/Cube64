#include "mainwindow.h"
#include "cpattern.h"
#include <QAction>
#include <QMenuBar>
#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>
#include <QMessageBox>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    pattern = new CPattern;
    connect(pattern, SIGNAL(showOn()), this, SLOT(disableMenuBar()));
    connect(pattern, SIGNAL(showOff()), this, SLOT(enableMenuBar()));

    QAction *newAction = new QAction(tr("&New"), this);
    newAction->setShortcut(QKeySequence::New);
    newAction->setStatusTip(tr("Create new pattern"));
    connect(newAction, SIGNAL(triggered()), this, SLOT(newPtrn()) );

    QAction *loadAction = new QAction(tr("&Load"), this);
    loadAction->setShortcut(tr("Ctrl+L"));
    loadAction->setStatusTip(tr("Load pattern"));
    connect(loadAction, SIGNAL(triggered()), this, SLOT(loadPtrn()) );

    QAction *saveAction = new QAction(tr("&Save"), this);
    saveAction->setShortcut(QKeySequence::Save);
    saveAction->setStatusTip(tr("Save pattern"));
    connect(saveAction, SIGNAL(triggered()), this, SLOT(savePtrn()) );

    QAction *exitAction = new QAction(tr("&Exit"), this);
    exitAction->setShortcut(tr("Ctrl+Q"));
    exitAction->setStatusTip(tr("Exit the application"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()) );

    QMenu *patternMenu = menuBar()->addMenu(tr("&Pattern"));
    patternMenu->addAction(newAction);
    patternMenu->addSeparator();
    patternMenu->addAction(loadAction);
    patternMenu->addAction(saveAction);
    patternMenu->addSeparator();
    patternMenu->addAction(exitAction);

    portMenu = menuBar()->addMenu(tr("&Port"));
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        QAction *port = new QAction(info.portName(),this);
        port->setCheckable(true);
        connect(port, SIGNAL(triggered()), this, SLOT(checkPort()));
        portMenu->addAction(port);
    }

    setCentralWidget(pattern);
}

MainWindow::~MainWindow()
{

}

void MainWindow::enableMenuBar()
{
    menuBar()->setEnabled(true);
}

void MainWindow::disableMenuBar()
{
    menuBar()->setEnabled(false);
}

void MainWindow::checkPort()
{
    foreach(QAction *act, portMenu->actions()){
        act->setChecked(false);
    }
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        if (pattern->comPort->isOpen()) pattern->comPort->close();
        pattern->comPort->setPortName(action->text());
        pattern->comPort->setBaudRate(9600);
        pattern->comPort->open(QIODevice::WriteOnly);
        if (pattern->comPort->isOpen()) {
            pattern->sendMessage();
            action->setChecked(true);
        }
        else {
            QMessageBox::warning(this, tr("Set COM Port"),
                        tr("The port couldn't be opened (busy?).\n"
                            "Select another one."),
                            QMessageBox::Ok );
        }
    }
}


void MainWindow::newPtrn()
{
    if ( okToContinue() ) pattern->newPattern();
}

void MainWindow::loadPtrn()
{
    if ( okToContinue() ) {
        QString fileName = QFileDialog::getOpenFileName(this,
                tr("Load Pattern"),".",
                tr("Cube64 files (*.c64)"));
        if ( !fileName.isEmpty() )  pattern->loadPattern(fileName);
    }
}

void MainWindow::savePtrn()
{
    QString fileName = QFileDialog::getSaveFileName(this,
            tr("Save Pattern"), ".",
            tr("Cube64 files (*.c64)"));
    if (!fileName.isEmpty()) pattern->savePattern(fileName);
}

bool MainWindow::okToContinue()
{
    if (pattern->isModified()) {
        int r = QMessageBox::warning(this, tr("Pattern"),
                    tr("The pattern has been modified.\n"
                        "Do you want to save your changes?"),
                        QMessageBox::Yes | QMessageBox::No|
                    QMessageBox::Cancel);
        if (r == QMessageBox::Yes) {
             savePtrn();
             return true;
        }
        else if (r == QMessageBox::Cancel) {
            return false;
        }
    }
    return true;
}

