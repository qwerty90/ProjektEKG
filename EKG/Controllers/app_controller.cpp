#include "app_controller.h"
#include <QObject>

app_controller::app_controller()
{

}

void app_controller::run()
{

    QLOG_INFO() << "\"Run\" pushbutton hit";
}

void app_controller::BindView(AirEcgMain *view)
{
    this->connect(view, SIGNAL(loadEntity(QString,QString)), this, SLOT(loadData(QString,QString)));
    this->connect(view, SIGNAL(run()                      ), this, SLOT(run()                    ));
}

void app_controller::loadData(const QString &directory, const QString &name) //sygnal
{
    this->AllData.raw_data.LoadAllData(name,directory);
}
