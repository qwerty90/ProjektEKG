#include <QApplication>
#include "Views/airecgmain.h"
#include "Controllers/appcontroller.h"

//include do loggera - korzystajcie smialo
#include <QsLog.h>
#include <QDir>
#include <QsLogDest.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // init the logging mechanism
    QsLogging::Logger& logger = QsLogging::Logger::instance();

    // set minimum log level and file name
    logger.setLoggingLevel(QsLogging::TraceLevel);
    const QString sLogPath(QDir(a.applicationDirPath()).filePath("log.txt"));

    // Create log destinations
    QsLogging::DestinationPtr fileDestination(
                QsLogging::DestinationFactory::MakeFileDestination(sLogPath) );
    QsLogging::DestinationPtr debugDestination(
                QsLogging::DestinationFactory::MakeDebugOutputDestination() );

    // set log destinations on the logger
    logger.addDestination(debugDestination.get());
    logger.addDestination(fileDestination.get());

    // write an info message
    QLOG_INFO() << "Program started";

    AirEcgMain w;
    AppController *controller = new AppController();
    controller->BindView(&w);
    w.show();
    return a.exec();
}
