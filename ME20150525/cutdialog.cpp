#include "cutdialog.h"
#include "ui_cutdialog.h"

cutDialog::cutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::cutDialog)
{
    ui->setupUi(this);

    InitStyle();
}

cutDialog::~cutDialog()
{
    delete ui;
}

void cutDialog::InitStyle()
{
    //设置窗体标题栏隐藏
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);
    location = this->geometry();
    max = false;
    mousePressed = false;

    //安装事件监听器,让标题栏识别鼠标双击
    ui->lab_Title->installEventFilter(this);
}

bool cutDialog::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonDblClick) {
        //this->on_btnMenu_Max_clicked();
        return true;
    }
    return QObject::eventFilter(obj, event);
}

void cutDialog::mouseMoveEvent(QMouseEvent *e)
{
    if(mousePressed && (e->buttons() && Qt::LeftButton) && !max) {
        this->move(e->globalPos() - mousePoint);
        e->accept();
    }
}

void cutDialog::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        mousePressed = true;
        mousePoint = e->globalPos() - this->pos();
        e->accept();
    }
}

void cutDialog::mouseReleaseEvent(QMouseEvent *)
{
    mousePressed = false;
}
