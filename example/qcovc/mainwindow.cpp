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

#include "voxelcolorer.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow),
    matrix_of_camera_calibration(4, 4)
{
    ui->setupUi(this);

    setup_ui();

    setup_connections();

    setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

    vc = new VoxelColorer();

    matrix_of_camera_calibration(0, 0) = 50;
    matrix_of_camera_calibration(0, 1) = 0;
    matrix_of_camera_calibration(0, 2) = 256;
    matrix_of_camera_calibration(0, 3) = 0;
    matrix_of_camera_calibration(1, 0) = 0;
    matrix_of_camera_calibration(1, 1) = 50;
    matrix_of_camera_calibration(1, 2) = 256;
    matrix_of_camera_calibration(1, 3) = 0;
    matrix_of_camera_calibration(2, 0) = 0;
    matrix_of_camera_calibration(2, 1) = 0;
    matrix_of_camera_calibration(2, 2) = 1;
    matrix_of_camera_calibration(2, 3) = 0;
}


MainWindow::~MainWindow()
{
    delete ui;

    delete vc;

    images.clear();
}

///////////////////////////////////////////////////////////////////////////////
//! Add new image
///////////////////////////////////////////////////////////////////////////////
void MainWindow::add_image()
{
    QString image_file_name = QFileDialog::getOpenFileName(this,
                                                           tr("Select meta file."),
                                                           "",
                                                           tr("Images (*.png *.jpg)"));
    if (image_file_name.isEmpty())
        return;

    // create image
    ImageInfo image(image_file_name);
    if (image.is_valid())
    {
        images.append(image);
        // add image to the preview widget
        image_preview_model->add_image(image.get_image());
    }
}

///////////////////////////////////////////////////////////////////////////////
//! Setting up new element of image calibration matrix
///////////////////////////////////////////////////////////////////////////////
void MainWindow::element_of_matrix_of_calibration_changed(int row, int column)
{
    if (row == -1 || column == -1)
        return;

    bool ok;
    float value = table_widget->item(row, column)->data(Qt::EditRole).toFloat(&ok);
    if (ok)
        images[image_preview_list->currentIndex().row()].set_element_of_matrif_of_calibration(row, column, value);
}


///////////////////////////////////////////////////////////////////////////////
//! Setting up selected image and it's bounding rectangle to image view
//!
//! @param index Image index in preview window
///////////////////////////////////////////////////////////////////////////////
void MainWindow::image_selected(QModelIndex index)
{
    int image_index = index.row();
    const QImage &image = images[image_index].get_image();

    // add image to the scene
    image_scene->set_image(image);

    // add bounding rectangle to the scene
    image_scene->set_rectangle(images[image_index].get_bounding_rectangle());

    // set calibration matrix
    const matrix<float> & calibration_matrix = images[image_index].get_matrix_of_calibration();

    for (int r = 0; r < calibration_matrix.RowNo(); ++r)
        for (int c = 0; c < calibration_matrix.ColNo(); ++c)
            table_widget->setItem(r, c, new QTableWidgetItem(QString("%1").arg(calibration_matrix(r, c))));
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

    QTextStream in(&file);

    size_t image_number;
    in >> image_number;

    // loading matrix of calibration
    // TODO: CAUTION: type-specific code
    // TODO: code duplication with "from line 159"
    for (size_t r=0; r < matrix_of_camera_calibration.RowNo(); ++r)
        for (size_t c=0; c < matrix_of_camera_calibration.ColNo(); ++c)
        {
            float x;
            in >> x;
            matrix_of_camera_calibration(r,c) = x;
        }

    image_preview_model->clear();

    images.clear();
    images.reserve(image_number);

    // load images names with bounding sqares from meta file
    for (size_t i = 0; i < image_number; ++i)
    {
        // loading image file name
        QString image_file_name;
        in >> image_file_name;

        QRectF bounding_rectangle;
        float temp[4];
        in >> temp[0];
        in >> temp[1];
        in >> temp[2];
        in >> temp[3];
        bounding_rectangle.setLeft(temp[0]);
        bounding_rectangle.setTop(temp[1]);
        bounding_rectangle.setRight(temp[2]);
        bounding_rectangle.setBottom(temp[3]);

        // loading matrix of calibration
        matrix<float> matrix_of_calibration(4, 4);
        // TODO: CAUTION: type-specific code
        for (size_t r=0; r < matrix_of_calibration.RowNo(); ++r)
            for (size_t c=0; c < matrix_of_calibration.ColNo(); ++c)
            {
                float x;
                in >> x;
                matrix_of_calibration(r,c) = x;
            }

        // create image
        ImageInfo image(image_file_name, matrix_of_calibration, bounding_rectangle);
        if (image.is_valid())
        {
            images.push_back(image);
            // add image to the preview widget
            image_preview_model->add_image(image.get_image());
        }
    }

    //close input file
    file.close();
}

///////////////////////////////////////////////////////////////////////////////
//! Set new rectangle
///////////////////////////////////////////////////////////////////////////////
void MainWindow::rectangle_changed(QRectF rectangle)
{
    images[image_preview_list->currentIndex().row()].set_bounding_rectangle(rectangle);
}

