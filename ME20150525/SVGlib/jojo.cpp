#include <core/core.hpp>
#include <imgproc/imgproc.hpp>

#include "potracelib.h"
#include "main.h"

#include "bitmap.h"

#include "progress.h"
#include "decompose.h"
#include "curve.h"
#include "trace.h"

#include "backend_svg.h"
#include "backend_eps.h"
#include "backend_pgm.h"

#include "greymap.h"
#include "render.h"
#include "lists.h"

#include "jojo.h"

using namespace std;
using namespace cv;

info_t info;

JoJo::JoJo()
{
    star=new Kujou;
    star->Kclass=3;
    star->TURD=5;
    star->TURDc=100;
    star->OPAQUED=0;
    star->NEEDc=1;

    platinum=new Jotarou;
    platinum->st=NULL;
}

JoJo::JoJo(int _Kclass, int _TURD, int _TURDc, int _OPAQUED, int _NEEDc)
{
    star=new Kujou;
    star->Kclass=_Kclass;
    star->TURD=_TURD;
    star->TURDc=_TURDc;
    star->OPAQUED=_OPAQUED;
    star->NEEDc=_NEEDc;

    platinum=new Jotarou;
    platinum->st=NULL;
}

JoJo::~JoJo()
{
    potrace_state_free(platinum->st);
    for(int i=0;i<platinum->vecAppend.size();++i)
        pathlist_free(platinum->vecAppend[i]);

    delete platinum;
    delete star;
}

