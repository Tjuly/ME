#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QtGui>
#include <Iconhelper/iconhelper.h>
#include <Painterwidget/painterwidget.h>
#include <SVGview/svgview.h>
#include <BasicPro.h>
#include <listitemform.h>
#include <imgshowform.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QPoint mousePoint;
    bool mousePressed;
    bool max;
    QRect location;

    QMenu *MenuFile, *MenuFunc, *MenuMDB;
    QAction *OpenFileAction;
    QAction *LineImageAction, *SVGAction;
    QAction *importFromDBAction, *exportToDBAction;

    PainterWidget *Painterwidow;
    SvgView *SVGwindow;

    void InitControler();
    void InitStyle();
    void ActivateMenuButton();
    QString FunctionSVG();

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *);

private slots:
    void on_btnMenu_Close_clicked();
    void on_btnMenu_Min_clicked();
    void on_btnMenu_Max_clicked();
    void MenuFile_Triggered(QAction *action);
    void MenuFunc_Triggered(QAction *action);
    void MenuMDB_Triggered(QAction *action);
    void on_Button_Cut_clicked();
    void on_Button_Clean_clicked();
    void on_Button_SaveBitmap_clicked();
    void on_Button_Reverse_clicked();
    void on_Button_Rotate_clicked();
    void on_Button_Recover_clicked();
    void on_ImportlistWidget_itemDoubleClicked(QListWidgetItem *item);
    void on_Button_Back1_clicked();
    void on_Button_SaveSVG_clicked();
    void on_Button_Back2_clicked();
    void on_Button_Back3_clicked();
    void on_Button_Import_clicked();
    void on_Button_Export_clicked();
};

#endif // MAINWINDOW_H
