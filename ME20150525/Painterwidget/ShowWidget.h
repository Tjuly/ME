#ifndef SHOWWIDGET_H
#define SHOWWIDGET_H

#include <QtGui>

class ShowWidget : public QWidget
{
    Q_OBJECT

public:
    ShowWidget();

    QImage ShowImage;

    int x_shift, y_shift;

    void RecImg(QImage SrcImg);

protected:
    void paintEvent(QPaintEvent *event);
};

#endif // SHOWWIDGET_H
