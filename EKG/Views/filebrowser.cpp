#include "filebrowser.h"
#include "ui_filebrowser.h"
#include <iostream>

fileBrowser::fileBrowser(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::fileBrowser)
{
    ui->setupUi(this);

    QString sPath = "D:/PROJEKT_EKG/Program/SampleData/";
    dirmod = new QFileSystemModel(this);
    dirmod->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
    dirmod->setRootPath(sPath);

    ui->treeView->setModel(dirmod);
    ui->treeView->resizeColumnToContents(0);
    ui->treeView->hideColumn(1);
    ui->treeView->hideColumn(2);
    ui->treeView->hideColumn(3);


    filemod = new QFileSystemModel(this);
    filemod->setFilter(QDir::NoDotAndDotDot | QDir::Files);
    filemod->setRootPath(sPath);


    ui->listView->setModel(filemod);
    ui->listView->setSelectionMode(QAbstractItemView::SingleSelection);

    QStringList filters;
    filters << "*.dat";
    filemod->setNameFilters(filters);

}

fileBrowser::~fileBrowser()
{
    delete ui;
}

void fileBrowser::on_treeView_clicked(const QModelIndex &index)
{
    QString sPath = dirmod->fileInfo(index).absoluteFilePath();
    ui->listView->setRootIndex(filemod->setRootPath(sPath));
}

void fileBrowser::on_pushButton_clicked()
{
    QString sName = filemod->fileInfo(ui->listView->selectionModel()->currentIndex()).baseName();
    QString sPath = filemod->fileInfo(ui->listView->selectionModel()->currentIndex()).absolutePath();

    emit fbLoadEntity(sPath,sName);

    this->close();

}

void fileBrowser::on_listView_doubleClicked(const QModelIndex &index)
{
    QString sName = filemod->fileInfo(index).baseName();
    QString sPath = filemod->fileInfo(index).absolutePath();

    emit fbLoadEntity(sPath,sName);

    this->close();
    //wypisywanie
    //std::cout << ">>> " << sName.toStdString() << " + " << sPath.toStdString()<< " <<<"<< std::endl<< std::endl;
}

