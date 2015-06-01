#ifndef BASICPRO_H
#define BASICPRO_H

#include <core/core.hpp>
#include <QtGui>

typedef struct
{
    cv::Vec3b color;
    double num;
}colorNum;

class BasicPro
{
public:
    static cv::Mat QImage2Mat(QImage& src);

    static void revImg(QImage& srcq);
    static void rotImg(QImage &srcq, double angle);

    static void lineDrawing(QImage& srcq, int thres1, int thres2);

    static QVector<colorNum> Maincolor(QImage qImg);
};


#endif // BASICPRO_H
