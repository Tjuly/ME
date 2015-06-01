#ifndef IMGSHOWFORM_H
#define IMGSHOWFORM_H

#include <QWidget>
#include <QtGui>
#include <Painterwidget/ShowWidget.h>

namespace Ui {
class imgShowForm;
}

class imgShowForm : public QWidget
{
    Q_OBJECT

public:
    explicit imgShowForm(QWidget *parent = 0);
    imgShowForm(QImage SrcImg);
    ~imgShowForm();

    void ShowImg(QImage SrcImg);


private:
    Ui::imgShowForm *ui;

    ShowWidget *ShowWindow;

protected:
    void mouseDoubleClickEvent(QMouseEvent*e);

};

#endif // IMGSHOWFORM_H
