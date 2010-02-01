#include "imageview.h"

ImageView::ImageView(QWidget *parent)
        :QFrame(parent)
{
    setMinimumSize(650, 599);
    setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
    setAcceptDrops(true);
}

void ImageView::mousePressEvent(QMouseEvent *event)
{
}

void ImageView::mouseReleaseEvent(QMouseEvent *event)
{
}

void ImageView::mouseMoveEvent(QMouseEvent *event)
{
}
