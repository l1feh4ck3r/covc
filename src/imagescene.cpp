#include "imagescene.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsRectItem>

ImageScene::ImageScene(QObject *parent)
        :QGraphicsScene(parent), rectangle(NULL)
{
}

void ImageScene::mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    qDebug("press : x = %f | y = %f", mouseEvent->scenePos().x(), mouseEvent->scenePos().y());

    if (mouseEvent->button() != Qt::LeftButton)
    {
        QGraphicsScene::mousePressEvent(mouseEvent);
        return;
    }


    rectangle_position = mouseEvent->scenePos();
    active_rectangle = QRectF(mouseEvent->scenePos(), mouseEvent->scenePos());
}

void ImageScene::mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    if (mouseEvent->button() == Qt::LeftButton)
    {
        QPointF release_position = mouseEvent->scenePos();
        if (release_position.x() < 0.0)
            release_position.setX(0.0);
        if (release_position.y() < 0.0)
            release_position.setY(0.0);
        if (release_position.x() > width())
            release_position.setX(width());
        if (release_position.y() > height())
            release_position.setY(height());

        qDebug("release : x = %f | y = %f", release_position.x(), release_position.y());

        emit rectangle_changed(QRectF(rectangle_position, release_position));
    }

    QGraphicsScene::mousePressEvent(mouseEvent);
}

void ImageScene::mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    if (!(mouseEvent->buttons() && Qt::LeftButton))
        return;

    active_rectangle.setBottomRight(mouseEvent->scenePos());
    rectangle->setRect(active_rectangle);
    QGraphicsScene::mousePressEvent(mouseEvent);
}

void ImageScene::set_image(const QImage &_image)
{
    if (image)
    {
        removeItem(image);
        delete image;
    }

//    QPixmap pixmap = QPixmap::fromImage(_image.scaled(width(),
//                                                      height(),
//                                                      Qt::KeepAspectRatio,
//                                                      Qt::SmoothTransformation));
    QPixmap pixmap = QPixmap::fromImage(_image);
    image = addPixmap(pixmap);
}

void ImageScene::set_rectangle(QRectF _rectangle)
{
    if (rectangle)
    {
        removeItem(rectangle);
        delete rectangle;
    }
    rectangle = addRect(_rectangle);
}
