#ifndef APP_CONTROLLER_H
#define APP_CONTROLLER_H

#include "results.h"
#include "Views/airecgmain.h"

//include do loggera - korzystajcie smialo
#include <QsLog.h>
#include <QDir>
#include <QsLogDest.h>

class app_controller : public QObject
{
    Q_OBJECT
private:
    results AllData;
public:
    app_controller();

    void BindView(AirEcgMain *view);

public slots:
    void run();
    void loadData(const QString &directory, const QString &name);
};

#endif // APP_CONTROLLER_H