void JoJo::hiJotarou(const Mat& ImgOrg,const Mat& ImgBW,FILE* fout,std::string postfix)
{
    //init ImgSeg
    //ImgSeg.create(ImgOrg.size(),CV_8UC3);
    //ImgSeg=Scalar(255,255,255);

    //init info
    initInfo(star);

    //cvt mat to potrace_bitmap_t
    potrace_bitmap_t *bm=bm_new(ImgBW.cols,ImgBW.rows);
    mat2Bm(ImgBW,bm);

    //init state
    potrace_state_t *st=(potrace_state_t *)malloc(sizeof(potrace_state_t));
	platinum->st=st;
    st->status=POTRACE_STATUS_OK;
    st->plist=NULL;
    st->priv=NULL;

    progress_t subprog;
    subprog.callback=NULL;

    //get root jaggy
    bm_to_pathlist(bm,&st->plist,info.param,&subprog);

    //...for the horde
    vector<potrace_path_s*> &vecAppend=platinum->vecAppend;
    potrace_param_t *param_extra=potrace_param_default();
    param_extra->turdsize=star->TURD;
    param_extra->turnpolicy=POTRACE_TURNPOLICY_BLACK;
    for(potrace_path_s *pRoot=st->plist;pRoot&&star->NEEDc;pRoot=pRoot->next)
    {
        //root deprecate '-'path
        if(!info.opaque&&pRoot->sign=='-')
            continue;

        //no more
        if(pRoot->color!=0)
            continue;

        //get root roi's mask
        Mat maskRoot;
        Point originRoot;
        getRoiMask(maskRoot,originRoot,ImgBW,pRoot);

        //split root roi's mask
        Mat maskR2Org(ImgOrg,Rect(originRoot.x,originRoot.y,maskRoot.cols,maskRoot.rows));
        vector<Mat> vecSplitMask;
        int maxRegionIdx=splitRoiMask(maskR2Org,maskRoot,vecSplitMask);

        //select color for root roi
        pRoot->color=selectRoiColor(maskR2Org,vecSplitMask[maxRegionIdx]);

        Vec3b rootColor(pRoot->color%(16*16),pRoot->color/(16*16)%(16*16),pRoot->color/int(pow(16,4)));
        //Mat maskR2Seg(ImgSeg,Rect(originRoot.x,originRoot.y,maskRoot.cols,maskRoot.rows));
        //for(int i=0;i<maskR2Seg.rows;++i)
        //{
        //    const uchar *pSplitM=maskRoot.ptr<uchar>(i);
        //    Vec3b *pMaskS=maskR2Seg.ptr<Vec3b>(i);
        //    for(int j=0;j<maskR2Seg.cols;++j)
        //    {
        //        if(pSplitM[j]==255)
        //            pMaskS[j]=rootColor;
        //    }
        //}

        //get extra path
        for(int idx=0;idx<star->Kclass;++idx)
        {
            if(idx!=maxRegionIdx)
            {
                Mat splitMaskInv(ImgBW.size(),CV_8U,Scalar::all(255));
                for(int i=0;i<vecSplitMask[idx].rows;++i)
                {
                    const uchar *pSmallR=vecSplitMask[idx].ptr<uchar>(i);
                    for(int j=0;j<vecSplitMask[idx].cols;++j)
                    {
                        if(pSmallR[j]==255)
                            splitMaskInv.at<uchar>(i+originRoot.y,j+originRoot.x)=0;
                    }
                }

                potrace_bitmap_t *bm_splitMaskInv=bm_new(ImgBW.cols,ImgBW.rows);
                mat2Bm(splitMaskInv,bm_splitMaskInv);

                //get sub jaggy
                potrace_path_s *pPath=NULL;
                bm_to_pathlist(bm_splitMaskInv,&pPath,param_extra,&subprog);

                if(pPath)
                {
                    for(potrace_path_s* pSubRoot=pPath;pSubRoot;pSubRoot=pSubRoot->next)
                    {
                        //sub deprecate '-'path
                        if(!info.opaque&&pSubRoot->sign=='-')
                            continue;

                        //get sub roi's mask
                        Mat maskSubRoot;
                        Point originSubRoot;
                        getRoiMask(maskSubRoot,originSubRoot,ImgBW,pSubRoot);

                        //select color for sub roi
                        Mat subMaskR2Org(ImgOrg,Rect(originSubRoot.x,originSubRoot.y,maskSubRoot.cols,maskSubRoot.rows));
                        pSubRoot->color=selectRoiColor(subMaskR2Org,maskSubRoot);

                        Vec3b subColor(pSubRoot->color%(16*16),pSubRoot->color/(16*16)%(16*16),pSubRoot->color/int(pow(16,4)));
                        //Mat subMaskR2Seg(ImgSeg,Rect(originSubRoot.x,originSubRoot.y,maskSubRoot.cols,maskSubRoot.rows));
                        //for(int i=0;i<subMaskR2Seg.rows;++i)
                        //{
                        //    const uchar *pSplitM=maskSubRoot.ptr<uchar>(i);
                        //    Vec3b *pMaskS=subMaskR2Seg.ptr<Vec3b>(i);
                        //    for(int j=0;j<subMaskR2Seg.cols;++j)
                        //    {
                        //        if(pSplitM[j]==255)
                        //            pMaskS[j]=subColor;
                        //    }
                        //}
                    }

                    vecAppend.push_back(pPath);
                }

                bm_free(bm_splitMaskInv);
            }
        }
    }

    //eliminate extra path
    //eliminateExtraPath(vecAppend,ImgBW);

    //get curve
    process_path(st->plist,info.param,&subprog);
    for(int i=0;i<vecAppend.size();++i)
        process_path(vecAppend[i],param_extra,&subprog);

    potrace_path_s **arrayAppend=new potrace_path_s* [vecAppend.size()];
    for(int i=0;i<vecAppend.size();++i)
        arrayAppend[i]=vecAppend[i];

    //save
    imginfo.pixwidth=bm->w;
    imginfo.pixheight=bm->h;

    calc_dimensions(&imginfo,st->plist);

	if(postfix=="svg")
		page_svg(fout,st->plist,&imginfo,arrayAppend,vecAppend.size());
	else if(postfix=="eps")
		page_eps(fout,st->plist,&imginfo);

    //...
    bm_free(bm);
    potrace_param_free(param_extra);
    delete []arrayAppend;
}

