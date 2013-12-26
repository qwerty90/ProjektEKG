#include "processing.h"
#include "ui_processing.h"

processing::processing(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::processing)
{
    ui->setupUi(this);
}

processing::~processing()
{
    delete ui;
}
