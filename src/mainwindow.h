#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QVector>

#include <QtGui/QMainWindow>

namespace Ui
{
    class MainWindow;
}

#include "image.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();


private slots:
    void load();


private:
    void setup_connections();


private:
    Ui::MainWindow *ui;
    QVector<Image> images;
};

#endif // MAINWINDOW_H
