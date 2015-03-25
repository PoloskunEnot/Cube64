#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class CPattern;

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    CPattern *pattern;
    QMenu *portMenu;
    bool okToContinue();
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void enableMenuBar();
    void disableMenuBar();
private slots:
    void checkPort();
    void newPtrn();
    void loadPtrn();
    void savePtrn();
};

#endif // MAINWINDOW_H
