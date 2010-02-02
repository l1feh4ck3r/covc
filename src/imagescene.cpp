#include "imagescene.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsRectItem>

ImageScene::ImageScene(QObject *parent)
        :QGraphicsScene(parent), rectangle(NULL)
{
}

void ImageScene::mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    qDebug("x = %f | y = %f", mouseEvent->scenePos().x(), mouseEvent->scenePos().y());

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
        emit rectangle_changed(QRectF(rectangle_position, mouseEvent->scenePos()));
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

void ImageScene::set_rectangle(QRectF _rectangle)
{
    if (!rectangle)
    {
        removeItem(rectangle);
        rectangle = NULL;
    }
    rectangle = addRect(_rectangle);
}
