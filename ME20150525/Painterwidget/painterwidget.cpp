#include "painterwidget.h"

PainterWidget::PainterWidget()
{
    setAutoFillBackground(true);     //自动填充背景
    setPalette(QPalette(Qt::gray)); //画板背景色初始化为白色

    Rubbersize = 5;   //初始化橡皮擦大小为10
    scalefactor = 1.0; //初始化缩放值为1
    x_shift = 0;
    y_shift = 0;
    thres_Gauss = 3;
    thres_Canny = 50;

//    QDesktopWidget* desktopWidget = QApplication::desktop(); //获得当前工作区屏幕大小
//    QRect clientRect = desktopWidget->availableGeometry();

//    bufferImage = QImage(clientRect.width(),clientRect.height(),QImage::Format_ARGB32);
//    bufferImage.fill(Qt::white);

//    tempImage = QImage(clientRect.width(),clientRect.height(),QImage::Format_ARGB32);

//    setMinimumSize(clientRect.width(),clientRect.height()); //绘制区窗体的最小尺寸

    setMouseTracking(true);//打开鼠标追踪状态
}

void PainterWidget::setShape(ShapeType shape) //设置当前绘制的图形种类
{
    if(shape != curShape){
        curShape = shape;
    }
}

void PainterWidget::mousePressEvent(QMouseEvent *event) //鼠标按下事件
{
    if(event->button() == Qt::LeftButton) //判断是否鼠标左键按下，进入绘图状态
    {
        switch(curShape) //准备要绘制的图形
        {
            case Cursor:
            {
                break;
            }
            case Line:
            {
                startPoint = event->pos() / scalefactor;  //记录起点位置
                break;
            }
            case Rubber:
            {
                startPoint = event->pos() / scalefactor;  //记录起点位置
                break;
            }
            case Rectangle:
            case Ellipse:
            {
                 bDrawing = true;
                 x = (event->x() - x_shift) / scalefactor;
                 y = (event->y() - y_shift) / scalefactor;
                 break;
            }
            case CutRect:
            {
                 bDrawing = true;
                 bcuting = true;
                 x = (event->x() - x_shift) / scalefactor;
                 y = (event->y() - y_shift) / scalefactor;
                 break;
            }
        }
    }
    else if(event->button() == Qt::MidButton)   //判断是否鼠标中键按下，进行缩小操作
    {
        scalefactor -= 0.1;
    }
    else if(event->button() == Qt::RightButton) //判断是否鼠标右键按下，进行放大操作
    {
        scalefactor += 0.1;
    }
}

void PainterWidget::mouseMoveEvent(QMouseEvent *event) //鼠标移动事件
{
    if(event->pos().x() > 0 || event->pos().y()>0) //鼠标进入活动区
    {
        QCursor cursor;

        if(curShape == Cursor)//当前功能为游标
        {
           cursor = QCursor(Qt::ArrowCursor);
        }

        if(curShape == Line)//当前功能为画线
        {
            QPixmap pixmap(":/Cursorshape/pen.png") ;
            cursor = QCursor(pixmap,-1,-1);
        }

        if(curShape == Rubber)//当前功能为橡皮擦
        {
            if(Rubbersize == 5){
                QPixmap pixmap(":/Cursorshape/rubber1.png");
                cursor = QCursor(pixmap,-1,-1);
            }
            else if(Rubbersize == 10){
                QPixmap pixmap(":/Cursorshape/rubber2.png");
                cursor = QCursor(pixmap,-1,-1);
            }
            else if(Rubbersize == 20){
                QPixmap pixmap(":/Cursorshape/rubber3.png");
                cursor = QCursor(pixmap,-1,-1);
            }
        }

        if(curShape == CutRect)//当前功能为截图
        {
            cursor = QCursor(Qt::CrossCursor);
        }

        setCursor(cursor);
    }

    QPainter painter(&bufferImage); //定义一个在bufferImage上绘图的painter

    if(event->buttons()&Qt::LeftButton) //判断移动鼠标时，左键是否一直按下去
    {
        switch(curShape)
        {
            case Line:
            {
                QPen pen;
                pen.setColor(color); //设置画笔的颜色
                painter.setPen(pen);
                endPoint=event->pos() / scalefactor; //记录终点位置
                painter.drawLine(startPoint,endPoint);
                startPoint=endPoint; //更新鼠标当前位置，为下次绘制做准备
                break;
            }
            case Rubber:
            {
                painter.setPen(QPen(Qt::white,Rubbersize));  //橡皮擦是画笔颜色为白色，线宽为Rubbersize的画笔
                endPoint=event->pos()  / scalefactor;
                painter.drawLine(startPoint,endPoint);
                startPoint=endPoint;
                break;
            }
            case Rectangle:
            case Ellipse:
            {
                w = (event->x() - x_shift) / scalefactor - x;
                h = (event->y() - y_shift) / scalefactor - y;
                break;
            }
            case CutRect:
            {
                w = (event->x() - x_shift) / scalefactor - x;
                h = (event->y() - y_shift) / scalefactor - y;
                break;
            }
        }
        tempImage = bufferImage;
        paint(tempImage);

        update();   //强迫窗口重绘
    }
    else if(event->buttons()&Qt::RightButton)//右键滑动为放大图层
    {
        scalefactor += 0.01;
        update();   //强迫窗口重绘
    }
    else if(event->buttons()&Qt::MidButton)//中键滑动为缩小图层
    {
        scalefactor -= 0.01;
        update();   //强迫窗口重绘
    }
}

