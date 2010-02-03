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

void MainWindow::image_selected(QModelIndex index)
{
    image_scene->set_image(images[index.row()].get_image());
    image_scene->set_rectangle(images[index.row()].get_bounding_rectangle());
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

    image_preview_model->clear();

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

void MainWindow::save_metafile()
{
}

void MainWindow::setup_connections()
{
    connect(ui->actionLoad_metafile, SIGNAL(triggered()), this, SLOT(load_metafile()));
    connect(image_preview_list, SIGNAL(clicked(QModelIndex)), this, SLOT(image_selected(QModelIndex)));
}

void MainWindow::setup_ui()
{
    image_preview_list = new QListView;
    image_preview_list->setViewMode(QListView::IconMode);
    image_preview_list->setIconSize(QSize(90, 90));
    image_preview_list->setGridSize(QSize(100, 100));
    image_preview_list->setSpacing(10);
    image_preview_list->setMovement(QListView::Snap);

    image_preview_model = new ImagePreview(this);
    image_preview_list->setModel(image_preview_model);

    image_view = new QGraphicsView(this);
    image_view->setMinimumSize(650, 599);
    image_view->setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);

    image_scene = new ImageScene(this);
    image_scene->setSceneRect(0.0, 0.0, 645.0, 595.0);
    image_scene->set_rectangle(QRectF(0.0, 0.0, 0.0, 0.0));
    image_view->setScene(image_scene);

    ui->horizontalLayout->addWidget(image_preview_list);
    ui->horizontalLayout->addWidget(image_view);
}
