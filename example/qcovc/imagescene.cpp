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

#include "imagescene.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsRectItem>

ImageScene::ImageScene(QObject *parent)
        :QGraphicsScene(parent), rectangle(NULL), image(NULL)
{
}

void ImageScene::mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
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
    if (mouseEvent->button() != Qt::LeftButton)
    {
        QGraphicsScene::mousePressEvent(mouseEvent);
        return;
    }

    QPointF release_position = mouseEvent->scenePos();
    if (release_position.x() < 0.0)
        release_position.setX(0.0);
    if (release_position.y() < 0.0)
        release_position.setY(0.0);
    if (release_position.x() > width())
        release_position.setX(width());
    if (release_position.y() > height())
        release_position.setY(height());

    emit rectangle_changed(QRectF(rectangle_position, release_position));
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
        image = NULL;
    }

    QPixmap pixmap = QPixmap::fromImage(_image);
    image = addPixmap(pixmap);
}

void ImageScene::set_rectangle(QRectF _rectangle)
{
    if (rectangle)
    {
        removeItem(rectangle);
        delete rectangle;
        rectangle = NULL;
    }

    rectangle = addRect(_rectangle);
    active_rectangle = _rectangle;
}
