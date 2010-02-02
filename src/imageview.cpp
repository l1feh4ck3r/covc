#include "imageview.h"

#include <QGraphicsSceneMouseEvent>

ImageView::ImageView(QWidget *parent)
        :QGraphicsView(parent)
{
    setMinimumSize(650, 599);
    setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
}

void ImageView::mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    qDebug("mousePressEvent");

    if (mouseEvent->button() == Qt::LeftButton)
        rectangle_position = mouseEvent->screenPos(); //scenePos() //pos();
}

void ImageView::mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    qDebug("mouseReleaseEvent");

    if (mouseEvent->button() == Qt::LeftButton)
    {
        emit rectangle_changed(QRect(rectangle_position, mouseEvent->screenPos()));
    }
}

void ImageView::mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
}
