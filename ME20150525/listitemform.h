#ifndef LISTITEMFORM_H
#define LISTITEMFORM_H

#include <QWidget>

namespace Ui {
class listItemForm;
}

class listItemForm : public QWidget
{
    Q_OBJECT

public:
    explicit listItemForm(QWidget *parent = 0);
    ~listItemForm();

private:
    Ui::listItemForm *ui;
};

#endif // LISTITEMFORM_H
