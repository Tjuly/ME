#ifndef CUTDIALOG_H
#define CUTDIALOG_H

#include <QDialog>
#include <QtGui>
#include <Iconhelper/iconhelper.h>

namespace Ui {
class cutDialog;
}

class cutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit cutDialog(QWidget *parent = 0);
    ~cutDialog();

private:
    Ui::cutDialog *ui;
    QPoint mousePoint;
    bool mousePressed;
    bool max;
    QRect location;

    void InitStyle();

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *);
};

#endif // CUTDIALOG_H
