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

#include <QtGui>

#include <QGraphicsView>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "imagepreview.h"
#include "imagescene.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setup_ui();

    setup_connections();

    setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
}


MainWindow::~MainWindow()
{
    delete ui;

    images.clear();
}


void MainWindow::add_image()
{
}


///////////////////////////////////////////////////////////////////////////////
//! Setting up selected image and it's bounding rectangle to image view
//!
//! @param index Image index in preview window
///////////////////////////////////////////////////////////////////////////////
void MainWindow::image_selected(QModelIndex index)
{
    const QImage &image = images[index.row()].get_image();

    image_scene->set_image(images[index.row()].get_image());
    // set scene size to image size
    image_view->setSceneRect(0.0,
                             0.0,
                             image.width(),
                             image.height());
    image_scene->set_rectangle(images[index.row()].get_bounding_rectangle());

    // scale image view size
    if (image.width() > image.height())
        image_view->scale(image.width()/645.0, image.width()/645.0);
    else
        image_view->scale(image.height()/595.0, image.height()/595.0);

    const matrix<int> & calibration_matrix = images[index.row()].get_matrix_of_calibration();

    for (int i = 0; i < calibration_matrix.RowNo(); ++i)
        for (int j = 0; j < calibration_matrix.ColNo(); ++j)
            table_widget->setItem(i, j, new QTableWidgetItem(QString("%1").arg(calibration_matrix(i, j))));
}


///////////////////////////////////////////////////////////////////////////////
//! Load meta file and all needed files from meta file definitions
///////////////////////////////////////////////////////////////////////////////
void MainWindow::load_metafile()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Select meta file."));

    if (filename.isEmpty())
        return;

    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    if (!file.isOpen())
        return;

    QDataStream in(&file);

    size_t image_number;
    in >> image_number;

    image_preview_model->clear();

    images.clear();
    images.resize(image_number);

    // load images names with bounding sqares from meta file
    for (size_t i = 0; i < image_number; ++i)
    {
        // loading image file name
        QString image_file_name;
        in >> image_file_name;

        QRectF bounding_square;
        in >> bounding_square;

        // loading matrix of calibration
        matrix<int> matrix_of_calibration(3,3);
        // CAUTION: type-specific code
        for (size_t i=0; i < matrix_of_calibration.RowNo(); i++)
            for (size_t j=0; j < matrix_of_calibration.ColNo(); j++)
            {
                int x;
                in >> x;
                matrix_of_calibration(i,j) = x;
            }

        // create image
        ImageInfo image(image_file_name, matrix_of_calibration, bounding_square);
        if (image.is_valid())
        {
            images.append(image);
            // add image to the preview widget
            image_preview_model->add_image(image.get_image());
        }
    }

    //close input file
    file.close();
}


///////////////////////////////////////////////////////////////////////////////
//! TODO May be one day it will be saving function
///////////////////////////////////////////////////////////////////////////////
void MainWindow::save_metafile()
{
}


///////////////////////////////////////////////////////////////////////////////
//! Setup qt connections
///////////////////////////////////////////////////////////////////////////////
void MainWindow::setup_connections()
{
    connect(ui->actionLoad_metafile, SIGNAL(triggered()), this, SLOT(load_metafile()));
    connect(image_preview_list, SIGNAL(clicked(QModelIndex)), this, SLOT(image_selected(QModelIndex)));
}


///////////////////////////////////////////////////////////////////////////////
//! Setup ui
///////////////////////////////////////////////////////////////////////////////
void MainWindow::setup_ui()
{
    //create left vertical layout for images preview and calibration matrix
    QVBoxLayout * verticalLayout = new QVBoxLayout(this);

    //create images preview
    image_preview_list = new QListView;
    image_preview_list->setViewMode(QListView::IconMode);
    image_preview_list->setIconSize(QSize(90, 90));
    image_preview_list->setGridSize(QSize(100, 100));
    image_preview_list->setSpacing(10);
    image_preview_list->setMovement(QListView::Snap);

    image_preview_model = new ImagePreview(this);
    image_preview_list->setModel(image_preview_model);

    //create table widget for calibration matrix
    table_widget = new QTableWidget(this);
    table_widget->setMaximumHeight(100);
    table_widget->setColumnCount(3);
    table_widget->setColumnWidth(0, 45);
    table_widget->setColumnWidth(1, 45);
    table_widget->setColumnWidth(2, 45);
    table_widget->setRowCount(3);
    table_widget->horizontalHeader()->hide();
    table_widget->verticalHeader()->hide();

    //create images view (main widget)
    image_view = new QGraphicsView(this);
    image_view->setMinimumSize(650, 599);
    image_view->setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);

    image_scene = new ImageScene(this);
    image_scene->setSceneRect(0.0, 0.0, 645.0, 595.0);
    image_scene->set_rectangle(QRectF(0.0, 0.0, 0.0, 0.0));
    image_view->setScene(image_scene);

    verticalLayout->addWidget(image_preview_list);
    verticalLayout->addWidget(table_widget);

    ui->horizontalLayout->addLayout(verticalLayout);
    ui->horizontalLayout->addWidget(image_view);
}
