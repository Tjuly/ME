#include "imgshowform.h"
#include "ui_imgshowform.h"

imgShowForm::imgShowForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::imgShowForm)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);

    //setWindowOpacity(0.9);
    setPalette(QPalette(QColor(240, 240, 240))); //画板背景色初始化为白色
}

void imgShowForm::ShowImg(QImage SrcImg)
{
    ShowWindow = new ShowWidget;
    ShowWindow->RecImg(SrcImg);

    ui->scrollArea->setWidget(ShowWindow);
}

void imgShowForm::mouseDoubleClickEvent(QMouseEvent *e)
{
    this->close();
}

imgShowForm::~imgShowForm()
{
    delete ui;
}
