#include <QtGui>

#include <fstream>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setup_connections();
}

MainWindow::~MainWindow()
{
    delete ui;

    images.clear();
}

void MainWindow::add_image()
{
}

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

    if (!images.isEmpty())
        images.clear();
    images.resize(image_number);

    // load images names with bounding sqares from meta file
    for (size_t i = 0; i < image_number; ++i)
    {
        // loading image file name
        QString image_file_name;
        in >> image_file_name;

        QPoint bounding_square;
        in >> bounding_square;

        // loading image bounding sqare
        matrix<int> matrix_of_calibration(2,2);
        // CAUTION: type-specific code
        for (size_t i=0; i < matrix_of_calibration.RowNo(); i++)
            for (size_t j=0; j < matrix_of_calibration.ColNo(); j++)
            {
                int x;
                in >> x;
                matrix_of_calibration(i,j) = x;
            }

        // create image
        Image image(image_file_name, matrix_of_calibration, bounding_square);
        if (image.isValid())
            images.append(image);
    }

    //close input file
    file.close();
}

void MainWindow::save_metafile()
{
}

void MainWindow::setup_connections()
{
    connect(ui->actionLoad_metafile, SIGNAL(triggered()), this, SLOT(load_metafile()));
}