void JoJo::hiKujou(cv::Mat& ImgSmo)
{
	potrace_path_t *p;
	greymap_t *gm;
	render_t *rm;
	int w, h;
	trans_t t;

	t.bb[0] = imginfo.trans.bb[0]+imginfo.lmar+imginfo.rmar;
	t.bb[1] = imginfo.trans.bb[1]+imginfo.tmar+imginfo.bmar;
	t.orig[0] = imginfo.trans.orig[0]+imginfo.lmar;
	t.orig[1] = imginfo.trans.orig[1]+imginfo.bmar;
	t.x[0] = imginfo.trans.x[0];
	t.x[1] = imginfo.trans.x[1];
	t.y[0] = imginfo.trans.y[0];
	t.y[1] = imginfo.trans.y[1];

	w = (int)ceil(t.bb[0]);
	h = (int)ceil(t.bb[1]);

	gm = gm_new(w, h);
	rm = render_new(gm);
	gm_clear(gm, 255); /* white */

	list_forall(p, platinum->st->plist) {
		pgm_path(&p->curve, t, rm);
	}

	render_close(rm);

	ImgSmo.create(Size(w,h),CV_8U);
	for(int i=0;i<rm->gm->h;++i)
	{
		uchar *pSmo=ImgSmo.ptr<uchar>(rm->gm->h-i-1);
		for(int j=0;j<rm->gm->w;++j)
			pSmo[j]=GM_UGET(rm->gm,j,i);
	}

	render_free(rm);
	gm_free(gm);
}

void JoJo::calc_dimensions(imginfo_t *imginfo, potrace_path_t *plist)
{
    double dim_def;
    double maxwidth,maxheight,sc;
    int default_scaling=0;

    if (imginfo->pixwidth == 0) {
        imginfo->pixwidth = 1;
    }
    if (imginfo->pixheight == 0) {
        imginfo->pixheight = 1;
    }

    /* set the default dimension for width, height, margins */
    dim_def = DEFAULT_DIM;

    /* apply default dimension to width, height, margins */
    imginfo->width = info.width_d.x == UNDEF ? UNDEF : double_of_dim(info.width_d, dim_def);
    imginfo->height = info.height_d.x == UNDEF ? UNDEF : double_of_dim(info.height_d, dim_def);
    imginfo->lmar = info.lmar_d.x == UNDEF ? UNDEF : double_of_dim(info.lmar_d, dim_def);
    imginfo->rmar = info.rmar_d.x == UNDEF ? UNDEF : double_of_dim(info.rmar_d, dim_def);
    imginfo->tmar = info.tmar_d.x == UNDEF ? UNDEF : double_of_dim(info.tmar_d, dim_def);
    imginfo->bmar = info.bmar_d.x == UNDEF ? UNDEF : double_of_dim(info.bmar_d, dim_def);

    /* start with a standard rectangle */
    trans_from_rect(&imginfo->trans, imginfo->pixwidth, imginfo->pixheight);

    /* if info.tight is set, tighten the bounding box */
    if (info.tight) {
        trans_tighten(&imginfo->trans, plist);
    }

    /* sx/rx is just an alternate way to specify width; sy/ry is just an
     alternate way to specify height. */
    if (imginfo->width == UNDEF && info.rx != UNDEF) {
        imginfo->width = imginfo->trans.bb[0] / info.rx * 72;
    }
    if (imginfo->height == UNDEF && info.ry != UNDEF) {
        imginfo->height = imginfo->trans.bb[1] / info.ry * 72;
    }

    /* if one of width/height is specified, use stretch to determine the
     other */
    if (imginfo->width == UNDEF && imginfo->height != UNDEF) {
        imginfo->width = imginfo->height / imginfo->trans.bb[1] * imginfo->trans.bb[0] / info.stretch;
    } else if (imginfo->width != UNDEF && imginfo->height == UNDEF) {
        imginfo->height = imginfo->width / imginfo->trans.bb[0] * imginfo->trans.bb[1] * info.stretch;
    }

    /* if width and height are still variable, tenatively use the
     default scaling factor of 72dpi (for dimension-based backends) or
     1 (for pixel-based backends). For fixed-size backends, this will
     be adjusted later to fit the page. */
    if (imginfo->width == UNDEF && imginfo->height == UNDEF) {
        imginfo->width = imginfo->trans.bb[0];
        imginfo->height = imginfo->trans.bb[1] * info.stretch;
        default_scaling = 1;
    }

    /* apply scaling */
    trans_scale_to_size(&imginfo->trans, imginfo->width, imginfo->height);

    /* apply rotation, and tighten the bounding box again, if necessary */
    if (info.angle != 0.0) {
        trans_rotate(&imginfo->trans, info.angle);
        if (info.tight) {
            trans_tighten(&imginfo->trans, plist);
        }
    }

    /* adjust margins */
    if (imginfo->lmar == UNDEF) {
        imginfo->lmar = 0;
    }
    if (imginfo->rmar == UNDEF) {
        imginfo->rmar = 0;
    }
    if (imginfo->bmar == UNDEF) {
        imginfo->bmar = 0;
    }
    if (imginfo->tmar == UNDEF) {
        imginfo->tmar = 0;
    }
}

