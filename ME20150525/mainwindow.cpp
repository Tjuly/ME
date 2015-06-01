#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "SVGlib/jojo.h"

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    InitStyle();
    InitControler();

    listItemForm *myList = new listItemForm();
    QListWidgetItem *item = new QListWidgetItem();
    ui->ImportlistWidget->insertItem(ui->ImportlistWidget->size().height(),item);
    item->setSizeHint(QSize(100,110));
    ui->ImportlistWidget->setItemWidget(item,myList);

//    myList = new listItemForm();
//    item = new QListWidgetItem();
//    ui->listWidget->insertItem(ui->listWidget->size().height(),item);
//    item->setSizeHint(QSize(100,110));
//    ui->listWidget->setItemWidget(item,myList);

//    myList = new listItemForm();
//    item = new QListWidgetItem();
//    ui->listWidget->insertItem(ui->listWidget->size().height(),item);
//    item->setSizeHint(QSize(100,110));
//    ui->listWidget->setItemWidget(item,myList);
//    myList = new listItemForm();
//    item = new QListWidgetItem();
//    ui->listWidget->insertItem(ui->listWidget->size().height(),item);
//    item->setSizeHint(QSize(100,110));
//    ui->listWidget->setItemWidget(item,myList);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::InitControler()
{
    //初始隐藏Dock窗体
    ui->Paintwidget_DockThreshold->hide();
    ui->Paintwidget_DockRotate->hide();
    ui->Paintwidget_Dock->hide();

    ui->widget_Button->hide();

    //设置按钮无效
    ui->Button_Cut->setEnabled(false);
    ui->Button_Clean->setEnabled(false);
    ui->Button_Reverse->setEnabled(false);
    ui->Button_Rotate->setEnabled(false);
    ui->Button_SaveBitmap->setEnabled(false);
    ui->Button_Recover->setEnabled(false);

    //激活MenuButton
    ActivateMenuButton();

    //新建绘制区
    Painterwidow = new PainterWidget();
    ui->Paintwidget_scrollAreaBitmap->setWidget(Painterwidow);

//    ImportForm *ImportWindow = new ImportForm();
//    ui->scrollArea->setWidget(ImportWindow);

    //新建矢量图载体
    SVGwindow = new SvgView;

    //关联slider控件与spinbox控件
    connect(ui->Paintwidget_spinBoxCanny,SIGNAL(valueChanged(int)),ui->Paintwidget_horizontalSliderCanny,SLOT(setValue(int)));
    connect(ui->Paintwidget_horizontalSliderCanny,SIGNAL(valueChanged(int)),ui->Paintwidget_spinBoxCanny,SLOT(setValue(int)));
    connect(ui->Paintwidget_spinBoxGauss,SIGNAL(valueChanged(int)),ui->Paintwidget_horizontalSliderGauss,SLOT(setValue(int)));
    connect(ui->Paintwidget_horizontalSliderGauss,SIGNAL(valueChanged(int)),ui->Paintwidget_spinBoxGauss,SLOT(setValue(int)));

    connect(ui->Paintwidget_horizontalSliderCanny,SIGNAL(valueChanged(int)),Painterwidow,SLOT(SetthresCanny(int)));
    connect(ui->Paintwidget_horizontalSliderGauss,SIGNAL(valueChanged(int)),Painterwidow,SLOT(SetthresGauss(int)));

    connect(ui->Paintwidget_spinBoxRotate,SIGNAL(valueChanged(int)),ui->Paintwidget_horizontalSliderRotate,SLOT(setValue(int)));
    connect(ui->Paintwidget_horizontalSliderRotate,SIGNAL(valueChanged(int)),ui->Paintwidget_spinBoxRotate,SLOT(setValue(int)));
    connect(ui->Paintwidget_horizontalSliderRotate,SIGNAL(valueChanged(int)),Painterwidow,SLOT(RotateImg(int)));


    //ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
}

void MainWindow::InitStyle()
{
    //设置窗体标题栏隐藏
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);
    location = this->geometry();
    max = false;
    mousePressed = false;

    //安装事件监听器,让标题栏识别鼠标双击
    ui->lab_Title->installEventFilter(this);

    IconHelper::Instance()->SetIcon(ui->btnMenu_Close, QChar(0xf00d), 10);
    IconHelper::Instance()->SetIcon(ui->btnMenu_Max, QChar(0xf096), 10);
    IconHelper::Instance()->SetIcon(ui->btnMenu_Min, QChar(0xf068), 10);
    IconHelper::Instance()->SetIcon(ui->btnMenu, QChar(0xf0c9), 10);
    IconHelper::Instance()->SetIcon(ui->lab_Ico, QChar(0xf015), 12);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonDblClick) {
        this->on_btnMenu_Max_clicked();
        return true;
    }
    return QObject::eventFilter(obj, event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
    if (mousePressed && (e->buttons() && Qt::LeftButton) && !max) {
        this->move(e->globalPos() - mousePoint);
        e->accept();
    }
}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        mousePressed = true;
        mousePoint = e->globalPos() - this->pos();
        e->accept();
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *)
{
    mousePressed = false;
}