///////////////////////////////////////////////////////////////////////////////
//! Run voxel colorer algorithm
///////////////////////////////////////////////////////////////////////////////
void MainWindow::run()
{
//    try
//    {
    if (!vc->prepare())
        return;

    vc->set_number_of_images(images.size());

    float camera_calibration_matrix[16];
    for (size_t r = 0; r < matrix_of_camera_calibration.RowNo(); ++r)
        for (size_t c = 0; c < matrix_of_camera_calibration.ColNo(); ++c)
            camera_calibration_matrix[r*4 + c] = matrix_of_camera_calibration(r, c);

    vc->set_camera_calibration_matrix(camera_calibration_matrix);

    for (size_t i = 0; i <  images.size(); ++i)
    {
        float matrix[16];
        for (size_t r = 0; r < images[i].get_matrix_of_calibration().RowNo(); ++r)
            for (size_t c = 0; c < images[i].get_matrix_of_calibration().ColNo(); ++c)
                matrix[r*4 + c] = images[i].get_matrix_of_calibration()(r, c);

        vc->add_image(images[i].get_image().bits(),
                      images[i].get_image().width(),
                      images[i].get_image().height(),
                      matrix);
    }

    vc->set_resulting_voxel_cube_dimensions(32, 32, 32);

    vc->build_voxel_model();
    //    }
//    catch (...)
//    {
//    }
}

///////////////////////////////////////////////////////////////////////////////
//! Save all information in meta file.
///////////////////////////////////////////////////////////////////////////////
void MainWindow::save_metafile()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Select meta file."));

    if (filename.isEmpty())
        return;

    QFile file(filename);
    file.open(QIODevice::WriteOnly);
    if (!file.isOpen())
        return;

    QTextStream out(&file);

    out << images.size();

    out << "\n";

    // save matrix of camera calibration
    // TODO: CAUTION: type-specific code
    for (size_t r=0; r < matrix_of_camera_calibration.RowNo(); ++r)
    {
        for (size_t c=0; c < matrix_of_camera_calibration.ColNo(); ++c)
        {
            out << matrix_of_camera_calibration(r,c) << " ";
        }
        out << "\n";
    }

    // saving images names with bounding sqares from meta file
    for (size_t i = 0; i < images.size(); ++i)
    {
        // save image file name
        out << images[i].get_path_to_image();

        out << "\n";

        out << images[i].get_bounding_rectangle().left() << " ";
        out << images[i].get_bounding_rectangle().top() << " ";
        out << images[i].get_bounding_rectangle().right() << " ";
        out << images[i].get_bounding_rectangle().bottom() << " ";

        out << "\n";

        // saving matrix of calibration
        // TODO: CAUTION: type-specific code
        for (size_t r=0; r < images[i].get_matrix_of_calibration().RowNo(); ++r)
        {
            for (size_t c=0; c < images[i].get_matrix_of_calibration().ColNo(); ++c)
                out << images[i].get_matrix_of_calibration()(r,c) << " ";

            out << "\n";
        }
    }

    //close input file
    file.close();
}


void MainWindow::save_voxel_model()
{
    std::vector<unsigned char> model = vc->get_voxel_model();

    QString filename = QFileDialog::getSaveFileName(this, tr("Select meta file."));

    if (filename.isEmpty())
        return;

    QFile file(filename);
    file.open(QIODevice::WriteOnly);
    if (!file.isOpen())
        return;

    QDataStream out(&file);

    for (size_t i = 0; i < model.size(); ++i)
        out << model[i];

    file.close();
}

///////////////////////////////////////////////////////////////////////////////
//! Setup qt connections
///////////////////////////////////////////////////////////////////////////////
void MainWindow::setup_connections()
{
    connect(ui->actionAdd_image, SIGNAL(triggered()), this, SLOT(add_image()));
    connect(ui->actionLoad_metafile, SIGNAL(triggered()), this, SLOT(load_metafile()));
    connect(ui->actionSave_metafile, SIGNAL(triggered()), this, SLOT(save_metafile()));
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));

    connect(ui->actionRun, SIGNAL(triggered()), this, SLOT(run()));
    connect(ui->actionSave_voxel_model, SIGNAL(triggered()), this, SLOT(save_voxel_model()));

    connect(image_preview_list, SIGNAL(clicked(QModelIndex)), this, SLOT(image_selected(QModelIndex)));
    connect(image_scene, SIGNAL(rectangle_changed(QRectF)) , this, SLOT(rectangle_changed(QRectF)));
    connect(table_widget, SIGNAL(cellChanged(int,int)), this, SLOT(element_of_matrix_of_calibration_changed(int,int)));
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
    table_widget->setColumnCount(4);
    table_widget->setColumnWidth(0, 45);
    table_widget->setColumnWidth(1, 45);
    table_widget->setColumnWidth(2, 45);
    table_widget->setColumnWidth(3, 45);
    table_widget->setRowCount(4);
    table_widget->horizontalHeader()->hide();
    table_widget->verticalHeader()->hide();

    //create images view (main widget)
    image_view = new QGraphicsView(this);
    image_view->setMinimumSize(600, 599);
    image_view->setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);

    image_scene = new ImageScene(this);
    //TODO: magic numbers
    image_scene->setSceneRect(0.0, 0.0, 645.0, 595.0);
    image_scene->set_rectangle(QRectF(0.0, 0.0, 0.0, 0.0));
    image_view->setScene(image_scene);

    verticalLayout->addWidget(image_preview_list);
    verticalLayout->addWidget(table_widget);

    ui->horizontalLayout->addLayout(verticalLayout);
    ui->horizontalLayout->addWidget(image_view);
}