void JoJo::initInfo(Kujou* star)
{
    info.debug = 0;
    info.width_d.x = UNDEF;
    info.height_d.x = UNDEF;
    info.rx = UNDEF;
    info.ry = UNDEF;
    info.sx = UNDEF;
    info.sy = UNDEF;
    info.stretch = 1;
    info.lmar_d.x = UNDEF;
    info.rmar_d.x = UNDEF;
    info.tmar_d.x = UNDEF;
    info.bmar_d.x = UNDEF;
    info.angle = 0;
    info.paperwidth = DEFAULT_PAPERWIDTH;
    info.paperheight = DEFAULT_PAPERHEIGHT;
    info.tight = 0;
    info.unit = 10;
    info.compress = 1;
    info.pslevel = 2;
    info.color = 0x000000;
    info.gamma = 2.2;
    info.param = potrace_param_default();
    star->Kclass>star->TURD ? info.param->turdsize=star->Kclass : info.param->turdsize=star->TURD;
    info.param->turnpolicy=POTRACE_TURNPOLICY_BLACK;
    info.longcoding = 0;
    info.outfile = NULL;
    info.blacklevel = 0.5;
    info.invert = 0;
    info.opaque = star->OPAQUED;
    info.grouping = 1;
    info.fillcolor = 0xffffff;
    info.progress = 0;
}

void JoJo::mat2Bm(const Mat& imgBW,potrace_bitmap_t* bm)
{
    bm_clear(bm,0);

    for(int i=0;i<imgBW.rows;++i)
    {
        const uchar *pImgBWR=imgBW.ptr<uchar>(i);
        for(int j=0;j<imgBW.cols;++j)
            BM_UPUT(bm,j,imgBW.rows-i-1,pImgBWR[j]==0);
    }
}

void JoJo::fillMask(Mat& mask,Point& origin,const Mat& imgBW,const potrace_path_s* jaggy)
{
    vector<vector<Point> > contoursSt;
    contoursSt.push_back(vector<Point>());
    point_t *jaggySt=jaggy->priv->pt;
    int jaggyLen=jaggy->priv->len;

    long jaggy_minX=INT_MAX,jaggy_minY=INT_MAX;
    long jaggy_maxX=-1,jaggy_maxY=-1;
    for(int i=0;i<jaggyLen-1;++i)
    {
        int dirx=jaggySt[i+1].x-jaggySt[i].x;
        int diry=jaggySt[i+1].y-jaggySt[i].y;

        if(dirx==1)
            contoursSt[0].push_back(Point(jaggySt[i].x,imgBW.rows-1-jaggySt[i].y));
        else if(dirx==-1)
            contoursSt[0].push_back(Point(jaggySt[i+1].x,imgBW.rows-jaggySt[i].y));
        else if(diry==1)
            contoursSt[0].push_back(Point(jaggySt[i].x-1,imgBW.rows-1-jaggySt[i].y));
        else if(diry==-1)
            contoursSt[0].push_back(Point(jaggySt[i+1].x,imgBW.rows-1-jaggySt[i+1].y));

        if(contoursSt[0][i].x>jaggy_maxX)
            jaggy_maxX=contoursSt[0][i].x;
        if(contoursSt[0][i].x<jaggy_minX)
            jaggy_minX=contoursSt[0][i].x;
        if(contoursSt[0][i].y>jaggy_maxY)
            jaggy_maxY=contoursSt[0][i].y;
        if(contoursSt[0][i].y<jaggy_minY)
            jaggy_minY=contoursSt[0][i].y;
    }

    for(int i=0;i<jaggyLen-1;++i)
    {
        contoursSt[0].at(i).x-=jaggy_minX;
        contoursSt[0].at(i).y-=jaggy_minY;
    }

    origin=Point(jaggy_minX,jaggy_minY);
    mask=Mat(Size(jaggy_maxX-jaggy_minX+1,jaggy_maxY-jaggy_minY+1),CV_8U,Scalar::all(0));
    drawContours(mask,contoursSt,-1,Scalar::all(255),-1);
}

