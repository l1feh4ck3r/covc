#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QVector>
#include <QPointer>

#include <QtGui/QMainWindow>

class QListView;

class ImagePreview;
class ImageView;

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
    void add_image();
    void load_metafile();
    void save_metafile();


private:
    void setup_connections();
    void setup_ui();


private:
    Ui::MainWindow  *ui;
    QListView       *image_preview_list;
    ImagePreview    *image_preview_model;
    ImageView       *image_view;

    QVector<Image>  images;
};

#endif // MAINWINDOW_H
