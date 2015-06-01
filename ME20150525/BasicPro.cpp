#include "BasicPro.h"

#include <core/core.hpp>
#include <imgproc/imgproc.hpp>
#include <highgui/highgui.hpp>

bool cmpColorNum(const colorNum & a,const colorNum & b)
{
    return a.num>b.num;
}

cv::Mat BasicPro::QImage2Mat(QImage &src)
{
    return cv::Mat(src.height(),src.width(),CV_8UC4,(uchar*)src.bits(),src.bytesPerLine());
}

void BasicPro::revImg(QImage& srcq)
{
    cv::Mat src = QImage2Mat(srcq);

    for(int i=0;i<src.rows;++i)
    {
        cv::Vec4b *pSrc=src.ptr<cv::Vec4b>(i);
        for(int j=0;j<src.cols;++j)
        {
            pSrc[j][0]=255-pSrc[j][0];
            pSrc[j][1]=255-pSrc[j][1];
            pSrc[j][2]=255-pSrc[j][2];
        }
    }
}

void BasicPro::rotImg(QImage& src,double angle)
{


}

double getOtsu( const cv::Mat& _src , const cv::Mat& _mask)
{
    const int N = 256;
    int i, j, num = 0, h[N] = {0};

    cv::Mat _seg;
    _mask.copyTo(_seg);
    if(_seg.empty())
        _seg=cv::Mat(_src.size(),CV_8U,cv::Scalar::all(255));

    for( i = 0; i < _src.rows; i++ )
    {
        const uchar* src = _src.ptr<uchar>(i);
        const uchar* seg = _seg.ptr<uchar>(i);

        for( j=0 ; j < _src.cols; j++ )
        {
            if(seg[j]==255)
            {
                num++;
                h[src[j]]++;
            }
        }
    }

    double mu = 0, scale = 1./double(num);
    for( i = 0; i < N; i++ )
        mu += i*(double)h[i];

    mu *= scale;
    double mu1 = 0, q1 = 0;
    double max_sigma = 0, max_val = 0;

    for( i = 0; i < N; i++ )
    {
        double p_i, q2, mu2, sigma;

        p_i = h[i]*scale;
        mu1 *= q1;
        q1 += p_i;
        q2 = 1. - q1;

        if(std::min(q1,q2) < FLT_EPSILON || std::max(q1,q2) > 1. - FLT_EPSILON)
            continue;

        mu1 = (mu1 + i*p_i)/q1;
        mu2 = (mu - q1*mu1)/q2;
        sigma = q1*q2*(mu1 - mu2)*(mu1 - mu2);
        if( sigma > max_sigma )
        {
            max_sigma = sigma;
            max_val = i;
        }
    }

    return max_val;
}

void updateImgRes(cv::Mat& ImgMinus,cv::Mat& ImgEdge,cv::Mat& SegMask,cv::Mat& ImgThresEdge)
{
    cv::Mat ImgMinusOtsu(ImgMinus.size(),CV_8U,cv::Scalar::all(255));

    cv::threshold(ImgMinus ,ImgMinusOtsu, getOtsu(ImgMinus,SegMask) ,255, cv::THRESH_BINARY);
    //threshold(ImgMinus ,ImgMinusOtsu, -1 ,255, THRESH_OTSU);

    for(int i=0;i<ImgThresEdge.rows;++i)
    {
        uchar *pResR=ImgThresEdge.ptr<uchar>(i);
        for(int j=0;j<ImgThresEdge.cols;++j)
            pResR[j]=255;
    }

    for(int i=1;i<ImgEdge.rows-1;++i)
    {
        uchar *pEdgeR=ImgEdge.ptr<uchar>(i);

        uchar *pMinusUpR=ImgMinus.ptr<uchar>(i-1);
        uchar *pMinusCenR=ImgMinus.ptr<uchar>(i);
        uchar *pMinusDownR=ImgMinus.ptr<uchar>(i+1);

        uchar *pResUpR=ImgThresEdge.ptr<uchar>(i-1);
        uchar *pResCenR=ImgThresEdge.ptr<uchar>(i);
        uchar *pResDownR=ImgThresEdge.ptr<uchar>(i+1);
        for(int j=1;j<ImgEdge.cols-1;++j)
        {
            if(pEdgeR[j]==0)
            {
                for(int offset=-1;offset<=1;++offset)
                {
                    if(pMinusUpR[j+offset]!=255)
                        pResUpR[j+offset]=0;

                    if(pMinusCenR[j+offset]!=255)
                        pResCenR[j+offset]=0;

                    if(pMinusDownR[j+offset]!=255)
                        pResDownR[j+offset]=0;
                }
            }
        }
    }

    ImgThresEdge&=ImgMinusOtsu;

    if(SegMask.data)
    {
        for(int i=0;i<ImgThresEdge.rows;++i)
        {
            uchar *pResR=ImgThresEdge.ptr<uchar>(i);
            uchar *pSegMaskR=SegMask.ptr<uchar>(i);
            for(int j=0;j<ImgThresEdge.cols;++j)
            {
                if(pSegMaskR[j]==0)
                    pResR[j]=255;
            }
        }
    }
}