void MainWindow::on_btnMenu_Close_clicked()
{
    qApp->exit();
}

void MainWindow::on_btnMenu_Max_clicked()
{
    if(max) {
        this->setGeometry(location);
        IconHelper::Instance()->SetIcon(ui->btnMenu_Max, QChar(0xf096), 10);
        ui->btnMenu_Max->setToolTip("最大化");
    } else {
        location = this->geometry();
        //this->setGeometry(qApp->desktop()->availableGeometry());
        this->showFullScreen();
        IconHelper::Instance()->SetIcon(ui->btnMenu_Max, QChar(0xf079), 10);
        ui->btnMenu_Max->setToolTip("还原");
    }
    max = !max;
}

void MainWindow::on_btnMenu_Min_clicked()
{
    this->showMinimized();
}

void MainWindow::ActivateMenuButton()
{
    MenuFile = new QMenu();
    MenuFile->setStyleSheet("font: 10pt");
    connect(MenuFile,SIGNAL(triggered(QAction *)),this,SLOT(MenuFile_Triggered(QAction *)));

    OpenFileAction = new QAction(QIcon(":/images/load.png"), tr("&打开图片"), this);
    OpenFileAction->setShortcuts(QKeySequence::Open);
    OpenFileAction->setStatusTip(tr("Open an existing file"));

    MenuFile->addAction(OpenFileAction);
    ui->MenuButton_File->setMenu(MenuFile);

    /**********************************************************************************************/

    MenuFunc = new QMenu();
    MenuFunc->setStyleSheet("font: 10pt");
    connect(MenuFunc,SIGNAL(triggered(QAction *)),this,SLOT(MenuFunc_Triggered(QAction *)));

    LineImageAction = new QAction(QIcon(":/images/load.png"), tr("&生成线描图"), this);
    LineImageAction->setShortcuts(QKeySequence::Bold);
    LineImageAction->setStatusTip(tr("Open an existing file"));

    SVGAction = new QAction(QIcon(":/images/load.png"), tr("&生成矢量图"), this);
    SVGAction->setShortcuts(QKeySequence::Back);
    SVGAction->setStatusTip(tr("Open an existing file"));

    MenuFunc->addAction(LineImageAction);
    MenuFunc->addAction(SVGAction);
    ui->MenuButton_Func->setMenu(MenuFunc);

    /**********************************************************************************************/

    MenuMDB = new QMenu();
    MenuMDB->setStyleSheet("font: 10pt");
    connect(MenuMDB,SIGNAL(triggered(QAction *)),this,SLOT(MenuMDB_Triggered(QAction *)));

    importFromDBAction = new QAction(QIcon(":/images/load.png"), tr("&导入素材"), this);
    importFromDBAction->setShortcuts(QKeySequence::Bold);
    importFromDBAction->setStatusTip(tr("Open an existing file"));

    exportToDBAction = new QAction(QIcon(":/images/load.png"), tr("&导出素材"), this);
    exportToDBAction->setShortcuts(QKeySequence::Back);
    exportToDBAction->setStatusTip(tr("Open an existing file"));

    MenuMDB->addAction(importFromDBAction);
    MenuMDB->addAction(exportToDBAction);
    ui->MenuButton_MDB->setMenu(MenuMDB);

}

void MainWindow::MenuFile_Triggered(QAction *action)
{
    if(action == OpenFileAction)
    {
        Painterwidow->loadFormFile();

        ui->widget_Button->show();

        //设置按钮有效
        ui->Button_Cut->setEnabled(true);
        ui->Button_Clean->setEnabled(true);
        ui->Button_Reverse->setEnabled(true);
        ui->Button_Rotate->setEnabled(true);
        ui->Button_SaveBitmap->setEnabled(true);
        ui->Button_Recover->setEnabled(true);

        cv::Mat mat_line = cv::Mat(Painterwidow->bufferImage.height(), Painterwidow->bufferImage.width(), CV_8UC4, (uchar*)Painterwidow->bufferImage.bits(), Painterwidow->bufferImage.bytesPerLine());

    }
}

