#ifndef IMAGESCENE_H
#define IMAGESCENE_H

#include <QGraphicsScene>

class ImageScene : public QGraphicsScene
{
    Q_OBJECT

public:
    ImageScene(QObject *parent);


public:
    void set_rectangle(QRectF _rectangle);
    void set_image(const QImage &_image);

signals:
    void rectangle_changed(QRectF rectangle);


protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent);
    

private:
    QPointF                 rectangle_position;
    QRectF                  active_rectangle;
    QGraphicsRectItem      *rectangle;
    QGraphicsPixmapItem    *image;
};

#endif // IMAGESCENE_H
