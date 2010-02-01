#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QFrame>

class ImageView : public QFrame
{
    Q_OBJECT

public:
    ImageView(QWidget *parent);


protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
};

#endif // IMAGEVIEW_H