void MainWindow::MenuFunc_Triggered(QAction *action)
{
    if(action == LineImageAction)
    {
        Painterwidow->Linedrawing(10, 20);
        ui->Paintwidget_Dock->show();
        ui->Paintwidget_DockThreshold->show();

        ui->Paintwidget_DockRotate->hide();

        ui->Button_Rotate->setEnabled(false);
        ui->Button_Cut->setEnabled(false);
    }
    else if(action == SVGAction)
    {
        ui->stackedWidget->setCurrentIndex(1);
        SVGwindow->openFile(QFile(FunctionSVG()));
        ui->SVGwidget_scrollArea->setWidget(SVGwindow);

    }
}

void MainWindow::MenuMDB_Triggered(QAction *action)
{
    if(action == importFromDBAction)
    {
        ui->stackedWidget->setCurrentIndex(2);

    }
    else if(action == exportToDBAction)
    {
        ui->stackedWidget->setCurrentIndex(3);
    }
}

//截图
void MainWindow::on_Button_Cut_clicked()
{
    Painterwidow->setCutAct();
}

//清除
void MainWindow::on_Button_Clean_clicked()
{
    Painterwidow->clear();

    //设置按钮无效
    ui->Button_Cut->setEnabled(false);
    ui->Button_Clean->setEnabled(false);
    ui->Button_Reverse->setEnabled(false);
    ui->Button_Rotate->setEnabled(false);
    ui->Button_SaveBitmap->setEnabled(false);
    ui->Button_Recover->setEnabled(false);

    ui->widget_Button->hide();

    ui->Paintwidget_Dock->hide();
    ui->Paintwidget_DockThreshold->hide();
    ui->Paintwidget_DockRotate->hide();
}

//反向
void MainWindow::on_Button_Reverse_clicked()
{
    Painterwidow->reverseImg();
}

//旋转
void MainWindow::on_Button_Rotate_clicked()
{
    ui->Paintwidget_horizontalSliderRotate->setValue(0);
    ui->Paintwidget_spinBoxRotate->setValue(0);
    ui->Paintwidget_Dock->show();
    ui->Paintwidget_DockRotate->show();
}

void MainWindow::on_Button_Recover_clicked()
{
    Painterwidow->RecoverImg();

    ui->Paintwidget_Dock->hide();
    ui->Paintwidget_DockRotate->hide();
    ui->Paintwidget_DockThreshold->hide();

    //设置按钮有效
    ui->Button_Cut->setEnabled(true);
    ui->Button_Clean->setEnabled(true);
    ui->Button_Reverse->setEnabled(true);
    ui->Button_Rotate->setEnabled(true);
    ui->Button_SaveBitmap->setEnabled(true);
    ui->Button_Recover->setEnabled(true);
}

QString MainWindow::FunctionSVG()
{
    JoJo jojo(3,3,100,0,0);
    cv::Mat ImgSmo, ImgOrg4,ImgBW4,ImgOrg3,ImgBW1;

    FILE *fout = fopen("D:/123.svg","wb");

    ImgBW4 = cv::Mat(Painterwidow->bufferImage.height(),Painterwidow->bufferImage.width(),CV_8UC4,(uchar*)Painterwidow->bufferImage.bits(),Painterwidow->bufferImage.bytesPerLine());
    ImgOrg4 = cv::Mat(Painterwidow->originalImage.height(),Painterwidow->originalImage.width(),CV_8UC4,(uchar*)Painterwidow->originalImage.bits(),Painterwidow->originalImage.bytesPerLine());

    cv::cvtColor(ImgOrg4,ImgOrg3,CV_BGRA2BGR);
    cv::cvtColor(ImgBW4,ImgBW1,CV_BGRA2GRAY);

    jojo.hiJotarou(ImgOrg3,ImgBW1,fout,"svg");
    fclose(fout);

    jojo.hiKujou(ImgSmo);
    imwrite("D:/123.bmp",ImgSmo);

    return QString("D:/123.svg");
}

void MainWindow::on_ImportlistWidget_itemDoubleClicked(QListWidgetItem *item)
{
    imgShowForm *wid = new imgShowForm;
    wid->ShowImg(Painterwidow->bufferImage);

    wid->setGeometry(ui->ImportlistWidget->mapToGlobal(QPoint(0, 0)).x(), ui->ImportlistWidget->mapToGlobal(QPoint(0, 0)).y(), ui->ImportlistWidget->width(), ui->ImportlistWidget->height());
    wid->setAttribute(Qt::WA_ShowModal, true);
    wid->show();
}

void MainWindow::on_Button_Back1_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_Button_Back2_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

//保存位图
void MainWindow::on_Button_SaveBitmap_clicked()
{
    Painterwidow->saveImage();
}

//保存矢量图
void MainWindow::on_Button_SaveSVG_clicked()
{

}

void MainWindow::on_Button_Back3_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

//导入素材
void MainWindow::on_Button_Import_clicked()
{

}

//导出素材
void MainWindow::on_Button_Export_clicked()
{

}
