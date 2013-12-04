#include "mainwindow.h"
#include "data_input.h"
#include <QApplication>
#include <QFile>
#include <QTextStream>

int main(int argc, char *argv[])
{
    data_input obiekt;
    obiekt.setDataFile();
    obiekt.LoadData();
    obiekt.LoadAnnotations();
    obiekt.LoadNotes();
    QFile f("plik.txt");
    f.open(QIODevice::WriteOnly);
    QTextStream out(&f);
    out << QString::number(obiekt.age);
    out << endl;
    QList<QString>::Iterator i;
    for(i=obiekt.time.begin();i != obiekt.time.end();i++)
    {
        QString s = (*i);
        out << s ;
        out << endl;
    }
    f.close();


    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