void JoJo::getRoiMask(Mat& mask,Point& origin,const Mat& imgBW,const potrace_path_s* jaggy)
{
    fillMask(mask,origin,imgBW,jaggy);
    for(potrace_path_s *pChild=jaggy->childlist;pChild;pChild=pChild->sibling)
    {
        Mat maskChild;
        Point originChild;

        fillMask(maskChild,originChild,imgBW,pChild);

        originChild-=origin;
        Mat maskC2R(mask,Rect(originChild.x,originChild.y,maskChild.cols,maskChild.rows));
        for(int i=0;i<maskC2R.rows;++i)
        {
            uchar *pRootR=maskC2R.ptr<uchar>(i);
            uchar *pChildR=maskChild.ptr<uchar>(i);
            for(int j=0;j<maskC2R.cols;++j)
                pRootR[j]^=pChildR[j];
        }
    }
}

int JoJo::splitRoiMask(const Mat& img,const Mat& mask,vector<Mat>& vecSplitMask)
{
    Mat R2OrgHSV;   //don't care this identifier...
    img.convertTo(R2OrgHSV,CV_32FC3);
    R2OrgHSV*=1.0/255.0;
    cvtColor(R2OrgHSV,R2OrgHSV,CV_BGR2Lab);

    int dataNum=0;
    for(int i=0;i<mask.rows;++i)
    {
        const uchar *pMaskR=mask.ptr<uchar>(i);
        for(int j=0;j<mask.cols;++j)
        {
            if(pMaskR[j]==255)
                ++dataNum;
        }
    }
    Mat roiData(dataNum,3,CV_32F);

    int dataCnt=0;
    for(int i=0;i<R2OrgHSV.rows;++i)
    {
        float *pImgR=R2OrgHSV.ptr<float>(i);
        const uchar *pMaskR=mask.ptr<uchar>(i);
        for(int j=0;j<R2OrgHSV.cols*R2OrgHSV.channels();j+=3)
        {
            if(pMaskR[j/3]==255)
            {
                float *pDataR=roiData.ptr<float>(dataCnt);

                pDataR[0]=pImgR[j];
                pDataR[1]=pImgR[j+1];
                pDataR[2]=pImgR[j+2];

                ++dataCnt;
            }
        }
    }

    Mat roiLabel,roiCenter;
    kmeans(roiData,star->Kclass,roiLabel,TermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER,10,1.0),1,KMEANS_PP_CENTERS,roiCenter);

    Mat_<int>::iterator itLabel=roiLabel.begin<int>();
    Mat _mask;
    mask.copyTo(_mask);
    for(int i=0;i<_mask.rows;++i)
    {
        uchar *pMaskR=_mask.ptr<uchar>(i);
        for(int j=0;j<_mask.cols;++j)
        {
            if(pMaskR[j]==255)
            {
                //...
                pMaskR[j]=*itLabel+1;

                ++itLabel;
            }
        }
    }

    vecSplitMask.clear();
    for(int i=0;i<star->Kclass;++i)
    {
        Mat imgBlack(mask.size(),CV_8U,Scalar::all(0));
        vecSplitMask.push_back(imgBlack);
    }

    int *regionSize=new int[star->Kclass];
    memset(regionSize,0,sizeof(regionSize));
    for(int i=0;i<_mask.rows;++i)
    {
        const uchar *pMaskR=_mask.ptr<uchar>(i);
        for(int j=0;j<_mask.cols;++j)
        {
            if(pMaskR[j]!=0)
            {
                vecSplitMask[pMaskR[j]-1].at<uchar>(i,j)=255;
                ++regionSize[pMaskR[j]-1];
            }
        }
    }

    int maxIdx=0,maxSize=regionSize[0];
    for(int i=1;i<star->Kclass;++i)
    {
        if(regionSize[i]>maxSize)
        {
            maxSize=regionSize[i];
            maxIdx=i;
        }
    }

	delete []regionSize;

    return maxIdx;
}

