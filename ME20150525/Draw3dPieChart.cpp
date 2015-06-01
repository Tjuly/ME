#include "Draw3dPieChart.h"
#include <math.h> -

// QColor cstDownPieColor[7] = {
//          QColor(164,159,148),
//          QColor(206,205,201),
//          QColor(130,133,119),
//          QColor(0,0,0),
//          QColor(43,7,1),
//          QColor(132,109,76),
//          QColor(124,85,40)
//         };
// QColor cstUpPieColor[7] = {
//         QColor(184,179,168),
//         QColor(226,225,221),
//         QColor(150,153,139),
//         QColor(16,12,12),
//         QColor(63,27,21),
//         QColor(152,129,96),
//         QColor(144,105,60)
//         }; //这里我定义了七种颜色，最大设为七块-

CDraw3dPieChart::CDraw3dPieChart(QWidget *parent)
   :QWidget(parent) 
{ 
    m_iPosX = 0;
    m_iPosY = 0;
    m_iWidth = 0;
    m_iHeight = 0;
    m_iDepth = 0;
    m_iDistence = 0;
    m_lstPercent.clear();
    m_lstSpanAgl.clear();
    m_lstStartAgl.clear();
    setFixedSize(200, 200);
} 

CDraw3dPieChart::~CDraw3dPieChart(void) 
{ 
    m_lstPercent.clear();
    m_lstSpanAgl.clear();
    m_lstStartAgl.clear();
} 

void CDraw3dPieChart::setColor(int PieSize, QColor DownPieColor[], QColor UpPieColor[])
{
    for(int i = 0; i < PieSize; i++)
    {
        cstDownPieColor[i] = DownPieColor[i];
        cstUpPieColor[i] = UpPieColor[i];
    }
}

void CDraw3dPieChart::setPiePos(int iPosX, int iPosY) 
{ 
    m_iPosX = iPosX;
    m_iPosY = iPosY;
} 

void CDraw3dPieChart::setPieSize(int iWidth, int iHeight) 
{ 
    m_iWidth = iWidth;
    m_iHeight = iHeight;
} 

void CDraw3dPieChart::setChartDepth(int iDepth) 
{ 
    m_iDepth = iDepth;
} 

void CDraw3dPieChart::setChartDistance(int iDistence) 
{ 
    m_iDistence = iDistence;
} 

void CDraw3dPieChart::setPiePerCent(QList<double> lstPercent) 
{ 
    m_lstPercent = lstPercent;
} 

void CDraw3dPieChart::paintEvent(QPaintEvent *event) 
{ 
    Q_UNUSED(event);
    QPainter painter(this);
    painter.begin(this);
    m_lstSpanAgl.clear();
    m_lstStartAgl.clear();
    m_lstStartAgl.append(0);

    if (m_lstPercent.size() <= 0)
    {
        return;
    }
    for (int i = 0; i < m_lstPercent.size(); i++)
    {
        m_lstSpanAgl.append(ceil(360*m_lstPercent.at(i)));
        if (i == 0)
        {
            continue;
        }
        m_lstStartAgl.append(m_lstSpanAgl.at(i-1) + m_lstStartAgl.at(i-1));
    }
    painter.setRenderHint(QPainter::Antialiasing, true); //设置平滑
    painter.setPen(Qt::NoPen);

    //下部分循环m_iDepth次
    for (int i = 0; i < m_iDepth; i++)
    {
        QRect rectDown(m_iPosX, m_iPosY-i, m_iWidth, m_iHeight);
        for (int j = 0; j < m_lstSpanAgl.size(); j++)
        {
            painter.setBrush(cstDownPieColor[j]);
            painter.drawPie(rectDown, m_lstStartAgl.at(j)*16, m_lstSpanAgl.at(j)*16);
        }
    }

    //最上部分画一次（不同颜色）
    QRect rectUp(m_iPosX, m_iPosY-m_iDepth, m_iWidth, m_iHeight);
    for (int i = 0; i < m_lstPercent.size(); i++)
    {
        painter.setBrush(cstUpPieColor[i]);
        painter.drawPie(rectUp, m_lstStartAgl.at(i)*16, m_lstSpanAgl.at(i)*16);
    }

    painter.end();
} 