void PainterWidget::mouseReleaseEvent(QMouseEvent *event)//鼠标松开事件
{
    bDrawing = false;   //绘图过程结束（中间结果保留在TempImage上）
    paint(bufferImage); //开始绘图（将最终结果绘制到BufferImg上）

    if(curShape == CutRect && bcuting)//如果当前状态为截图
    {
        if(QDialog::Accepted == cutwindow.exec())
        {
            curentImage = bufferImage.copy(x+2, y+2, w-4, h-4); //拷贝截图区域
            bufferImage = curentImage;
            originalImage = curentImage;
        }
        else
        {
            bufferImage = tempImage = curentImage;
            scalefactor = 1.0;
        }

        curShape = Cursor;

        update();

        bcuting = false;  //关闭截图状态
    }
}

void PainterWidget::paintEvent(QPaintEvent *event)  //完成绘制区窗体的更新工作
{
    QPainter painter(this);

    painter.scale(scalefactor ,scalefactor);  //坐标轴缩放

    x_shift = this->width() - bufferImage.width() > 0 ? (this->width() - bufferImage.width())/2 : 0;
    y_shift = this->height() - bufferImage.height() > 0 ? (this->height() - bufferImage.height())/2 : 0;

    if(bDrawing)
         painter.drawImage(x_shift, y_shift,tempImage);   //中间结果绘制在tempImage上
    else
         painter.drawImage(x_shift, y_shift,bufferImage); //最终结果绘制在bufferImage上

    setMinimumSize(bufferImage.width() * scalefactor,bufferImage.height() * scalefactor); //绘制区窗体的最小尺寸
}

void PainterWidget::setColor(QColor c) //接收主窗口传来的画笔颜色值
{
    color = c;
}

void PainterWidget::paint(QImage &image)//绘制图形
{
    QPainter painter(&image);
    painter.setPen(QPen(Qt::blue,1)); //设置画笔形式为蓝色，宽度为1

    switch(curShape){

        case Rectangle:
            painter.drawRect(x,y,w,h); //以x,y为左上顶点，w,h为宽高绘制矩形
            break;

        case Ellipse:
            painter.drawEllipse(x,y,w,h); //以x,y为左上顶点，w,h为宽高绘制椭圆
            break;

        case CutRect:
            painter.setPen(QPen(Qt::black,1,Qt::DashLine)); //设置画笔形式为白色虚线，宽度为1
            painter.drawRect(x,y,w,h); //以x,y为左上顶点，w,h为宽高绘制截图矩形
            break;
    }

    update(); //更新窗口
}

void PainterWidget::clear() //完成绘制区的清屏
{
    bufferImage.fill(Qt::gray);

    scalefactor = 1.0;

    update();
}

QString PainterWidget::loadFormFile() //打开图片
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),":/",tr("Images (*.png *.jpg *.bmp)"));

    bufferImage = originalImage = curentImage = initImage =QImage(fileName);

    scalefactor = 1.0; //打开新图片时，初始化缩放值

    update();

    cv::Mat mat = cv::Mat(originalImage.height(), originalImage.width(), CV_8UC4, (uchar*)originalImage.bits(), originalImage.bytesPerLine());
    originalMat = cv::Mat(mat.rows, mat.cols, CV_8UC3 );
    int from_to[] = { 0,0, 1,1, 2,2 };
    cv::mixChannels( &mat, 1, &originalMat, 1, from_to, 3 );

    return fileName;
}

void PainterWidget::saveImage() //保存图片
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),":/untitled.png",tr("Images (*.png *.jpg *.bmp)"));
    bufferImage.save(fileName);
}

void PainterWidget::setCutAct() //设置当前功能为截图
{
    curShape = CutRect;
}

void PainterWidget::BacktoOriginal() //回到原图
{
    bufferImage = tempImage = curentImage = originalImage;

    update();
}

void PainterWidget::setthinsize() //设置橡皮擦大小为2
{
    Rubbersize = 5;
}

void PainterWidget::setmidsize() //设置橡皮擦大小为10
{
    Rubbersize = 10;
}

void PainterWidget::setwidesize() //设置橡皮擦大小为20
{
    Rubbersize = 20;
}

void PainterWidget::Linedrawing(int threshold1,int threshold2)//生成线描图
{
    BasicPro::lineDrawing(bufferImage, threshold1, threshold2);

    update();
}

void PainterWidget::SetthresGauss(int Value) //调整阈值1（Gauss阈值）
{
    bufferImage = curentImage;
    BasicPro::lineDrawing(bufferImage, Value, thres_Canny);
    thres_Gauss = Value;
    qDebug()<<"thres1";

    update();
}

void PainterWidget::SetthresCanny(int Value) //调整阈值2（Canny阈值）
{
    bufferImage = curentImage;
    BasicPro::lineDrawing(bufferImage, thres_Gauss, Value);
    thres_Canny = Value;
    qDebug()<<"thres2";

    update(); 
}

void PainterWidget::reverseImg()
{
    BasicPro::revImg(originalImage);
    BasicPro::revImg(curentImage);
    BasicPro::revImg(bufferImage);

    scalefactor = 1.0; //打开新图片时，初始化缩放值

    update();
}

void PainterWidget::RotateImg(int Value)
{
    QMatrix matrix;
    matrix.rotate(Value);

    curentImage = originalImage.transformed(matrix);
    bufferImage = curentImage;

    update();
}

void PainterWidget::RecoverImg()
{
    bufferImage = tempImage = curentImage = originalImage = initImage;

    update();
}
