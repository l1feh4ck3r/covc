#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QGraphicsView>

class ImageView : public QGraphicsView
{
    Q_OBJECT

public:
    ImageView(QWidget *parent);


public:
signals:
    void rectangle_changed(QRect rect);


protected:
    void mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent);
    

private:
    QPoint          rectangle_position;
};

#endif // IMAGEVIEW_H
