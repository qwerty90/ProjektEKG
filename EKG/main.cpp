#include <QApplication>
//#include <QFile>
//#include <QTextStream>
#include "Views/airecgmain.h"
#include "Controllers/app_controller.h"

//include do loggera - korzystajcie smialo
#include <QsLog.h>
#include <QDir>
#include <QsLogDest.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // init the logging mechanism
    QsLogging::Logger& logger = QsLogging::Logger::instance();
    const QString sLogPath(QDir(a.applicationDirPath()).filePath("log.txt"));
    // Create log destinations
    QsLogging::DestinationPtr fileDestination(
       QsLogging::DestinationFactory::MakeFileDestination(sLogPath) );
    logger.addDestination(fileDestination.get());
 // write an info message
    QLOG_INFO() << "Program Started";

    AirEcgMain w;
    app_controller *controller = new app_controller();

    //AppController *controller = new AppController();
    controller->BindView(&w);
    w.show();
    return a.exec();
}
