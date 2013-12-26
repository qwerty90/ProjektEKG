#ifndef FILEBROWSER_H
#define FILEBROWSER_H

#include <QDialog>
#include <QtCore>
#include <QtGui>

namespace Ui {
class fileBrowser;
}

class fileBrowser : public QDialog
{
    Q_OBJECT
    
public:
    explicit fileBrowser(QWidget *parent = 0);
    ~fileBrowser();

signals:
    void fbLoadEntity(const QString &directory, const QString &name);

private slots:
    void on_treeView_clicked(const QModelIndex &index);

    void on_pushButton_clicked();

    void on_listView_doubleClicked(const QModelIndex &index);

private:
    Ui::fileBrowser *ui;
    QFileSystemModel *dirmod;
    QFileSystemModel *filemod;
};

#endif // FILEBROWSER_H

