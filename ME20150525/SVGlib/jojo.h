#ifndef JOJO_H
#define JOJO_H

#include "potracelib.h"
#include "main.h"

#include <core/core.hpp>
#include <imgproc/imgproc.hpp>

struct Kujou
{
    int Kclass;
    int TURD;
    int TURDc;
    int OPAQUED;
    int NEEDc;
};

struct Jotarou
{
    potrace_state_t *st;
    std::vector<potrace_path_s*> vecAppend;
};

class JoJo
{
public:
    JoJo();
    JoJo(int _Kclass,int _TURD,int _TURDc,int _OPAQUED,int _NEEDc);
    ~JoJo();
    void hiJotarou(const cv::Mat& ImgOrg,const cv::Mat& ImgBW,FILE* fout,std::string postfix);
	void hiKujou(cv::Mat& ImgSmo);

private:
	Kujou* star;
    Jotarou* platinum;

	imginfo_t imginfo;

    void calc_dimensions(imginfo_t *imginfo, potrace_path_t *plist);

    void initInfo(Kujou* star);
    void mat2Bm(const cv::Mat& imgBW,potrace_bitmap_t* bm);
    void fillMask(cv::Mat& mask,cv::Point& origin,const cv::Mat& imgBW,const potrace_path_s* jaggy);
    void getRoiMask(cv::Mat& mask,cv::Point& origin,const cv::Mat& imgBW,const potrace_path_s* jaggy);
    int splitRoiMask(const cv::Mat& img, const cv::Mat& mask, std::vector<cv::Mat>& vecSplitMask);
    int selectRoiColor(const cv::Mat& roiBGR,const cv::Mat& mask);
    int getThreshVal_Otsu(const std::vector<int>& _src);
    void eliminateExtraPath(std::vector<potrace_path_s*>& vecAppend,const cv::Mat& imgBW);
};

#endif // JOJO_H
