#ifndef PAINTERWIDGET_H
#define PAINTERWIDGET_H

#include <QtGui>
#include <cv.h>
#include <core/core.hpp>
#include <highgui/highgui.hpp>
#include <imgproc/imgproc.hpp>
#include "cutdialog.h"
#include <BasicPro.h>

using namespace std;
using namespace cv;

class QPoint;
class QPixmap;
class QColor;

class PainterWidget : public QWidget
{
    Q_OBJECT

public:
    PainterWidget();  //构造函数

    enum ShapeType {  //枚举准备绘制的图形
        Cursor,
        CutRect,
        Line,
        Rubber,
        Rectangle,
        Ellipse
    };

    QImage bufferImage;   //用来存储最终的图形
    QImage tempImage;     //临时缓冲区
    QImage curentImage;      //
    QImage originalImage, initImage; //最原始的图片

    cv::Mat ImgMinus,ImgEdge,LineImage,TextureImage,ImgThresEdge; //Opencv参数
    cv::Mat originalMat;
    cv::Mat seg_fg,seg_bg,SegMask,forShow;

    ShapeType curShape;           //当前绘制的功能
    QPoint startPoint, endPoint;  //起始点、终点
    QColor color;                 //调色盘颜色

    void paint(QImage &image);       //绘图
    void setShape(ShapeType shape);  //设置绘图形状
    QString loadFormFile();          //加载图片
    void saveImage();               //保存图片
    void Linedrawing(int, int);  //生成线描图
    void reverseImg();           //取反

    cutDialog cutwindow;

    int x_shift, y_shift;
    int thres_Gauss, thres_Canny;

protected:
    bool bDrawing, bcuting; //当前绘图状态（绘图中、截图中）
    double scalefactor;     //缩放系数
    int x,y,w,h;            //位置参数
    int Rubbersize;         //橡皮擦大小

    void mousePressEvent(QMouseEvent *event);   //鼠标按下事件
    void mouseMoveEvent(QMouseEvent *event);    //鼠标移动事件
    void mouseReleaseEvent(QMouseEvent *event); //鼠标松开事件
    void paintEvent(QPaintEvent *event);        //更新绘制区

public slots:
    void setColor(QColor);   //设置画笔颜色
    void clear();            //清屏
    void setCutAct();        //设置当前功能为截图
    void setthinsize();      //设置橡皮擦大小为细
    void setmidsize();       //设置橡皮擦大小为中
    void setwidesize();      //设置橡皮擦大小为粗
    void BacktoOriginal();   //回到原图
    void SetthresGauss(int); //调整阈值1（Gauss阈值）
    void SetthresCanny(int); //调整阈值2（Canny阈值）
    void RotateImg(int Value);
    void RecoverImg();
};

#endif // PAINTERWIDGET_H
