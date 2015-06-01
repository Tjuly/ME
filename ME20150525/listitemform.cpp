#include "listitemform.h"
#include "ui_listitemform.h"

listItemForm::listItemForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::listItemForm)
{
    ui->setupUi(this);
}

listItemForm::~listItemForm()
{
    delete ui;
}
