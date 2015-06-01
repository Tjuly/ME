#include <QWidget>
#include <QtGui>

class CDraw3dPieChart : public QWidget
{
    Q_OBJECT
    public:
    CDraw3dPieChart(QWidget *parent = 0);
    public:
    ~CDraw3dPieChart(void);
    public:
    void   setColor(int , QColor DownPieColor[], QColor UpPieColor[]);
    void   setPiePos(int iPosX, int iPosY);
    void   setPieSize(int iWidth, int iHeight);
    void   setPiePerCent(QList<double> lstPercent);
    void   setChartDepth(int iDepth);
    void   setChartDistance(int iDistence);
    void   paintEvent(QPaintEvent *event);

    private:
    int    m_iPosX, m_iPosY, m_iWidth, m_iHeight, m_iDepth, m_iDistence;
    QColor cstDownPieColor[7], cstUpPieColor[7];

    QList<double> m_lstPercent;
    QList<int>    m_lstSpanAgl;
    QList<int>    m_lstStartAgl;
}; 