int JoJo::selectRoiColor(const Mat& roiBGR,const Mat& mask)
{
    int channels[]={0,1,2};
    int bins=256;
    int histSize[]={bins,bins,bins};
    float ranges[]={0,256};
    const float* histRange[]={ranges,ranges,ranges};

    MatND hist;
    calcHist(&roiBGR,1,channels,mask,hist,3,histSize,histRange);
    double max_bgr;
    int idx_bgr[3];
    minMaxIdx(hist,0,&max_bgr,0,idx_bgr);

    int color=idx_bgr[0];
    color+=idx_bgr[1]*int(pow(16,2));
    color+=idx_bgr[2]*int(pow(16,4));

    return color;
}

int JoJo::getThreshVal_Otsu(const vector<int>& _src)
{
    int max_src=_src[0];
    for(int i=1;i<_src.size();++i)
    {
        if(_src[i]>max_src)
            max_src=_src[i];
    }

    const int N=max_src+1;
    int *h=new int[N];
    memset(h,0,sizeof(h));
    for(int i=0;i<_src.size();++i)
        ++h[_src[i]];

    double mu=0,scale=1.0/double(_src.size());
    for(int i=0;i<N;++i)
        mu+=i*(double)h[i];

    mu*=scale;
    double mu1=0,q1=0;
    double max_sigma=0;
    int max_val=0;

    for(int i=0;i<N;++i)
    {
        double p_i,q2,mu2,sigma;

        p_i=h[i]*scale;
        mu1*=q1;
        q1+=p_i;
        q2=1.0-q1;

        if(min(q1,q2)<FLT_EPSILON || max(q1,q2)>1.0-FLT_EPSILON)
            continue;

        mu1=(mu1+i*p_i)/q1;
        mu2=(mu-q1*mu1)/q2;
        sigma=q1*q2*(mu1-mu2)*(mu1-mu2);
        if(sigma>max_sigma)
        {
            max_sigma=sigma;
            max_val=i;
        }
    }

	delete []h;

    return max_val;
}

void JoJo::eliminateExtraPath(vector<potrace_path_s*>& vecAppend,const Mat& imgBW)
{
    vector<int> vecArea;
    for(int i=0;i<vecAppend.size();++i)
    {
        for(potrace_path_s* pSubRoot=vecAppend[i];pSubRoot;pSubRoot=pSubRoot->next)
            vecArea.push_back(pSubRoot->area);
    }
    int otsuArea=getThreshVal_Otsu(vecArea)/2;

    potrace_bitmap_t *bm_tmp=bm_new(imgBW.cols,imgBW.rows);
    for(int i=0;i<vecAppend.size();++i)
    {
        potrace_path_s* pre=vecAppend[i];
        potrace_path_s* cur=vecAppend[i]->next;
        while(cur)
        {
            if(cur->area<=otsuArea)
            {
                pre->next=cur->next;
                path_free(cur);
                cur=pre->next;
            }
            else
            {
                cur->childlist=NULL;
                cur->sibling=NULL;

                pre=cur;
                cur=cur->next;
            }
        }

        pathlist_to_tree(vecAppend[i],bm_tmp);
    }
    bm_free(bm_tmp);
}
