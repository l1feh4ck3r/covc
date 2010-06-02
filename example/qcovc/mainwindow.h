/*
 * Copyright (c) 2010 Alexey 'l1feh4ck3r' Antonov
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModelIndex>
#include <QTableWidget>
#include <QVector>
#include <QPointer>

class QListView;
class QGraphicsView;

class ImagePreview;
class ImageScene;

namespace Ui
{
    class MainWindow;
}

class VoxelColorer;

#include "imageinfo.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();


private slots:
    void add_image();
    void element_of_matrix_of_calibration_changed(int row, int column);
    void image_selected(QModelIndex index);
    void load_metafile();
    void rectangle_changed(QRectF rectangle);
    void run();
    void save_metafile();
    void save_voxel_model();


private:
    void setup_connections();
    void setup_ui();


private:
    Ui::MainWindow  *ui;
    QListView       *image_preview_list;
    ImagePreview    *image_preview_model;
    ImageScene      *image_scene;
    QGraphicsView   *image_view;
    QTableWidget    *table_widget;

    matrix<float>   matrix_of_camera_calibration;
    QVector<ImageInfo>  images;

    QModelIndex last_selected_image;

    VoxelColorer * vc;
};

#endif // MAINWINDOW_H
