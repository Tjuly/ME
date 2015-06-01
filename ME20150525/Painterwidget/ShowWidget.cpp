#include "ShowWidget.h"

ShowWidget::ShowWidget()
{
    setAutoFillBackground(true);     //自动填充背景
    setPalette(QPalette(Qt::gray)); //画板背景色初始化为白色

    x_shift = 0;
    y_shift = 0;
}

void ShowWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    x_shift = this->width() - ShowImage.width() > 0 ? (this->width() - ShowImage.width())/2 : 0;
    y_shift = this->height() - ShowImage.height() > 0 ? (this->height() - ShowImage.height())/2 : 0;

    painter.drawImage(x_shift, y_shift,ShowImage);

    setMinimumSize(ShowImage.width(), ShowImage.height());
}

void ShowWidget::RecImg(QImage SrcImg)
{
    ShowImage = SrcImg;

    update();
}