void BasicPro::lineDrawing(QImage& srcq,int thres1,int thres2)
{
    cv::Mat src=QImage2Mat(srcq);
    cv::Mat Img,ImgBlur,ImgMinus,ImgEdge;

    cv::cvtColor(src,Img,CV_BGRA2GRAY);

    cv::GaussianBlur(Img,ImgBlur,cv::Size(2*thres1+1,2*thres1+1),0,0);//thres1 范围1-100

    ImgMinus.create(Img.size(),CV_8U);
    for(int i=0;i<ImgMinus.rows;++i)
    {
        uchar *pMinusR=ImgMinus.ptr<uchar>(i);
        uchar *pBlurR=ImgBlur.ptr<uchar>(i);
        uchar *pImgR=Img.ptr<uchar>(i);
        for(int j=0;j<ImgMinus.cols;++j)
            pMinusR[j]=cv::saturate_cast<uchar>(int(255.0/pBlurR[j]*pImgR[j]));
    }

    cv::blur(Img,ImgEdge,cv::Size(3,3));
    cv::Canny(ImgEdge,ImgEdge,thres2,3*thres2);//thres2 范围0-100
    cv::Mat ImgWhite(Img.rows,Img.cols,CV_8U,cv::Scalar::all(255));
    ImgEdge=255-ImgEdge;

    if(!ImgMinus.empty() && !ImgEdge.empty())
    {
        cv::Mat ImgThresEdge(src.size(), CV_8U);
        updateImgRes(ImgMinus,ImgEdge,ImgWhite,ImgThresEdge);

        for(int i=0;i<src.rows;++i)
        {
            cv::Vec4b *pSrc=src.ptr<cv::Vec4b>(i);
            uchar *pTmp=ImgThresEdge.ptr<uchar>(i);
            for(int j=0;j<src.cols;++j)
            {
                pSrc[j][0]=pTmp[j];
                pSrc[j][1]=pTmp[j];
                pSrc[j][2]=pTmp[j];
            }
        }
    }
}

QVector<colorNum> BasicPro::Maincolor(QImage qImg)
{
    cv::Mat mat = cv::Mat(qImg.height(), qImg.width(), CV_8UC4, (uchar*)qImg.bits(), qImg.bytesPerLine());
    cv::Mat Img = cv::Mat(mat.rows, mat.cols, CV_8UC3 );
    int from_to[] = { 0,0, 1,1, 2,2 };
    cv::mixChannels( &mat, 1, &Img, 1, from_to, 3 );

    const int K = 7;

    cv::Mat ImgCvt;
    cvtColor(Img,ImgCvt,CV_BGR2Lab);

    cv::Mat ImgData(Img.rows*Img.cols,1,CV_32FC3);
    cv::Mat_<cv::Vec3b>::iterator itCvt=ImgCvt.begin<cv::Vec3b>();
    cv::Mat_<cv::Vec3f>::iterator itData=ImgData.begin<cv::Vec3f>();
    for(;itCvt<ImgCvt.end<cv::Vec3b>();itCvt += 2,itData += 2)
        *itData=*itCvt;

    cv::Mat ImgLab,ImgCen;
    kmeans(ImgData,K,ImgLab,cv::TermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER,10,1.0),1,cv::KMEANS_PP_CENTERS,ImgCen);

    cv::Mat ImgCenBGR(ImgCen.rows,1,CV_8UC3);
    for(int i=0;i<ImgCen.rows;++i)
    {
        float *pCen=ImgCen.ptr<float>(i);
        ImgCenBGR.at<cv::Vec3b>(i,0) = cv::Vec3b(uchar(pCen[0]),uchar(pCen[1]),uchar(pCen[2]));
    }

    cvtColor(ImgCenBGR,ImgCenBGR,CV_Lab2BGR);

    QVector<cv::Vec3b> vecCen(ImgCen.rows);
    for(int i=0;i<ImgCen.rows;++i)
        vecCen[i]=ImgCenBGR.at<cv::Vec3b>(i,0);

    cv::Mat ImgRes(Img.size(),CV_8UC3);
    QVector<colorNum> vecColorNum(ImgCen.rows);
    for(size_t i=0;i<vecColorNum.size();++i)
    {
        vecColorNum[i].color=vecCen[i];
        vecColorNum[i].num=0.0;
    }

    cv::Mat_<int>::iterator itLabel = ImgLab.begin<int>();
    cv::Mat_<cv::Vec3b>::iterator itRes = ImgRes.begin<cv::Vec3b>();
    for(;itLabel<ImgLab.end<int>();++itLabel,++itRes)
    {
        *itRes = vecCen[*itLabel];
        vecColorNum[*itLabel].num += 1.0;
    }

    std::sort(vecColorNum.begin(), vecColorNum.end(), cmpColorNum);
    for(size_t i=0; i<vecColorNum.size(); ++i)
        vecColorNum[i].num /= double(ImgLab.rows);

    return vecColorNum;
}
