#include "airecgmain.h"
#include "ui_airecgmain.h"
#include "about.h"
#include "filebrowser.h"
#include "notready.h"
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_magnifier.h>
#include <qwt_abstract_slider.h>
#include <qwt_symbol.h>
#include <qwt_scale_engine.h>
#include <qwt_legend.h>
#include <QSignalMapper>
#include <cmath>
#include <qapplication.h>
#include <qlayout.h>
#include <qwt_plot_spectrocurve.h>
#include <QVector>
#include <qwt_point_3d.h>
#include <qwt_color_map.h>
#include <qwt_plot_marker.h>



AirEcgMain::AirEcgMain(QWidget *parent) :
    QMainWindow(parent),
    baselineSignalMapper(new QSignalMapper(this)),
    ui(new Ui::AirEcgMain)
{
    ui->setupUi(this);

    this->qrsClassPlot = NULL;
    this->currentQrsClassPlot = NULL;
    connect(ui->QRSClassesToolBox, SIGNAL(currentChanged(int)),this,SLOT(on_qrsclassestoolbox_changed(int)));
    connect(ui->qrsClassLabelTextEdit, SIGNAL(textChanged(QString)),this,SLOT(qrcclasslabel_changed(QString)));

    connect(ui->butterworthRadioButton, SIGNAL(clicked()), baselineSignalMapper, SLOT(map()));
    connect(ui->movingAverageRadioButton, SIGNAL(clicked()), baselineSignalMapper, SLOT(map()));
    connect(ui->savitzkyGolayRadioButton, SIGNAL(clicked()), baselineSignalMapper, SLOT(map()));
    baselineSignalMapper->setMapping(ui->butterworthRadioButton, 0);
    baselineSignalMapper->setMapping(ui->movingAverageRadioButton, 1);
    baselineSignalMapper->setMapping(ui->savitzkyGolayRadioButton, 2);
    connect(baselineSignalMapper, SIGNAL(mapped(int)), SIGNAL(switchEcgBaseline(int)));

    ui->stackedWidget->setCurrentIndex(1);

    QShortcut* shortcut = new QShortcut(QKeySequence("Ctrl+O"),this);
    shortcut->setContext(Qt::ApplicationShortcut);
    connect(shortcut,SIGNAL(activated()), ui->actionWczytaj,SLOT(click()));

    // Hide unused controls
    ui->label_10->setVisible(false);
    ui->qrsSettingsGMeansParallelCheckBox->setVisible(false);
    ui->qrsFeaturesSettingsGroupBox->setVisible(false);
    ui->QRSSampleDataGroupBox->setVisible(false);
    ui->progressBar->setVisible(false);
}


AirEcgMain::~AirEcgMain()
{
    delete ui;
}
void AirEcgMain::on_actionO_programie_triggered()
{
    about dialogAbout;
    dialogAbout.setModal(true);
    dialogAbout.exec();
}
void AirEcgMain::on_actionWczytaj_triggered()
{
    ui->progressBar->setVisible(true);
    fileBrowser dialogFileBrowser;
    this->connect(&dialogFileBrowser, SIGNAL(fbLoadEntity(QString,QString)), this, SLOT(fbLoadData(QString,QString)));
    dialogFileBrowser.setModal(true);
    dialogFileBrowser.exec();
    ui->progressBar->setVisible(false);
}



void AirEcgMain::fbLoadData(const QString &directory, const QString &name)
{
    emit loadEntity(directory, name);
    ui->pushButton_2->setEnabled(true);
   // ui->pushButton_3_2->setEnabled(true);
    ui->pushButton_5->setEnabled(true);
    ui->pushButton_6->setEnabled(true);
    ui->pushButton_7->setEnabled(true);
    ui->pushButton_8->setEnabled(true);
    ui->pushButton_9->setEnabled(true);
    ui->pushButton_10->setEnabled(true);
    ui->pushButton_12->setEnabled(true);
   ui->rpeaksGroupBox_2->setEnabled(true);
    ui->baselineGroupBox->setEnabled(true);
    ui->qrsClustererSettingsGroupBox->setEnabled(true);
    ui->qrsClustererSettingsGroupBox->setToolTip("");
    ui->qrsFeaturesSettingsGroupBox->setEnabled(true);
    ui->qrsFeaturesSettingsGroupBox->setToolTip("");
}

void AirEcgMain::receivePatientData(EcgData *data)
{
    EcgInfo *info = data->info;
    ui->patientSexLabel_2->setText(info->sex);
    ui->patientAgeLabel_2->setText(QString::number(info->age));
    ui->patientMedicinesLabel_2->setWordWrap(1);
    ui->patientMedicinesLabel_2->setText(info->medicines);
    ui->ecgSampleFrequencyLabel_2->setText(info->samplingFreq);
    ui->ecgSampleLengthLabel_2->setText(info->signalLength);


    QwtPlot *plot1 = plotPlot(*(data->primary),info->frequencyValue);
    ui->scrollAreaMLII->setWidget(plot1);
    ui->scrollAreaMLII->show();

    QwtPlot *plot2 = plotPlot(*(data->secondary),info->frequencyValue);
    ui->scrollAreaVI->setWidget(plot2);
    ui->scrollAreaVI->show();

    ui->tabWidget_3->setTabText(0, data->info->primaryName);
    ui->tabWidget_3->setTabText(1, data->info->secondaryName);

    ui->radioButton->setText(data->info->primaryName);
    ui->radioButton_2->setChecked(true);
    ui->radioButton_2->setText(data->info->secondaryName);

    ui->radioButton_3->setText(data->info->primaryName);
    ui->radioButton_4->setChecked(true);
    ui->radioButton_4->setText(data->info->secondaryName);

    ui->groupBox_2->setEnabled(true);

    ui->groupBox_11->setEnabled(true);

}

void AirEcgMain::on_Hilbert_radiobutton_clicked()
{
    emit this->switchRPeaks(1);
}
void AirEcgMain::on_PanTompkins_radiobutton_clicked()
{
    emit this->switchRPeaks(2);
}

void AirEcgMain::on_qrsclassestoolbox_changed(int index)
{
    if (this->qrsClassPlot != NULL)
        delete this->qrsClassPlot;
    this->qrsClassPlot = new QwtPlot();

    QString tmpText = ui->QRSClassesToolBox->itemText(index);
    QString label = "";

    for(int i = 0 ; i < tmpText.count(); i++)
    {
        if (tmpText.at(i) != QChar('['))
            label.append(tmpText.at(i));
        else
            break;
    }

    ui->qrsClassLabelTextEdit->setText(label);

    QwtLegend* legend = new QwtLegend();
    legend->setItemMode(QwtLegend::ReadOnlyItem);
    this->qrsClassPlot->insertLegend(legend, QwtPlot::BottomLegend);
    QwtText xaxis("Time [ms]");
    QwtText yaxis("Voltage [mV]");
    xaxis.setFont(QFont("Arial", 8));
    yaxis.setFont(QFont("Arial", 8));
    this->qrsClassPlot->setAxisTitle( QwtPlot::yLeft, yaxis );
    this->qrsClassPlot->setAxisTitle( QwtPlot::xBottom, xaxis );
    ui->ClassesPlotLayout->addWidget(this->qrsClassPlot);
    emit qrsClassChanged(index,1);
}

void AirEcgMain::qrssample_changed(QString text)
{
    QString sampleNo = "";

    for(int i = 7 ; i < text.count() ; i++)
    {
        sampleNo.append(text.at(i));
    }

    int index = sampleNo.toInt();

    if (this->qrsClassPlot != NULL)
    {
        this->qrsClassPlot->detachItems(QwtPlotItem::Rtti_PlotCurve,false);
        delete this->qrsClassPlot;
    }

    this->qrsClassPlot = new QwtPlot();

    QwtLegend* legend = new QwtLegend();
    legend->setItemMode(QwtLegend::ReadOnlyItem);
    this->qrsClassPlot->insertLegend(legend, QwtPlot::BottomLegend);

    QwtText xaxis("Time [ms]");
    QwtText yaxis("Voltage [mV]");
    xaxis.setFont(QFont("Arial", 8));
    yaxis.setFont(QFont("Arial", 8));
    this->qrsClassPlot->setAxisTitle( QwtPlot::yLeft, yaxis );
    this->qrsClassPlot->setAxisTitle( QwtPlot::xBottom, xaxis );

    ui->ClassesPlotLayout->addWidget(this->qrsClassPlot);
    emit qrsClassChanged(index,2);
}

void AirEcgMain::qrcclasslabel_changed(QString value)
{
    int currentIndex = ui->QRSClassesToolBox->currentIndex();
    QString currentLabel = ui->QRSClassesToolBox->itemText(currentIndex);
    value.append(" ");

    bool start = false;
    for(int i = 0 ; i < currentLabel.count(); i++)
    {
        if (currentLabel.at(i) == QChar('['))
            start = true;
        if (start)
            value.append(currentLabel.at(i));
    }

    ui->QRSClassesToolBox->setItemText(currentIndex,value);
}

QwtPlot* AirEcgMain::plotPlot(QList<int> &y,float freq){

    QVector<int> yData = QVector<int>::fromList(y);
    QVector<double> yDataFin = QVector<double>(yData.size());
    QVector<double> sampleNo = QVector<double>(yData.size());


    float max=yData.first();
    float min=yData.first();

    double tos=1/freq;

    for (int i=0;i<yData.size();++i)
    {
        sampleNo[i]=(i)*tos;
        yDataFin[i]=yData[i]/200.0;
        if (max<yData[i]) max=yData[i];
        if (min>yData[i]&&min>0) min=yData[i];
    }
    max/=200;
    min/=200;

    QwtPlot *plot = new QwtPlot();

    plot->setCanvasBackground( Qt::white );
    plot->setAxisScale( QwtPlot::yLeft, min, max,0.5 );

    plot->setAxisScale( QwtPlot::xBottom , 0.0,4.0,1);// sampleNo.last());

    QList<double> ticks;
    for(int i=0; i<100;i++)
    {
        ticks.append(i);
    }

    /* wstawianie lini pionowych
    QwtPlotMarker *mX = new QwtPlotMarker();
    mX->setLabel( QString::fromLatin1( "label" ) );
    mX->setLabelAlignment( Qt::AlignLeft | Qt::AlignBottom );
    mX->setLabelOrientation( Qt::Vertical );
    mX->setLineStyle( QwtPlotMarker::VLine );
    mX->setLinePen( QPen( Qt::black, 0, Qt::DashDotLine ) );
    mX->setXValue( 2.0);
    mX->attach( plot );
    */

    /* linia laczaca 2pkty
    QwtPlotCurve *curve2 = new QwtPlotCurve();
    curve2->setPen(QPen( Qt::blue, 3));
    curve2->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    QwtSymbol *symbol = new QwtSymbol( QwtSymbol::Ellipse,
                                      QBrush( Qt::yellow ), QPen( Qt::red, 2 ), QSize( 8, 8 ) );
    curve2->setSymbol( symbol );
    QPolygonF points;
    points << QPointF( 0.0, 4.4 ) << QPointF( 1.0, 3.0 );
    curve2->setSamples( points );
    curve2->attach( plot );
    */

    /* 3d example
        double pi = 3.14;
        double dr = 0.01;
        double d_angl = 0.0025;
        double r_max = 0.5;
        QVector<QwtPoint3D> *vector = new QVector<QwtPoint3D>;
        QwtPlotSpectroCurve *curve = new QwtPlotSpectroCurve;


        curve->setColorRange(QwtInterval(0, r_max));
        for(double r = dr; r < r_max; r += dr) {
         for(double angl = 0; angl < 2*pi; angl += d_angl) {
          vector->append(QwtPoint3D(4+0.5*r*sin(angl), 4+r*sin(angl), r));
         }
        }
        curve->setSamples(*vector);
    */

    QwtText xaxis("Time [ss]");
    QwtText yaxis("Voltage [mV]");
    xaxis.setFont(QFont("Arial", 8));
    yaxis.setFont(QFont("Arial", 8));

    plot->setAxisTitle( QwtPlot::yLeft, yaxis );
    plot->setAxisTitle( QwtPlot::xBottom, xaxis );

    QwtPlotGrid *grid = new QwtPlotGrid();
    QPen *pen = new QPen;

    QColor kolor = QColor();
    kolor.setAlpha(127);
    kolor.setBlue(0);
    kolor.setRed(255);
    kolor.setBlue(0);
    pen->setColor(kolor);
    grid->setPen( *pen );
    grid->attach( plot );

    QwtPlotCurve *curve = new QwtPlotCurve();

    curve->setPen(QPen( Qt::blue, 1));
    curve->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    curve->setSamples(sampleNo,yDataFin);
    curve->attach( plot );

    plot->resize( 1150, 500 );

    plot->canvas()->setFrameStyle( QFrame::Box | QFrame::Plain );
    plot->canvas()->setLineWidth( 1 );
    plot->canvas()->setGeometry(0,0,sampleNo.last(),0);

    zoom = new ScrollZoomer(plot->canvas());
    zoom->setRubberBandPen(QPen(Qt::white));
    zoom->setZoomBase(plot->canvas()->rect());
    zoom->setZoomStack(zoom->zoomStack(), 100);

    return plot;
}

QwtPlot* AirEcgMain::plotPlot(const QVector<double>& yData, float freq)
{
    QVector<double> sampleNo = QVector<double>(yData.size());

    double max = yData.first();
    double min = yData.first();

    double tos=1/freq;

    for (int i = 0; i < yData.size(); ++i)
    {
        sampleNo[i] = i*tos;
        max = qMax(max, yData[i]);
        min = qMin(min, yData[i]);
    }

    QwtPlot* plot = new QwtPlot();
    plot->setCanvasBackground(Qt::white);
    plot->setAxisScale(QwtPlot::yLeft, min, max);
    plot->setAxisScale( QwtPlot::xBottom , 0, 4.0);

    QwtText xaxis("Time [s]");
    QwtText yaxis("Voltage [mV]");
    xaxis.setFont(QFont("Arial", 8));
    yaxis.setFont(QFont("Arial", 8));

    plot->setAxisTitle( QwtPlot::yLeft, yaxis );
    plot->setAxisTitle( QwtPlot::xBottom, xaxis );

    QwtPlotGrid* grid = new QwtPlotGrid();
    grid->setPen(QPen(QColor(255, 0, 0 ,127)));
    grid->attach(plot);

    QwtPlotCurve* curve = new QwtPlotCurve();
    curve->setPen(QPen(Qt::blue, 1));
    curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    curve->setSamples(sampleNo, yData);
    curve->attach(plot);

    /*
    QwtPlotZoomer* zoomer = new QwtPlotZoomer(plot->canvas());
    zoomer->setRubberBandPen(QColor(Qt::black));
    zoomer->setTrackerPen(QColor(Qt::black));
    zoomer->setMousePattern(QwtEventPattern::MouseSelect2,
        Qt::RightButton, Qt::ControlModifier);
    zoomer->setMousePattern(QwtEventPattern::MouseSelect3,
        Qt::RightButton);
*/
    zoom = new ScrollZoomer(plot->canvas());
    zoom->setRubberBandPen(QPen(Qt::white));
    //zoom->setZoomBase( false );
    plot->canvas()->setGeometry(0,0,sampleNo.last(),0);
    zoom->setZoomBase(plot->canvas()->rect());

    QwtPlotPanner* panner = new QwtPlotPanner(plot->canvas());
    panner->setMouseButton(Qt::MidButton);
    panner->setOrientations(Qt::Horizontal);

    QwtPlotMagnifier* magnifier = new QwtPlotMagnifier(plot->canvas());
    magnifier->setAxisEnabled(QwtPlot::yLeft, false);

    return plot;
}

QwtPlot* AirEcgMain::plotHrt(QList<double>& y)
{
    QVector<double> yData = y.toVector();
    QVector<double> sampleNo = QVector<double>(y.size());

    double max = yData.first();
    double min = yData.first();

    for (int i = 0; i < yData.size(); ++i)
    {
        sampleNo[i] = i;
        max = qMax(max, yData[i]);
        min = qMin(min, yData[i]);
    }

    QwtPlot* plot = new QwtPlot();
    plot->setCanvasBackground(Qt::white);
    plot->setAxisScale(QwtPlot::yLeft, min, max);
    plot->setAxisScale( QwtPlot::xBottom , 0, 4.0);

    QwtText xaxis("# RR interval");
    QwtText yaxis("Time [ms]");
    xaxis.setFont(QFont("Arial", 8));
    yaxis.setFont(QFont("Arial", 8));

    plot->setAxisTitle( QwtPlot::yLeft, yaxis );
    plot->setAxisTitle( QwtPlot::xBottom, xaxis );

    QwtPlotGrid* grid = new QwtPlotGrid();
    grid->setPen(QPen(QColor(255, 0, 0 ,127)));
    grid->attach(plot);

    QwtPlotCurve* curve = new QwtPlotCurve();
    curve->setPen(QPen(Qt::blue, 1));
    curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    curve->setSamples(sampleNo, yData);
    curve->attach(plot);
/*
    QwtPlotZoomer* zoomer = new QwtPlotZoomer(plot->canvas());
    zoomer->setRubberBandPen(QColor(Qt::black));
    zoomer->setTrackerPen(QColor(Qt::black));
    zoomer->setMousePattern(QwtEventPattern::MouseSelect2,
        Qt::RightButton, Qt::ControlModifier);
    zoomer->setMousePattern(QwtEventPattern::MouseSelect3,
        Qt::RightButton);
*/
    zoom = new ScrollZoomer(plot->canvas());
    zoom->setRubberBandPen(QPen(Qt::white));
    //zoom->setZoomBase( false );
    plot->canvas()->setGeometry(0,0,sampleNo.last(),0);
    zoom->setZoomBase(plot->canvas()->rect());

    QwtPlotPanner* panner = new QwtPlotPanner(plot->canvas());
    panner->setMouseButton(Qt::MidButton);
    panner->setOrientations(Qt::Horizontal);

    QwtPlotMagnifier* magnifier = new QwtPlotMagnifier(plot->canvas());
    magnifier->setAxisEnabled(QwtPlot::yLeft, false);

    return plot;
}

QwtPlot* AirEcgMain::plotLogPlot(QList<double> &x,QList<double> &y,int rodzaj){
    QVector<double> xData = QVector<double>::fromList(x);
    QVector<double> yData = QVector<double>::fromList(y);

    int maxy=yData.first();
    int miny=yData.first();
    int maxx=xData.first();
    int minx=xData.first();

    for (int i=0;i<yData.size();++i)
    {
        if (maxy<yData[i]) maxy=yData[i];
        if (miny>yData[i]) miny=yData[i];

        if (maxx<xData[i]) maxx=xData[i];
        if (minx>xData[i]) minx=xData[i];
    }

    QwtPlot *plot = new QwtPlot();
    plot->setCanvasBackground( Qt::white );
    plot->setAxisScale( QwtPlot::yLeft, miny, maxy );
    plot->setAxisScale( QwtPlot::xBottom , minx, maxx);

    QwtPlotGrid *grid = new QwtPlotGrid();
    QPen *pen = new QPen;

    QColor kolor = QColor();
    kolor.setAlpha(127);
    kolor.setBlue(0);
    kolor.setRed(255);
    kolor.setBlue(0);
    pen->setColor(kolor);
    grid->setPen( *pen );
    grid->attach( plot );

    QwtPlotCurve *curve = new QwtPlotCurve();
    curve->setPen(QPen( Qt::blue, 1));
    curve->setRenderHint( QwtPlotItem::RenderAntialiased, true );

    curve->setSamples(xData,yData);
    curve->attach( plot );

    QwtPlotZoomer* zoomer = new QwtPlotZoomer( plot->canvas() );
    zoomer->setRubberBandPen( QColor( Qt::black ) );
    zoomer->setTrackerPen( QColor( Qt::black ) );
    zoomer->setMousePattern( QwtEventPattern::MouseSelect2,
        Qt::RightButton, Qt::ControlModifier );
    zoomer->setMousePattern( QwtEventPattern::MouseSelect3,
        Qt::RightButton );

    QwtPlotPanner *panner = new QwtPlotPanner( plot->canvas() );
    panner->setMouseButton( Qt::MidButton );
    panner->setOrientations(Qt::Horizontal);
    QwtPlotMagnifier *magnifier = new QwtPlotMagnifier(plot->canvas() );
    magnifier->setAxisEnabled(QwtPlot::yLeft,false);

    if (rodzaj == 1) {
        plot->setAxisScaleEngine( QwtPlot::xBottom, new QwtLog10ScaleEngine );
    } else if (rodzaj == 2){
        plot->setAxisScaleEngine( QwtPlot::yLeft, new QwtLog10ScaleEngine );
    } else if (rodzaj == 3){
        plot->setAxisScaleEngine( QwtPlot::yLeft, new QwtLog10ScaleEngine );
        plot->setAxisScaleEngine( QwtPlot::xBottom, new QwtLog10ScaleEngine );
    } else {
        plot->setAxisScaleEngine( QwtPlot::xBottom, new QwtLog10ScaleEngine );
    }

    return plot;
}
// hrv1
QwtPlot* AirEcgMain::plotPointsPlotDoubleToDouble(QList<double> &x, QList<double> &y){
    QVector<double> yData = QVector<double>::fromList(y);
    QVector<double> xData = QVector<double>::fromList(x);
    QVector<double> yDataFin = QVector<double>(yData.size());
    QVector<double> sampleNo = QVector<double>(yData.size());


    double maxX=xData.first();
    double minX=xData.first();
    for (int i=0;i<xData.size();++i)
    {
        sampleNo[i]=xData[i];
        if (maxX<xData[i])
        {
            maxX=xData[i];
            //qDebug() << "!!!!!!!!!!!!!!!!!";
            //qDebug() << maxX;
            //qDebug() << xData[i];
        }
        if (minX>xData[i]) minX=xData[i];
    }

    double maxY=yData.first();
    double minY=yData.first();
    for (int i=0;i<yData.size();++i)
    {
      //  sampleNo[i]=(i);
        yDataFin[i]=yData[i];
        if (maxY<yData[i]) maxY=yData[i];
        if (minY>yData[i]) minY=yData[i];
    }

    QwtPlot *plot = new QwtPlot();
    plot->setCanvasBackground( Qt::white );
    plot->setAxisScale( QwtPlot::yLeft, minY, maxY );
    //plot->setAxisScale( QwtPlot::xBottom , 0, yData.size());
    plot->setAxisScale( QwtPlot::xBottom , minX, maxX);

    QwtPlotGrid *grid = new QwtPlotGrid();
    QPen *pen = new QPen;

    QColor kolor = QColor();
    kolor.setAlpha(127);
    kolor.setBlue(0);
    kolor.setRed(255);
    kolor.setBlue(0);
    pen->setColor(kolor);
    grid->setPen( *pen );
    grid->attach( plot );

    QwtPlotCurve *curve = new QwtPlotCurve();
    curve->setPen(QPen( Qt::blue, 1));
    curve->setRenderHint( QwtPlotItem::RenderAntialiased, true );

    curve->setSamples(sampleNo,yDataFin);
    curve->attach( plot );

    // ponizszy zakomentowany kod odpowiada za MARKERY do zaznaczania
    // r_peaks lub innych punktow charakterystycznych

    QVector<double> pData = QVector<double>::fromList(x);
    QVector<double> pDataX = QVector<double>(pData.size());
    QVector<double> pDataY = QVector<double>(pData.size());

    for (int i=0;i<pData.size();++i)
    {
        pDataX[i]=pData[i];
        pDataY[i]=yData[pDataX[i]];
    }

    QwtPlotCurve *points = new QwtPlotCurve();
    QwtSymbol *marker = new QwtSymbol( QwtSymbol::Ellipse, Qt::red, QPen( Qt::red ), QSize( 5, 5 ) );
    points->setSymbol(marker);
    points->setPen( QColor( Qt::red ) );
    points->setStyle( QwtPlotCurve::NoCurve );
    //points->setSamples(pDataX,pDataY);
    points->setSamples(xData,yData);
    points->attach( plot );

    //

    QwtPlotZoomer* zoomer = new QwtPlotZoomer( plot->canvas() );
    zoomer->setRubberBandPen( QColor( Qt::black ) );
    zoomer->setTrackerPen( QColor( Qt::black ) );
    zoomer->setMousePattern( QwtEventPattern::MouseSelect2,
        Qt::RightButton, Qt::ControlModifier );
    zoomer->setMousePattern( QwtEventPattern::MouseSelect3,
        Qt::RightButton );

    QwtPlotPanner *panner = new QwtPlotPanner( plot->canvas() );
    panner->setMouseButton( Qt::MidButton );
    panner->setOrientations(Qt::Horizontal);
    QwtPlotMagnifier *magnifier = new QwtPlotMagnifier(plot->canvas() );
    magnifier->setAxisEnabled(QwtPlot::yLeft,false);
    return plot;
}


//funkcja przyjmuje jako parametry, wartosci RR i ich wystepienia, wspolrzedne
//rownomiernego rozmieszczenia danych sprobkowanych z funkcji interpolujacej
//oraz wartosci w tych wspolrzednych, sa to punkty wykorzystywane potem do FFT
QwtPlot* AirEcgMain::plotPoints(QList<double> &x, QList<double> &y, QList<double> fftSamplesX,
                       QList<double> fftSamplesY, QList<double> interpolateX, QList<double> interpolateY)
{
    // wszystkie QListy musza byc przetworzone na QVectory
    QVector<double> xData = QVector<double>::fromList(x);
    QVector<double> yData = QVector<double>::fromList(y);
    QVector<double> fftXData = QVector<double>::fromList(fftSamplesX);
    QVector<double> fftYData = QVector<double>::fromList(fftSamplesY);
    QVector<double> interpolateXData = QVector<double>::fromList(interpolateX);
    QVector<double> interpolateYData = QVector<double>::fromList(interpolateY);

    QVector<double> yDataFin = QVector<double>(yData.size());
    QVector<double> sampleNo = QVector<double>(yData.size());


    // ograniczenia na X
    double maxX=xData.first();
    double minX=xData.first();
    for (int i=0;i<xData.size();++i)
    {
        sampleNo[i]=xData[i];
        if (maxX<xData[i])
        {
            maxX=xData[i];
        }
        if (minX>xData[i]) minX=xData[i];
    }

    // ograniczenia na Y
    double maxY=yData.first();
    double minY=yData.first();
    for (int i=0;i<yData.size();++i)
    {
        yDataFin[i]=yData[i];
        if (maxY<yData[i]) maxY=yData[i];
        if (minY>yData[i]) minY=yData[i];
    }


    QwtPlot *plot = new QwtPlot();
    plot->setCanvasBackground( Qt::white );
    plot->setAxisScale( QwtPlot::yLeft, minY, maxY );
    //plot->setAxisScale( QwtPlot::xBottom , 0, yData.size());
    plot->setAxisScale( QwtPlot::xBottom , minX, maxX);

    //labels
    QwtText xaxis("Time [s]");
    QwtText yaxis("Time [ms]");
    xaxis.setFont(QFont("Arial", 8));
    yaxis.setFont(QFont("Arial", 8));
    plot->setAxisTitle( QwtPlot::yLeft, yaxis );
    plot->setAxisTitle( QwtPlot::xBottom, xaxis );


    // ustawianie siatki
    QwtPlotGrid *grid = new QwtPlotGrid();
    QPen *pen = new QPen;
    QColor kolor = QColor();
    kolor.setAlpha(127);
    kolor.setBlue(0);
    kolor.setRed(255);
    kolor.setBlue(0);
    pen->setColor(kolor);
    grid->setPen( *pen );
    grid->attach( plot );
    //********************

    QwtPlotCurve *curve = new QwtPlotCurve();
    curve->setPen(QPen( Qt::blue, 1));
    curve->setRenderHint( QwtPlotItem::RenderAntialiased, true );

    curve->setSamples(sampleNo,yDataFin);
    curve->attach( plot );

    // ponizszy kod odpowiada za MARKERY do zaznaczania r_peaks lub innych punktow charakterystycznych

    // wyrysowanie pkt funkcji interpolujacej
    QwtPlotCurve *interpolatedPoints = new QwtPlotCurve();
    QwtSymbol *marker0 = new QwtSymbol( QwtSymbol::Ellipse, Qt::green, QPen( Qt::green ), QSize( 2, 2 ) );
    interpolatedPoints->setSymbol(marker0);
    interpolatedPoints->setPen( QColor( Qt::green ) );
    interpolatedPoints->setStyle( QwtPlotCurve::NoCurve );
    interpolatedPoints->setSamples(interpolateXData, interpolateYData);
    interpolatedPoints->attach( plot );

    // oznaczenie pkt funkcji na podstawie ktorej byla wczesniej stworzona interpolacja
    QwtPlotCurve *points = new QwtPlotCurve();
    QwtSymbol *marker = new QwtSymbol( QwtSymbol::Ellipse, Qt::red, QPen( Qt::red ), QSize( 5, 5 ) );
    points->setSymbol(marker);
    points->setPen( QColor( Qt::red ) );
    points->setStyle( QwtPlotCurve::NoCurve );
    //points->setSamples(pDataX,pDataY);
    points->setSamples(xData,yData);
    points->attach( plot );

    // oznaczenie probek pobieranych do FFT
    QwtPlotCurve *FFTPoints = new QwtPlotCurve();
    QwtSymbol *marker1 = new QwtSymbol( QwtSymbol::Ellipse, Qt::magenta, QPen( Qt::magenta ), QSize( 5, 5 ) );
    FFTPoints->setSymbol(marker1);
    FFTPoints->setPen( QColor( Qt::magenta ) );
    FFTPoints->setStyle( QwtPlotCurve::NoCurve );
    FFTPoints->setSamples(fftXData,fftYData);
    FFTPoints->attach( plot );

    //do zoomowania
    QwtPlotZoomer* zoomer = new QwtPlotZoomer( plot->canvas() );
    zoomer->setRubberBandPen( QColor( Qt::black ) );
    zoomer->setTrackerPen( QColor( Qt::black ) );
    zoomer->setMousePattern( QwtEventPattern::MouseSelect2,
        Qt::RightButton, Qt::ControlModifier );
    zoomer->setMousePattern( QwtEventPattern::MouseSelect3,
        Qt::RightButton );
    //***********************

    QwtPlotPanner *panner = new QwtPlotPanner( plot->canvas() );
    panner->setMouseButton( Qt::MidButton );
    panner->setOrientations(Qt::Horizontal);
    QwtPlotMagnifier *magnifier = new QwtPlotMagnifier(plot->canvas() );
    magnifier->setAxisEnabled(QwtPlot::yLeft,false);
    return plot;
}


QwtPlot* AirEcgMain::plotLogPlotF(QList<double> &x,QList<double> &y,int rodzaj){
    QVector<double> xData = QVector<double>::fromList(x);
    QVector<double> yData = QVector<double>::fromList(y);

    double maxy=yData.first();
    double miny=yData.first();
    double maxx=xData.first();
    double minx=xData.first();

    for (int i=0;i<yData.size();++i)
    {
        if (maxy<yData[i]) maxy=yData[i];
        if (miny>yData[i]) miny=yData[i];

        if (maxx<xData[i]) maxx=xData[i];
        if (minx>xData[i]) minx=xData[i];
    }

    QwtPlot *plot = new QwtPlot();
    plot->setCanvasBackground( Qt::white );
    plot->setAxisScale( QwtPlot::yLeft, miny, maxy );
    plot->setAxisScale( QwtPlot::xBottom , minx, maxx);

    //labels
    QwtText xaxis("Time [Hz]");
    QwtText yaxis("Power");
    xaxis.setFont(QFont("Arial", 8));
    yaxis.setFont(QFont("Arial", 8));
    plot->setAxisTitle( QwtPlot::yLeft, yaxis );
    plot->setAxisTitle( QwtPlot::xBottom, xaxis );


    QwtPlotGrid *grid = new QwtPlotGrid();
    QPen *pen = new QPen;

    QColor kolor = QColor();
    kolor.setAlpha(127);
    kolor.setBlue(0);
    kolor.setRed(255);
    kolor.setBlue(0);
    pen->setColor(kolor);
    grid->setPen( *pen );
    grid->attach( plot );

    QwtPlotCurve *curve = new QwtPlotCurve();
    curve->setPen(QPen( Qt::blue, 1));
    curve->setRenderHint( QwtPlotItem::RenderAntialiased, true );

    curve->setSamples(xData,yData);
    curve->attach( plot );

    QwtPlotZoomer* zoomer = new QwtPlotZoomer( plot->canvas() );
    zoomer->setRubberBandPen( QColor( Qt::black ) );
    zoomer->setTrackerPen( QColor( Qt::black ) );
    zoomer->setMousePattern( QwtEventPattern::MouseSelect2,
        Qt::RightButton, Qt::ControlModifier );
    zoomer->setMousePattern( QwtEventPattern::MouseSelect3,
        Qt::RightButton );

    QwtPlotPanner *panner = new QwtPlotPanner( plot->canvas() );
    panner->setMouseButton( Qt::MidButton );
    panner->setOrientations(Qt::Horizontal);
    QwtPlotMagnifier *magnifier = new QwtPlotMagnifier(plot->canvas() );
    magnifier->setAxisEnabled(QwtPlot::yLeft,false);

    if (rodzaj == 1) {
        plot->setAxisScaleEngine( QwtPlot::xBottom, new QwtLog10ScaleEngine );
    } else if (rodzaj == 2){
        plot->setAxisScaleEngine( QwtPlot::yLeft, new QwtLog10ScaleEngine );
    } else if (rodzaj == 3){
        plot->setAxisScaleEngine( QwtPlot::yLeft, new QwtLog10ScaleEngine );
        plot->setAxisScaleEngine( QwtPlot::xBottom, new QwtLog10ScaleEngine );
    } else {
        //plot->setAxisScaleEngine( QwtPlot::xBottom, new QwtLog10ScaleEngine );
    }

    return plot;
}
// koniec hrv1

QwtPlot* AirEcgMain::plotBarChart(QList<unsigned int> &x, QList<int> &y){
    QVector<unsigned int> xData = QVector<unsigned int>::fromList(x);
    QVector<int> yData = QVector<int>::fromList(y);
    QVector<double> xDataFin = QVector<double>(xData.size());
    QVector<double> yDataFin = QVector<double>(yData.size());

    int maxx=xData.first();
    int minx=xData.first();
    int maxy=yData.first();
    int miny=yData.first();

    for (int i=0;i<yData.size();++i)
    {
        xDataFin[i] = xData[i];
        yDataFin[i] = yData[i];
        if (maxx<xData[i]) maxx=xData[i];
        if (minx>xData[i]) minx=xData[i];
        if (maxy<yData[i]) maxy=yData[i];
        if (miny>yData[i]) miny=yData[i];
    }

    QwtPlot *plot = new QwtPlot();
    plot->setCanvasBackground( Qt::white );
    plot->setAxisScale( QwtPlot::xBottom , minx, maxx);
    plot->setAxisScale( QwtPlot::yLeft, miny, maxy );

    QwtPlotGrid *grid = new QwtPlotGrid();
    QPen *pen = new QPen;

    QColor kolor = QColor();
    kolor.setAlpha(127);
    kolor.setRed(255);
    kolor.setGreen(0);
    kolor.setBlue(0);
    pen->setColor(kolor);
    grid->setPen( *pen );
    grid->attach( plot );

    QwtPlotCurve *points = new QwtPlotCurve();
    QwtSymbol *marker = new QwtSymbol( QwtSymbol::Ellipse, Qt::blue, QPen( Qt::blue ), QSize( 3, 3 ) );
    points->setSymbol(marker);
    points->setStyle( QwtPlotCurve::Sticks );
    points->setSamples(xDataFin, yDataFin);
    points->attach( plot );

    QwtPlotZoomer* zoomer = new QwtPlotZoomer( plot->canvas() );
    zoomer->setRubberBandPen( QColor( Qt::black ) );
    zoomer->setTrackerPen( QColor( Qt::black ) );
    zoomer->setMousePattern( QwtEventPattern::MouseSelect2,
        Qt::RightButton, Qt::ControlModifier );
    zoomer->setMousePattern( QwtEventPattern::MouseSelect3,
        Qt::RightButton );

    QwtPlotPanner *panner = new QwtPlotPanner( plot->canvas() );
    panner->setMouseButton( Qt::MidButton );
    panner->setOrientations(Qt::Horizontal);
    QwtPlotMagnifier *magnifier = new QwtPlotMagnifier(plot->canvas() );
    magnifier->setAxisEnabled(QwtPlot::yLeft,false);
    return plot;
}

QwtPlot* AirEcgMain::plotPointsPlot(const QVector<QVector<double>::const_iterator> &p, const QVector<double> &yData, float freq){
    //QVector<int> yData = QVector<int>::fromList(y);
    QVector<double> yDataFin = QVector<double>(yData.size());
    QVector<double> sampleNo = QVector<double>(yData.size());

    int max=yData.first();
    int min=yData.first();

    double tos=1/freq;

    for (int i=0;i<yData.size();++i)
    {
        sampleNo[i]=(i)*tos;
        yDataFin[i]=yData[i];
        if (max<yData[i]) max=yData[i];
        if (min>yData[i]) min=yData[i];
    }

    QwtPlot *plot = new QwtPlot();
    plot->setCanvasBackground( Qt::white );
    plot->setAxisScale( QwtPlot::yLeft, min, max );
    plot->setAxisScale( QwtPlot::xBottom , 0, 4.0);

    QwtText xaxis("Time [s]");
    QwtText yaxis("Voltage [mV]");
    xaxis.setFont(QFont("Arial", 8));
    yaxis.setFont(QFont("Arial", 8));

    plot->setAxisTitle( QwtPlot::yLeft, yaxis );
    plot->setAxisTitle( QwtPlot::xBottom, xaxis );

    QwtPlotGrid *grid = new QwtPlotGrid();
    QPen *pen = new QPen;

    QColor kolor = QColor();
    kolor.setAlpha(127);
    kolor.setBlue(0);
    kolor.setRed(255);
    kolor.setBlue(0);
    pen->setColor(kolor);
    grid->setPen( *pen );
    grid->attach( plot );

    QwtPlotCurve *curve = new QwtPlotCurve();
    curve->setPen(QPen( Qt::blue, 1));
    curve->setRenderHint( QwtPlotItem::RenderAntialiased, true );

    curve->setSamples(sampleNo,yDataFin);
    curve->attach( plot );

    // MARKERY do zaznaczania r_peaks lub innych punktow charakterystycznych    
    QVector<unsigned int> pData;
    for (int i=0;i<p.size();i++)
        pData.append(p.at(i)- yData.begin());
    //QVector<unsigned int> pData = QVector<unsigned int>::fromList(p);
    QVector<double> pDataX = QVector<double>(pData.size());
    QVector<double> pDataY = QVector<double>(pData.size());

    for (int i=0;i<pData.size();++i)
    {
        pDataX[i]=pData[i];
        pDataY[i]=yData[pDataX[i]];
        pDataX[i]=pDataX[i]*tos;
    }

    QwtPlotCurve *points = new QwtPlotCurve();
    QwtSymbol *marker = new QwtSymbol( QwtSymbol::Ellipse, Qt::red, QPen( Qt::red ), QSize( 5, 5 ) );
    points->setSymbol(marker);
    points->setPen( QColor( Qt::red ) );
    points->setStyle( QwtPlotCurve::NoCurve );
    points->setSamples(pDataX,pDataY);
    points->attach( plot );

    //
/*
    QwtPlotZoomer* zoomer = new QwtPlotZoomer( plot->canvas() );
    zoomer->setRubberBandPen( QColor( Qt::black ) );
    zoomer->setTrackerPen( QColor( Qt::black ) );
    zoomer->setMousePattern( QwtEventPattern::MouseSelect2,
        Qt::RightButton, Qt::ControlModifier );
    zoomer->setMousePattern( QwtEventPattern::MouseSelect3,
        Qt::RightButton );
*/
    zoom = new ScrollZoomer(plot->canvas());
    zoom->setRubberBandPen(QPen(Qt::white));
    //zoom->setZoomBase( false );
    plot->canvas()->setGeometry(0,0,sampleNo.last(),0);
    zoom->setZoomBase(plot->canvas()->rect());

    QwtPlotPanner *panner = new QwtPlotPanner( plot->canvas() );
    panner->setMouseButton( Qt::MidButton );
    panner->setOrientations(Qt::Horizontal);
    QwtPlotMagnifier *magnifier = new QwtPlotMagnifier(plot->canvas() );
    magnifier->setAxisEnabled(QwtPlot::yLeft,false);
    return plot;
}

QwtPlot* AirEcgMain::plotPoincarePlot(QList<unsigned int> &x, QList<int> &y, double &sd1, double &sd2){
    QVector<int> yData = QVector<int>::fromList(y);
    QVector<unsigned int> xData = QVector<unsigned int>::fromList(x);
    QVector<double> yDataFin = QVector<double>(yData.size());
    QVector<double> xDataFin = QVector<double>(xData.size());

    int maxy=yData.first();
    int miny=yData.first();
    int maxx=xData.first();
    int minx=xData.first();

    for (int i=0;i<yData.size();++i)
    {
        yDataFin[i]=yData[i];
        xDataFin[i]=xData[i];

        if (maxy<yData[i]) maxy=yData[i];
        if (miny>yData[i]) miny=yData[i];

        if (maxx<xData[i]) maxx=xData[i];
        if (minx>xData[i]) minx=xData[i];
    }

    QwtPlot *plot = new QwtPlot();
    plot->setCanvasBackground( Qt::white );
    plot->setAxisScale( QwtPlot::yLeft, miny, maxy );
    plot->setAxisScale( QwtPlot::xBottom , minx, maxx);

    QwtPlotGrid *grid = new QwtPlotGrid();
    QPen *pen = new QPen;

    QColor kolor = QColor();
    kolor.setAlpha(127);
    kolor.setRed(255);
    kolor.setGreen(0);
    kolor.setBlue(0);
    pen->setColor(kolor);
    grid->setPen( *pen );
    grid->attach( plot );

    // Punkty glowne
    QwtPlotCurve *points = new QwtPlotCurve();
    QwtSymbol *marker = new QwtSymbol( QwtSymbol::Ellipse, Qt::blue, QPen( Qt::blue ), QSize( 4, 4 ) );
    points->setSymbol(marker);
    points->setStyle( QwtPlotCurve::NoCurve );
    points->setSamples(xDataFin,yDataFin);
    points->attach( plot );

    // Glowne proste
    int sum = 0;
    for (int i=0; i<xData.size(); i++)
        sum += xData[i];
    sum += yData.last();
    double mean = sum / (xData.size()+1);

    QVector<double> xSamples1 = QVector<double>(2);
    QVector<double> ySamples1 = QVector<double>(2);
    QVector<double> xSamples2 = QVector<double>(2);
    QVector<double> ySamples2 = QVector<double>(2);

    xSamples1[0] = 0;
    ySamples1[0] = 0;
    xSamples1[1] = maxx;
    ySamples1[1] = maxx;

    QwtPlotCurve *curve1 = new QwtPlotCurve();
    curve1->setPen(QPen( Qt::black, 1));
    curve1->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    curve1->setSamples(xSamples1, ySamples1);
    curve1->attach( plot );

    xSamples2[0] = 0;
    ySamples2[0] = 2*mean;
    xSamples2[1] = maxx;
    ySamples2[1] = -maxx + 2*mean;

    QwtPlotCurve *curve2 = new QwtPlotCurve();
    curve2->setPen(QPen( Qt::black, 1));
    curve2->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    curve2->setSamples(xSamples2, ySamples2);
    curve2->attach( plot );

    // Elipsa
    int dokladnosc = 628;
    double interwal = 6.28 / dokladnosc;
    QVector<double> elipsaxTemp = QVector<double>(dokladnosc+1);
    QVector<double> elipsayTemp = QVector<double>(dokladnosc+1);

    for (double i=0.0; i<=6.28; i+=0.01)
    {
        elipsaxTemp.append(sd2*cos(i));
        elipsayTemp.append(sd1*cos(i-3.14/2));
    }

    QVector<double> elipsax = QVector<double>(dokladnosc+1);
    QVector<double> elipsay = QVector<double>(dokladnosc+1);

    for (int i=0; i<elipsaxTemp.size(); i++)
    {
        elipsax.append(elipsaxTemp[i]*cos(3.14/4) - elipsayTemp[i]*sin(3.14/4) + mean);
        elipsay.append(elipsaxTemp[i]*sin(3.14/4) + elipsayTemp[i]*cos(3.14/4) + mean);
    }

    QwtPlotCurve *curveElipse = new QwtPlotCurve();
    curveElipse->setPen(QPen( Qt::black, 1));
    curveElipse->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    curveElipse->setSamples(elipsax, elipsay);
    curveElipse->attach( plot );

    // Odcinki SD1, SD2
    QVector<double> sd1xSamples = QVector<double>(2);
    QVector<double> sd1ySamples = QVector<double>(2);
    QVector<double> sd2xSamples = QVector<double>(2);
    QVector<double> sd2ySamples = QVector<double>(2);

    sd1xSamples[0] = mean - sd1/sqrt(2);
    sd1ySamples[0] = mean + sd1/sqrt(2);
    sd1xSamples[1] = mean;
    sd1ySamples[1] = mean;
    sd2xSamples[0] = mean + sd2/sqrt(2);
    sd2ySamples[0] = mean + sd2/sqrt(2);
    sd2xSamples[1] = mean;
    sd2ySamples[1] = mean;

    QwtPlotCurve *curveSd1 = new QwtPlotCurve();
    curveSd1->setPen(QPen( Qt::green, 3));
    curveSd1->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    curveSd1->setSamples(sd1xSamples, sd1ySamples);
    curveSd1->attach( plot );

    QwtPlotCurve *curveSd2 = new QwtPlotCurve();
    curveSd2->setPen(QPen( Qt::red, 3));
    curveSd2->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    curveSd2->setSamples(sd2xSamples, sd2ySamples);
    curveSd2->attach( plot );

    // Reszta
    QwtPlotZoomer* zoomer = new QwtPlotZoomer( plot->canvas() );
    zoomer->setRubberBandPen( QColor( Qt::black ) );
    zoomer->setTrackerPen( QColor( Qt::black ) );
    zoomer->setMousePattern( QwtEventPattern::MouseSelect2,
        Qt::RightButton, Qt::ControlModifier );
    zoomer->setMousePattern( QwtEventPattern::MouseSelect3,
        Qt::RightButton );

    QwtPlotPanner *panner = new QwtPlotPanner( plot->canvas() );
    panner->setMouseButton( Qt::MidButton );
    panner->setOrientations(Qt::Horizontal);
    QwtPlotMagnifier *magnifier = new QwtPlotMagnifier(plot->canvas() );
    //magnifier->setAxisEnabled(QwtPlot::yLeft,false);
    return plot;
}

QwtPlot* AirEcgMain::plotTWAPlot(const QVector<double> &yData, QList<unsigned int> &TWA_positive, QList<unsigned int> &TWA_negative, float freq){

    QVector<double> yDataFin = QVector<double>();
    QVector<double> sampleNo = QVector<double>();

    int max=yData.first();
    int min=yData.first();

    double tos=1/freq;

    int maxSize = yData.size();

    for (int i=0;i<maxSize;++i)
    {
        sampleNo.append(i);
        yDataFin.append(yData[i]);
        if (max<yData[i]) max=yData[i];
        if (min>yData[i]) min=yData[i];
    }

    QwtPlot *plot = new QwtPlot();
    plot->setCanvasBackground( Qt::white );

    QwtPlotGrid *grid = new QwtPlotGrid();
    QPen *pen = new QPen;

    QColor kolor = QColor();
    kolor.setAlpha(127);
    kolor.setBlue(0);
    kolor.setRed(255);
    kolor.setGreen(0);
    pen->setColor(kolor);
    grid->setPen( *pen );
    grid->attach( plot );


    // MARKERY do zaznaczania r_peaks lub innych punktow charakterystycznych

    QVector<double> posDataX = QVector<double>();
    QVector<double> posDataY = QVector<double>();
    QVector<double> negDataX = QVector<double>();
    QVector<double> negDataY = QVector<double>();

    for (int i=0;i<TWA_positive.length();i++) {
        posDataX.append(TWA_positive.at(i));
        posDataY.append(yData[TWA_positive.at(i)]);
    }
    for (int i=0;i<TWA_negative.length();i++) {
        negDataX.append(TWA_negative.at(i));
        negDataY.append(yData[TWA_negative.at(i)]);
    }

    for (int i=0; i<sampleNo.size(); ++i){
        sampleNo[i]=sampleNo[i]*tos;
    }

    for (int i=0; i<posDataX.size(); ++i){
        posDataX[i]=posDataX[i]*tos;
    }

    for (int i=0; i<negDataX.size(); ++i){
        negDataX[i]=negDataX[i]*tos;
    }
    QwtPlotCurve *curve = new QwtPlotCurve();
    curve->setPen(QPen( Qt::blue, 1));
    curve->setRenderHint( QwtPlotItem::RenderAntialiased, true );

    plot->setAxisScale( QwtPlot::yLeft, min, max );
    plot->setAxisScale( QwtPlot::xBottom , 0, 4.0);

    curve->setSamples(sampleNo,yDataFin);
    curve->attach( plot );

    QwtPlotCurve *posPoints = new QwtPlotCurve();
    QwtSymbol *posMarker = new QwtSymbol( QwtSymbol::Ellipse, Qt::red, QPen( Qt::red ), QSize( 5, 5 ) );
    posPoints->setSymbol(posMarker);
    posPoints->setPen( QColor( Qt::red ) );
    posPoints->setStyle( QwtPlotCurve::NoCurve );
    posPoints->setSamples(posDataX,posDataY);
    posPoints->attach( plot );


    QwtPlotCurve *negPoints = new QwtPlotCurve();
    QwtSymbol *negMarker = new QwtSymbol( QwtSymbol::Ellipse, Qt::green, QPen( Qt::green ), QSize( 5, 5 ) );
    negPoints->setSymbol(negMarker);
    negPoints->setPen( QColor( Qt::green ) );
    negPoints->setStyle( QwtPlotCurve::NoCurve );
    negPoints->setSamples(negDataX,negDataY);
    negPoints->attach( plot );

    //
/*
    QwtPlotZoomer* zoomer = new QwtPlotZoomer( plot->canvas() );
    zoomer->setRubberBandPen( QColor( Qt::black ) );
    zoomer->setTrackerPen( QColor( Qt::black ) );
    zoomer->setMousePattern( QwtEventPattern::MouseSelect2,
        Qt::RightButton, Qt::ControlModifier );
    zoomer->setMousePattern( QwtEventPattern::MouseSelect3,
        Qt::RightButton );
*/
    zoom = new ScrollZoomer(plot->canvas());
    zoom->setRubberBandPen(QPen(Qt::white));
    //zoom->setZoomBase( false );
    plot->canvas()->setGeometry(0,0,sampleNo.last(),0);
    zoom->setZoomBase(plot->canvas()->rect());

    QwtPlotPanner *panner = new QwtPlotPanner( plot->canvas() );
    panner->setMouseButton( Qt::MidButton );
    panner->setOrientations(Qt::Horizontal);
    QwtPlotMagnifier *magnifier = new QwtPlotMagnifier(plot->canvas() );
    magnifier->setAxisEnabled(QwtPlot::yLeft,false);

    QwtText xaxis("Time [s]");
    QwtText yaxis("Voltage [mV]");
    xaxis.setFont(QFont("Arial", 8));
    yaxis.setFont(QFont("Arial", 8));

    plot->setAxisTitle( QwtPlot::yLeft, yaxis );
    plot->setAxisTitle( QwtPlot::xBottom, xaxis );

    return plot;
}

QwtPlot* AirEcgMain::plotTWAPlot2(QList<unsigned int> &TWA_positive, QList<double> &TWA_positive_value, QList<unsigned int> &TWA_negative, QList<double> &TWA_negative_value) {

    QVector<double> yDataFin = QVector<double>();
    QVector<double> sampleNo = QVector<double>();
    int max;
    int min;
    if (TWA_positive.length()!=0) {
        TWA_positive_value.at(0);
        TWA_positive_value.at(0);
    } else {
        TWA_negative_value.at(0);
        TWA_negative_value.at(0);
    }

    QVector<double> posDataX = QVector<double>();
    QVector<double> posDataY = QVector<double>();
    QVector<double> negDataX = QVector<double>();
    QVector<double> negDataY = QVector<double>();

    int pos=0,neg=0,i=0;
    while(!(TWA_positive.length()==pos && TWA_negative.length()==neg)) {
        if (pos!=TWA_positive.length() && neg !=TWA_negative.length()) {
            if (TWA_positive.at(pos)<TWA_negative.at(neg)) {
                sampleNo.append(i);
                yDataFin.append(TWA_positive_value.at(pos));
                posDataX.append(i);
                posDataY.append(TWA_positive_value.at(pos));
                if (max < TWA_positive_value.at(pos))
                    max = TWA_positive_value.at(pos);
                else if (min > TWA_positive_value.at(pos))
                    min = TWA_positive_value.at(pos);
                pos++;
            } else {
                sampleNo.append(i);
                yDataFin.append(TWA_negative_value.at(neg));
                negDataX.append(i);
                negDataY.append(TWA_negative_value.at(neg));
                if (max < TWA_negative_value.at(neg))
                    max = TWA_negative_value.at(neg);
                else if (min > TWA_negative_value.at(neg))
                    min = TWA_negative_value.at(neg);
                neg++;
            }
            i++;
        } else if (pos==TWA_positive.length() && neg!=TWA_negative.length()) {
            if (neg!=TWA_negative.length()-1) {
                sampleNo.append(i);
                yDataFin.append(TWA_negative_value.at(neg));
                negDataX.append(i);
                negDataY.append(TWA_negative_value.at(neg));
                if (max < TWA_negative_value.at(neg))
                    max = TWA_negative_value.at(neg);
                else if (min > TWA_negative_value.at(neg))
                    min = TWA_negative_value.at(neg);
            }
            neg++;
            i++;
        } else if (neg==TWA_negative.length() && pos!=TWA_positive.length()) {
            if (pos!=TWA_positive.length()-1) {
                sampleNo.append(i);
                yDataFin.append(TWA_positive_value.at(pos));
                posDataX.append(i);
                posDataY.append(TWA_positive_value.at(pos));
                if (max < TWA_positive_value.at(pos))
                    max = TWA_positive_value.at(pos);
                else if (min > TWA_positive_value.at(pos))
                    min = TWA_positive_value.at(pos);
            }
            pos++;
            i++;
        } else {
            break;
        }
    }

    QwtPlot *plot = new QwtPlot();
    plot->setCanvasBackground( Qt::white );
    //plot->setAxisScale( QwtPlot::yLeft, min, max );
    plot->setAxisScale( QwtPlot::xBottom , 0, 4.0);

    QwtPlotGrid *grid = new QwtPlotGrid();
    QPen *pen = new QPen;

    QColor kolor = QColor();
    kolor.setAlpha(127);
    kolor.setBlue(0);
    kolor.setRed(255);
    kolor.setGreen(0);
    pen->setColor(kolor);
    grid->setPen( *pen );
    grid->attach( plot );

    QwtPlotCurve *curve = new QwtPlotCurve();
    curve->setPen(QPen( Qt::blue, 1));
    curve->setRenderHint( QwtPlotItem::RenderAntialiased, true );

    curve->setSamples(sampleNo,yDataFin);
    curve->attach( plot );

    QwtPlotCurve *posPoints = new QwtPlotCurve();
    QwtSymbol *posMarker = new QwtSymbol( QwtSymbol::Ellipse, Qt::red, QPen( Qt::red ), QSize( 5, 5 ) );
    posPoints->setSymbol(posMarker);
    posPoints->setPen( QColor( Qt::red ) );
    posPoints->setStyle( QwtPlotCurve::NoCurve );
    posPoints->setSamples(posDataX,posDataY);
    posPoints->attach( plot );


    QwtPlotCurve *negPoints = new QwtPlotCurve();
    QwtSymbol *negMarker = new QwtSymbol( QwtSymbol::Ellipse, Qt::green, QPen( Qt::green ), QSize( 5, 5 ) );
    negPoints->setSymbol(negMarker);
    negPoints->setPen( QColor( Qt::green ) );
    negPoints->setStyle( QwtPlotCurve::NoCurve );
    negPoints->setSamples(negDataX,negDataY);
    negPoints->attach( plot );

    //
/*
    QwtPlotZoomer* zoomer = new QwtPlotZoomer( plot->canvas() );
    zoomer->setRubberBandPen( QColor( Qt::black ) );
    zoomer->setTrackerPen( QColor( Qt::black ) );
    zoomer->setMousePattern( QwtEventPattern::MouseSelect2,
        Qt::RightButton, Qt::ControlModifier );
    zoomer->setMousePattern( QwtEventPattern::MouseSelect3,
        Qt::RightButton );
*/
    zoom = new ScrollZoomer(plot->canvas());
    zoom->setRubberBandPen(QPen(Qt::white));
    //zoom->setZoomBase( false );
    plot->canvas()->setGeometry(0,0,sampleNo.last(),0);
    zoom->setZoomBase(plot->canvas()->rect());

    QwtPlotPanner *panner = new QwtPlotPanner( plot->canvas() );
    panner->setMouseButton( Qt::MidButton );
    panner->setOrientations(Qt::Horizontal);
    QwtPlotMagnifier *magnifier = new QwtPlotMagnifier(plot->canvas() );
    magnifier->setAxisEnabled(QwtPlot::yLeft,false);


    return plot;
}

//Dwa wykresy dla DFA
QwtPlot* AirEcgMain::plotPlotdfa(QList<double> &y1, QList<double> &y2){
    QVector<double> y1Data = QVector<double>::fromList(y1);
       QVector<double> y2Data = QVector<double>::fromList(y2);
    QVector<double> y1DataFin = QVector<double>();
        QVector<double> y2DataFin = QVector<double>();
    QVector<double> sampleNo = QVector<double>();

    double max1=y1Data.first();
    double min1=y1Data.first();
    double max2=y2Data.first();
    double min2=y2Data.first();

    for (int i=0;i<y1Data.size();++i)
    {
        sampleNo.append(i);
        y1DataFin.append(y1Data[i]);
        if (max1<y1Data[i]) max1=y1Data[i];
        if (min1>y1Data[i]) min1=y1Data[i];
    }

    for (int i=0;i<y2Data.size();++i)
    {
        sampleNo.append(i);
        y2DataFin.append(y2Data[i]);
        if (max2<y2Data[i]) max2=y2Data[i];
        if (min2>y2Data[i]) min2=y2Data[i];
    }


    QwtPlot *plot = new QwtPlot();
    plot->setCanvasBackground( Qt::white );
    plot->setAxisScale( QwtPlot::yLeft, std::min(min1, min2), std::max(max1, max2));
    plot->setAxisScale( QwtPlot::xBottom , 0, std::max(y1Data.size(), y2Data.size()));

    QwtPlotGrid *grid = new QwtPlotGrid();
    QPen *pen = new QPen;

    QColor kolor = QColor();
    kolor.setAlpha(127);
    kolor.setBlue(0);
    kolor.setRed(255);
    kolor.setBlue(0);
    pen->setColor(kolor);
    grid->setPen( *pen );
    grid->attach( plot );

    QwtPlotCurve *curve1 = new QwtPlotCurve();
    curve1->setPen(QPen( Qt::blue, 1));
    curve1->setRenderHint( QwtPlotItem::RenderAntialiased, true );

    curve1->setSamples(sampleNo,y1DataFin);
    curve1->attach( plot );

    QwtPlotCurve *curve2 = new QwtPlotCurve();
    curve2->setPen(QPen( Qt::red, 2));
    curve2->setRenderHint( QwtPlotItem::RenderAntialiased, true );

    curve2->setSamples(sampleNo,y2DataFin);
    curve2->attach( plot );

    QwtPlotZoomer* zoomer = new QwtPlotZoomer( plot->canvas() );
    zoomer->setRubberBandPen( QColor( Qt::black ) );
    zoomer->setTrackerPen( QColor( Qt::black ) );
    zoomer->setMousePattern( QwtEventPattern::MouseSelect2,
        Qt::RightButton, Qt::ControlModifier );
    zoomer->setMousePattern( QwtEventPattern::MouseSelect3,
        Qt::RightButton );

    QwtPlotPanner *panner = new QwtPlotPanner( plot->canvas() );
    panner->setMouseButton( Qt::MidButton );
    panner->setOrientations(Qt::Horizontal);
    QwtPlotMagnifier *magnifier = new QwtPlotMagnifier(plot->canvas() );
    magnifier->setAxisEnabled(QwtPlot::yLeft,false);
    return plot;
}

QwtPlot* AirEcgMain::plotPointsPlotDFA(QList<double> &x, QList<double> &y , double &wsp_a, double &wsp_b){


    QVector<double> yData = QVector<double>::fromList(y);
    QVector<double> xData = QVector<double>::fromList(x);
    QVector<double> yDataFin = QVector<double>(yData.size());
    QVector<double> xDataFin = QVector<double>(xData.size());

    double maxy=yData.first();
    double miny=yData.first();
    double maxx=xData.first();
    double minx=xData.first();

    for (int i=0;i<yData.size();++i)
    {
        yDataFin[i]=yData[i];
        xDataFin[i]=xData[i];

        if (maxy<yData[i]) maxy=yData[i];
        if (miny>yData[i]) miny=yData[i];

        if (maxx<xData[i]) maxx=xData[i];
        if (minx>xData[i]) minx=xData[i];
    }

    QVector<double> xSamples1 = QVector<double>(2);
    QVector<double> ySamples1 = QVector<double>(2);


    xSamples1[0] = minx;
    ySamples1[0] = wsp_a*minx+wsp_b;
    xSamples1[1] = maxx;
    ySamples1[1] = wsp_a*maxx+wsp_b;


    QwtPlot *plot = new QwtPlot();
    plot->setCanvasBackground( Qt::white );
    plot->setAxisScale( QwtPlot::yLeft, std::min(miny, ySamples1[0]), std::max(maxy, ySamples1[1]));
    plot->setAxisScale( QwtPlot::xBottom , std::min(minx, xSamples1[0]), std::max(maxx, xSamples1[1]));

    QwtPlotGrid *grid = new QwtPlotGrid();
    QPen *pen = new QPen;

    QColor kolor = QColor();
    kolor.setAlpha(127);
    kolor.setRed(255);
    kolor.setGreen(0);
    kolor.setBlue(0);
    pen->setColor(kolor);
    grid->setPen( *pen );
    grid->attach( plot );

    QwtPlotCurve *curve1 = new QwtPlotCurve();
    curve1->setPen(QPen( Qt::black, 1));
    curve1->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    curve1->setSamples(xSamples1, ySamples1);
    curve1->attach( plot );



    // Punkty główne
    QwtPlotCurve *points = new QwtPlotCurve();
    QwtSymbol *marker = new QwtSymbol( QwtSymbol::Ellipse, Qt::blue, QPen( Qt::blue ), QSize( 4, 4 ) );
    points->setSymbol(marker);
    points->setStyle( QwtPlotCurve::NoCurve );
    points->setSamples(xDataFin,yDataFin);
    points->attach( plot );
    //

    QwtPlotZoomer* zoomer = new QwtPlotZoomer( plot->canvas() );
    zoomer->setRubberBandPen( QColor( Qt::black ) );
    zoomer->setTrackerPen( QColor( Qt::black ) );
    zoomer->setMousePattern( QwtEventPattern::MouseSelect2,
        Qt::RightButton, Qt::ControlModifier );
    zoomer->setMousePattern( QwtEventPattern::MouseSelect3,
        Qt::RightButton );

    QwtPlotPanner *panner = new QwtPlotPanner( plot->canvas() );
    panner->setMouseButton( Qt::MidButton );
    panner->setOrientations(Qt::Horizontal);
    QwtPlotMagnifier *magnifier = new QwtPlotMagnifier(plot->canvas() );
    magnifier->setAxisEnabled(QwtPlot::yLeft,false);
    return plot;
}

QwtPlot *AirEcgMain::plotWavesPlot(QList<int> &ecgSignal, QList<Waves::EcgFrame *> &ecgFrames, double samplingFrequency)
{
    QVector<int> yData = QVector<int>::fromList(ecgSignal);
    QVector<double> yDataFin = QVector<double>(yData.size());
    QVector<double> sampleNo = QVector<double>(yData.size());

    int max=yData.first();
    int min=yData.first();

    double dt = 1.0/samplingFrequency;
    for (int i=0;i<yData.size();++i)
    {
        sampleNo[i]=(i)*dt;
        yDataFin[i]=yData[i];
        if (max<yData[i]) max=yData[i];
        if (min>yData[i]) min=yData[i];
    }

    QwtPlot *plot = new QwtPlot();
    plot->setCanvasBackground( Qt::white );
    plot->setAxisScale( QwtPlot::yLeft, min, max );
    plot->setAxisScale( QwtPlot::xBottom , 0, 4.0);

    QwtPlotGrid *grid = new QwtPlotGrid();
    QPen *pen = new QPen;

    QColor kolor = QColor();
    kolor.setAlpha(127);
    kolor.setBlue(0);
    kolor.setRed(255);
    kolor.setBlue(0);
    pen->setColor(kolor);
    grid->setPen( *pen );
    grid->attach( plot );

    QwtPlotCurve *curve = new QwtPlotCurve();
    curve->setPen(QPen( Qt::blue, 1));
    curve->setTitle("Signal");
    curve->setRenderHint( QwtPlotItem::RenderAntialiased, true );

    curve->setSamples(sampleNo,yDataFin);
    curve->attach( plot );
    zoom = new ScrollZoomer(plot->canvas());
    zoom->setRubberBandPen(QPen(Qt::white));
    //zoom->setZoomBase( false );
    plot->canvas()->setGeometry(0,0,yData.size()*dt,0);
    zoom->setZoomBase(plot->canvas()->rect());

    // MARKERY do zaznaczania r_peaks lub innych punktow charakterystycznych

    QVector<unsigned int> P_onsetData;
    QVector<unsigned int> P_endData;
    QVector<unsigned int> Qrs_onsetData;
    QVector<unsigned int> Qrs_endData;
    QVector<unsigned int> T_endData;
    for(unsigned int i = 0; i < ecgFrames.size(); i++)
    {
        P_onsetData.append(ecgFrames[i]->P_onset);
        P_endData.append(ecgFrames[i]->P_end);
        Qrs_onsetData.append(ecgFrames[i]->QRS_onset);
        Qrs_endData.append(ecgFrames[i]->QRS_end);
        T_endData.append(ecgFrames[i]->T_end);
    }

    QVector<double> P_onsetDataX = QVector<double>(ecgFrames.size());
    QVector<double> P_onsetDataY = QVector<double>(ecgFrames.size());

    QVector<double> P_endDataX = QVector<double>(ecgFrames.size());
    QVector<double> P_endDataY = QVector<double>(ecgFrames.size());

    QVector<double> Qrs_onsetDataX = QVector<double>(ecgFrames.size());
    QVector<double> Qrs_onsetDataY = QVector<double>(ecgFrames.size());

    QVector<double> Qrs_endDataX = QVector<double>(ecgFrames.size());
    QVector<double> Qrs_endDataY = QVector<double>(ecgFrames.size());

    QVector<double> T_endDataX = QVector<double>(ecgFrames.size());
    QVector<double> T_endDataY = QVector<double>(ecgFrames.size());


    for (int i=0; i < ecgFrames.size();++i)
    {
        P_onsetDataX[i]=P_onsetData[i]*dt;
        P_onsetDataY[i]=yData[P_onsetData[i]];

        P_endDataX[i]=P_endData[i]*dt;
        P_endDataY[i]=yData[P_endData[i]];

        Qrs_onsetDataX[i]=Qrs_onsetData[i]*dt;
        Qrs_onsetDataY[i]=yData[Qrs_onsetData[i]];

        Qrs_endDataX[i]=Qrs_endData[i]*dt;
        Qrs_endDataY[i]=yData[Qrs_endData[i]];

        T_endDataX[i]=T_endData[i]*dt;
        T_endDataY[i]=yData[T_endData[i]];
    }

    QwtPlotCurve *P_onsetPoints = new QwtPlotCurve();
    QwtSymbol *P_onsetMarker = new QwtSymbol( QwtSymbol::Ellipse, Qt::green, QPen( Qt::green ), QSize( 5, 5 ) );
    P_onsetPoints->setSymbol(P_onsetMarker);
    P_onsetPoints->setTitle("P_onset");
    P_onsetPoints->setPen( QColor( Qt::green ) );
    P_onsetPoints->setStyle( QwtPlotCurve::NoCurve );
    P_onsetPoints->setSamples(P_onsetDataX,P_onsetDataY);
    P_onsetPoints->attach( plot );

    QwtPlotCurve *P_endPoints = new QwtPlotCurve();
    QwtSymbol *P_endMarker = new QwtSymbol( QwtSymbol::Ellipse, Qt::cyan, QPen( Qt::cyan ), QSize( 5, 5 ) );
    P_endPoints->setSymbol(P_endMarker);
    P_endPoints->setTitle("P_end");
    P_endPoints->setPen( QColor( Qt::cyan ) );
    P_endPoints->setStyle( QwtPlotCurve::NoCurve );
    P_endPoints->setSamples(P_endDataX,P_endDataY);
    P_endPoints->attach( plot );

    QwtPlotCurve *Qrs_onsetPoints = new QwtPlotCurve();
    QwtSymbol *Qrs_onsetMarker = new QwtSymbol( QwtSymbol::Ellipse, Qt::red, QPen( Qt::red ), QSize( 5, 5 ) );
    Qrs_onsetPoints->setSymbol(Qrs_onsetMarker);
    Qrs_onsetPoints->setTitle("QRS_onset");
    Qrs_onsetPoints->setPen( QColor( Qt::red ) );
    Qrs_onsetPoints->setStyle( QwtPlotCurve::NoCurve );
    Qrs_onsetPoints->setSamples(Qrs_onsetDataX,Qrs_onsetDataY);
    Qrs_onsetPoints->attach( plot );

    QwtPlotCurve *Qrs_endPoints = new QwtPlotCurve();
    QwtSymbol *Qrs_endMarker = new QwtSymbol( QwtSymbol::Ellipse, Qt::magenta, QPen( Qt::magenta ), QSize( 5, 5 ) );
    Qrs_endPoints->setSymbol(Qrs_endMarker);
    Qrs_endPoints->setTitle("QRS_end");
    Qrs_endPoints->setPen( QColor( Qt::magenta ) );
    Qrs_endPoints->setStyle( QwtPlotCurve::NoCurve );
    Qrs_endPoints->setSamples(Qrs_endDataX,Qrs_endDataY);
    Qrs_endPoints->attach( plot );

    QwtPlotCurve *T_endPoints = new QwtPlotCurve();
    QwtSymbol *T_endMarker = new QwtSymbol( QwtSymbol::Ellipse, Qt::black, QPen( Qt::black ), QSize( 5, 5 ) );
    T_endPoints->setSymbol(T_endMarker);
    T_endPoints->setTitle("T_end");
    T_endPoints->setPen( QColor( Qt::black ) );
    T_endPoints->setStyle( QwtPlotCurve::NoCurve );
    T_endPoints->setSamples(T_endDataX,T_endDataY);
    T_endPoints->attach( plot );


    QwtLegend* legend = new QwtLegend();
    legend->setItemMode(QwtLegend::ReadOnlyItem);
    plot->insertLegend(legend, QwtPlot::BottomLegend);
    //

    QwtPlotZoomer* zoomer = new QwtPlotZoomer( plot->canvas() );
    zoomer->setRubberBandPen( QColor( Qt::black ) );
    zoomer->setTrackerPen( QColor( Qt::black ) );
    zoomer->setMousePattern( QwtEventPattern::MouseSelect2,
        Qt::RightButton, Qt::ControlModifier );
    zoomer->setMousePattern( QwtEventPattern::MouseSelect3,
        Qt::RightButton );

    QwtPlotPanner *panner = new QwtPlotPanner( plot->canvas() );
    panner->setMouseButton( Qt::MidButton );
    panner->setOrientations(Qt::Horizontal);
    QwtPlotMagnifier *magnifier = new QwtPlotMagnifier(plot->canvas() );
    magnifier->setAxisEnabled(QwtPlot::yLeft,false);

    QwtText xaxis("Time [s]");
    QwtText yaxis("Voltage [mV]");
    xaxis.setFont(QFont("Arial", 8));
    yaxis.setFont(QFont("Arial", 8));

    plot->setAxisTitle( QwtPlot::yLeft, yaxis );
    plot->setAxisTitle( QwtPlot::xBottom, xaxis );

    return plot;
}

QwtPlot *AirEcgMain::plotIntervalPlot(QList<double> &ecgbaselined, QList<int> &stbegin, QList<int> &stend, double samplingFrequency)
{
    QVector<double> yData = QVector<double>::fromList(ecgbaselined);
    QVector<double> yDataFin = QVector<double>(yData.size());
    QVector<double> sampleNo = QVector<double>(yData.size());

    double max=yData.first();
    double min=yData.first();

    double dt = 1.0/samplingFrequency;
    for (int i=0;i<yData.size();++i)
    {
        sampleNo[i]=(i)*dt;
        yDataFin[i]=yData[i];
        if (max<yData[i]) max=yData[i];
        if (min>yData[i]) min=yData[i];
    }


    QwtPlot *plot = new QwtPlot();
    plot->setCanvasBackground( Qt::white );
    plot->setAxisScale( QwtPlot::yLeft, min, max );
    plot->setAxisScale( QwtPlot::xBottom , 0, yData.size()*dt);

    QwtPlotGrid *grid = new QwtPlotGrid();
    QPen *pen = new QPen;

    QColor kolor = QColor();
    kolor.setAlpha(127);
    kolor.setBlue(0);
    kolor.setRed(255);
    kolor.setBlue(0);
    pen->setColor(kolor);
    grid->setPen( *pen );
    grid->attach( plot );

    QwtPlotCurve *curve = new QwtPlotCurve();
    curve->setPen(QPen( Qt::blue, 1));
    curve->setTitle("Signal");
    curve->setRenderHint( QwtPlotItem::RenderAntialiased, true );

    curve->setSamples(sampleNo,yDataFin);
    curve->attach( plot );

    // MARKERY do zaznaczania r_peaks lub innych punktow charakterystycznych

    QVector<unsigned int> st_beginData;
    QVector<unsigned int> st_endData;

    for(unsigned int i = 0; i < stbegin.size(); i++)
    {
        st_beginData.append(stbegin.at(i));
        st_endData.append(stend.at(i));
    }

    QVector<double> st_beginDataX = QVector<double>(stbegin.size());
    QVector<double> st_beginDataY = QVector<double>(stend.size());

    QVector<double> st_endDataX = QVector<double>(stbegin.size());
    QVector<double> st_endDataY = QVector<double>(stend.size());


    for (int i=0; i < stbegin.size();++i)
    {
        st_beginDataX[i]=st_beginData[i]*dt;
        st_beginDataY[i]=yData[st_beginData[i]];

        st_endDataX[i]=st_endData[i]*dt;
        st_endDataY[i]=yData[st_endData[i]];

    }

    QwtPlotCurve *st_beginPoints = new QwtPlotCurve();
    QwtSymbol *st_beginMarker = new QwtSymbol( QwtSymbol::Ellipse, Qt::red, QPen( Qt::red ), QSize( 5, 5 ) );
    st_beginPoints->setSymbol(st_beginMarker);
    st_beginPoints->setTitle("ST_begin");
    st_beginPoints->setPen( QColor( Qt::red ) );
    st_beginPoints->setStyle( QwtPlotCurve::NoCurve );
    st_beginPoints->setSamples(st_beginDataX,st_beginDataY);
    st_beginPoints->attach( plot );

    QwtPlotCurve *st_endPoints = new QwtPlotCurve();
    QwtSymbol *st_endMarker = new QwtSymbol( QwtSymbol::Ellipse, Qt::black, QPen( Qt::black ), QSize( 5, 5 ) );
    st_endPoints->setSymbol(st_endMarker);
    st_endPoints->setTitle("ST_end");
    st_endPoints->setPen( QColor( Qt::black ) );
    st_endPoints->setStyle( QwtPlotCurve::NoCurve );
    st_endPoints->setSamples(st_endDataX,st_endDataY);
    st_endPoints->attach( plot );

    QwtLegend* legend = new QwtLegend();
    legend->setItemMode(QwtLegend::ReadOnlyItem);
    plot->insertLegend(legend, QwtPlot::BottomLegend);
    //

    QwtPlotZoomer* zoomer = new QwtPlotZoomer( plot->canvas() );
    zoomer->setRubberBandPen( QColor( Qt::black ) );
    zoomer->setTrackerPen( QColor( Qt::black ) );
    zoomer->setMousePattern( QwtEventPattern::MouseSelect2,
        Qt::RightButton, Qt::ControlModifier );
    zoomer->setMousePattern( QwtEventPattern::MouseSelect3,
        Qt::RightButton );

    QwtPlotPanner *panner = new QwtPlotPanner( plot->canvas() );
    panner->setMouseButton( Qt::MidButton );
    panner->setOrientations(Qt::Horizontal);
    QwtPlotMagnifier *magnifier = new QwtPlotMagnifier(plot->canvas() );
    magnifier->setAxisEnabled(QwtPlot::yLeft,false);

    QwtText xaxis("Time [s]");
    QwtText yaxis("Voltage [mV]");
    xaxis.setFont(QFont("Arial", 8));
    yaxis.setFont(QFont("Arial", 8));

    plot->setAxisTitle( QwtPlot::yLeft, yaxis );
    plot->setAxisTitle( QwtPlot::xBottom, xaxis );

    return plot;
}

void AirEcgMain::drawEcgBaseline(EcgData *data)
{
    QLOG_INFO() << "Start rysowania baseline";
    //dla pierwszego taba
    QwtPlot *plotMLII = plotPlot(*(data->ecg_baselined),data->info->frequencyValue);
    ui->baselinedArea->setWidget(plotMLII);
    ui->baselinedArea->show();
    QLOG_INFO() << "Koniec pierwszego rysowania baseline";
    //dla sig edr
    QwtPlot *plotBaseEDR = plotPlot(*(data->ecg_baselined),data->info->frequencyValue);
    ui->Baseline_edr->setWidget(plotBaseEDR);
    ui->Baseline_edr->show();
    QLOG_INFO() << "Koniec drugiego rysowania baseline";
}

void AirEcgMain::drawAtrialFibr(EcgData *data)
{
    QLOG_INFO() << "Start \"rysowania\" AtrialFibr";
    //
}

void AirEcgMain::drawRPeaks(EcgData *data)
{

    QwtPlot *plotVI = plotPointsPlot(*(data->Rpeaks),*(data->ecg_baselined),data->info->frequencyValue);
    ui->rpeaksArea->setWidget(plotVI);
    ui->rpeaksArea->show();
}

void AirEcgMain::drawHrv1(EcgData *data)
{
    ui->Mean->setText("Mean = " % QString::number((data->Mean), 'f', 2) + " ms");
    ui->SDNN->setText("SDNN = " %QString::number((data->SDNN), 'f', 2) + " ms");
    ui->RMSSD->setText("RMSSD = " %QString::number((data->RMSSD), 'f', 2) + " ms");
    ui->RR50->setText("RR50 = " %QString::number((data->RR50), 'd', 2));
    ui->RR50Ratio->setText("RR50 Ratio = " %QString::number((data-> RR50Ratio), 'c', 2) + " %");
    ui->SDANN->setText("SDANN = " %QString::number((data->SDANN), 'f', 2) + " ms");
    ui->SDANNindex->setText("SDANN Index = " %QString::number((data->SDANNindex), 'f', 2) + " ms");
    ui->SDSD->setText("SDSD = " %QString::number((data->SDSD), 'f', 2) + " ms");

    //Frequency Parameters
    QwtPlot *plotRR = plotPoints(*(data->RR_x), *(data->RR_y), data->fftSamplesX,
                                 data->fftSamplesY, data->interpolantX, data->interpolantY);
    ui->scrollAreaRR->setWidget(plotRR);
    ui->scrollAreaRR->show();

    //Fourier    
    QwtPlot *plotFT = plotLogPlotF(*(data->fft_x), *(data->fft_y), 2);
    ui->scrollAreaFT->setWidget(plotFT);
    ui->scrollAreaFT->show();

    //Frequency Coefficients
    ui->TP->setText("TP=" %QString::number(((long)data->TP), 'f', 2));
    ui->HF->setText("HF=" %QString::number(((long)data->HF), 'f', 2));
    ui->LF->setText("LF=" %QString::number(((long)data->LF), 'f', 2));
    ui->VLF->setText("VLF=" %QString::number(((long)data->VLF), 'd', 2));
    ui->ULF->setText("ULF=" %QString::number(((long)data->ULF), 'c', 2));
    ui->LFHF->setText("LFHF=" %QString::number(100*(data->LFHF), 'f', 2) + "%");
}

void AirEcgMain::drawHrv2(EcgData *data)
{
    QwtPlot *plotHistogram = plotBarChart(*(data->histogram_x), *(data->histogram_y));
    ui->scrollAreaHistogram->setWidget(plotHistogram);
    ui->scrollAreaHistogram->show();

    ui->triangularIndex->setText("Triangular index = " % QString::number(*(data->triangularIndex), 'f', 2));
    ui->TINN->setText("  TINN = " % QString::number(*(data->TINN), 'f', 2));

   //TMP
 //   QwtPlot *plotFT = plotPointsPlot(*(data->fft_x), *(data->fft_y));
//    ui->scrollAreaFT->setWidget(plotFT);
  //  ui->scrollAreaFT->show();

    // HRV2: poincare
    QwtPlot *plotPoincare = plotPoincarePlot(*(data->poincare_x), *(data->poincare_y), *(data->SD1), *(data->SD2));
    ui->scrollAreaPoincare->setWidget(plotPoincare);
    ui->scrollAreaPoincare->show();
    ui->SD1->setText("SD1 = " % QString::number(*(data->SD1), 'f', 2));
    ui->SD2->setText("  SD2 = " % QString::number(*(data->SD2), 'f', 2));
}

void AirEcgMain::drawStInterval(EcgData *data)
{

    QwtPlot *plotX = plotIntervalPlot(*(data->ecg_baselined_mv), *(data->STbegin_x_probki), *(data->STend_x_probki), 360.0);

    ui->stIntervalArea->setWidget(plotX);
    ui->stIntervalArea->show();
}

void AirEcgMain::drawHrvDfa(EcgData *data)
{
//    //DFA
    QwtPlot *plotFluctuaction =plotPointsPlotDFA(*(data->trend_y), *(data->trend_x), *(data->wsp_a), *(data->wsp_b));
    ui->scrollAreaFlu->setWidget(plotFluctuaction);
    ui->scrollAreaFlu->show();

    QwtPlot *plotTrend=plotPlotdfa(*(data->trend_z),*(data->trend_v));
    ui->scrollAreaTrend->setWidget(plotTrend);
    ui->scrollAreaTrend->show();

       ui->window_min->setText("window_min = " % QString::number(*(data->window_min), 'd', 0));
       ui->window_max->setText("window_max = " % QString::number(*(data->window_max), 'd', 0));
       ui->alfa2->setText("alfa = " % QString::number(*(data->alfa), 'f', 2));
       ui->window_plot->setText("box_plot = " % QString::number(*(data->window_plot), 'd', 0));
       ui->boxes2->setText("Boxes: " % QString::number(*(data->boxes), 'd', 0));
}

void AirEcgMain::drawTwa(EcgData *data)
{
    QwtPlot *plotTWA = plotTWAPlot(*(data->ecg_baselined),*(data->TWA_positive),*(data->TWA_negative),data->info->frequencyValue);
    ui->scrollAreaTwave->setWidget(plotTWA);
    ui->scrollAreaTwave->show();
    QwtPlot *plotTWA2 = plotTWAPlot2(*(data->TWA_positive), *(data->TWA_positive_value), *(data->TWA_negative), *(data->TWA_negative_value));
    ui->scrollAreaTwave_2->setWidget(plotTWA2);
    ui->scrollAreaTwave_2->show();

    if (*(data->twa_num)>0) {
        ui->twa_result->setText("Positive");
        ui->twa_num->setText(QString::number(*(data->twa_num),'d',0));
    } else {
        ui->twa_result->setText("Negative");
        ui->twa_num->setText("0");
    }
    ui->twa_value->setText(QString::number(*(data->twa_highest_val),'f',4));
}

void AirEcgMain::drawWaves(EcgData *data)
{
    QwtPlot *wavesPlot = plotWavesPlot(*(data->GetCurrentSignal()), *(data->waves), 360.0);
    ui->scrollAreaWaves->setWidget(wavesPlot);
    ui->scrollAreaWaves->show();
}

void AirEcgMain::drawQrsClass(EcgData *data)
{
    this->resetQrsToolbox(data);
}

void AirEcgMain::drawHrt(EcgData *data)
{
    // rozrysuj hrt tachogram
    // wyswietl wyniki obliczen
    QwtPlot *hrtTachogram = plotHrt(*(data->hrt_tachogram));
    ui->scrollAreaHrt->setWidget(hrtTachogram);
    ui->scrollAreaHrt->show();
    ui->vpbs_detected_count->setText(QString::number(*(data->vpbs_detected_count), 'f', 0));
    ui->turbulence_onset_val->setText(QString::number(*(data->turbulence_onset), 'f', 2));
    ui->turbulence_slope_val->setText(QString::number(*(data->turbulence_slope), 'f', 2));
}

void AirEcgMain::receiveResults(EcgData *data)
{
    this->drawEcgBaseline(data);
    this->drawRPeaks(data);
    this->drawHrv1(data);
    this->drawHrv2(data);
    this->drawHrvDfa(data);
    this->drawTwa(data);
    this->drawWaves(data);
    this->drawQrsClass(data);
    this->drawStInterval(data);
    this->drawHrt(data);
    emit this->closeDialog();
    return;
}

void AirEcgMain::resetQrsToolbox(EcgData *data)
{
    this->tScale = 1000/data->info->frequencyValue;
    ui->stackedWidget->setCurrentIndex(0);
    ui->qrsSettingsResultsButton->setEnabled(1);
    int items = ui->QRSClassesToolBox->count();
    for(int i = 0 ; i < items; i++)
    {
        ui->QRSClassesToolBox->removeItem(0);
    }

    for(int i = 0 ; i < data->classes->count(); i++)
    {
        QString labelText = "";
        labelText.append(data->classes->at(i).classLabel);
        labelText.append("[ ").append(QString::number(data->classes->at(i).classMembers->count())).append(" ]");
        QListWidget* listView = new QListWidget();

        int classMembersCnt = data->classes->at(i).classMembers->count();
        for(int j = 0 ; j < classMembersCnt; j++)
        {
            QString itemText = QString("QRS No. ").append(QString::number(data->classes->at(i).classMembers->at(j)));
            QListWidgetItem* item = new QListWidgetItem(itemText, listView, QListWidgetItem::UserType);
            item->setWhatsThis(QString::number(data->classes->at(i).classMembers->at(j)));
        }

        this->connect(listView, SIGNAL(currentTextChanged(QString)),this,SLOT(qrssample_changed(QString)));
        listView->setMinimumHeight(100);
        ui->QRSClassesToolBox->addItem(listView,labelText);
    }
}

void AirEcgMain::receiveQRSData(QRSClass currClass, int type)
{
    QVector<double> xAxis;
    QVector<double> qrsSegment = *(currClass.representative);

    if (qrsSegment.count() < 2)
    {
        QLabel* label = new QLabel("Zero length QRS Segment");
        label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        while (ui->ClassesPlotLayout->count() != 0)
        {
            QLayoutItem *item = ui->ClassesPlotLayout->itemAt(0);

            if (item != 0)
            {
                if (item->widget())
                {
                    QWidget* widget = item->widget();
                    ui->ClassesPlotLayout->removeWidget(item->widget());
                    delete widget;
                }
                else
                {
                    ui->ClassesPlotLayout->removeItem(item);
                    delete item;
                }
            }
        }
        ui->ClassesPlotLayout->addWidget(label);
        this->qrsClassPlot = NULL;
        this->currentQrsClassPlot = NULL;
        return;
    }

    double yMax = qrsSegment.first();
    double yMin = qrsSegment.first();
    double xMax = 0;
    double xMin = 0;

    if (this->currentQrsClassPlot != NULL && type != 1)
    {
        yMax = this->currentQrsClassPlot->maxYValue();
        yMin = this->currentQrsClassPlot->minYValue();
        xMax = this->currentQrsClassPlot->maxXValue();
    }

    if (xMax < qrsSegment.count())
        xMax = qrsSegment.count();

    double signalMax = qrsSegment.first();
    int maxIndex = 0;

    for(int i = 0 ; i < qrsSegment.count(); i++)
    {
        if (yMax < qrsSegment.at(i))
            yMax = qrsSegment.at(i);

        if (signalMax < qrsSegment.at(i))
        {
            signalMax = qrsSegment.at(i);
            maxIndex = i;
        }
        if (yMin > qrsSegment.at(i))
            yMin = qrsSegment.at(i);

        xAxis.append(i*this->tScale);
    }

    QwtPlotCurve *curve = new QwtPlotCurve();

    if (type == 1)
    {
        curve->setPen(QPen(Qt::red,1));
        curve->setTitle("Class Representative");
        this->currentQrsClassPlot = curve;
        this->qrsClassCurveMaxIndex = maxIndex;
    }
    else
    {
        if(currentQrsClassPlot != NULL)
        {
            int delta = this->qrsClassCurveMaxIndex - maxIndex;

            for(int j = 0 ; j < xAxis.count(); j++)
            {
                xAxis[j] += delta*tScale;
            }

            if (delta < 0)
                xMin += delta;

            xMax = qrsSegment.count() + delta > this->currentQrsClassPlot->dataSize() ? qrsSegment.count() + delta : this->currentQrsClassPlot->dataSize();
            this->currentQrsClassPlot->attach(this->qrsClassPlot);
        }

        curve->setPen(QPen(Qt::blue));
        curve->setTitle("Selected QRS Segment");
    }

    curve->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    curve->setSamples(xAxis,qrsSegment);
    curve->attach(this->qrsClassPlot);
    this->qrsClassPlot->setAxisScale( QwtPlot::yLeft, yMin, yMax );
    this->qrsClassPlot->setAxisScale( QwtPlot::xBottom , xMin*this->tScale, xMax*this->tScale);

    this->populareQRSClassBox(currClass, type);
}

void AirEcgMain::populareQRSClassBox(QRSClass currentClass, int type)
{
    QGridLayout* layout;

    if (type == 1)
        layout = ui->QRSGridLayout;
    else
        return;

    for(int i = 0; i < currentClass.features->count(); i++)
    {
        QLabel* label = new QLabel(currentClass.featureNames->at(i));
        label->setToolTip(currentClass.featureTooltip->at(i));
        QLabel* label2 = new QLabel(QString::number(currentClass.features->at(i)));

        for(int j = 0; j < 2; j++)
        {
            QLayoutItem* item = layout->itemAtPosition(3+i,j);

            if (item != 0)
            {
                if (item->widget())
                {
                    QWidget* widget = item->widget();
                    layout->removeWidget(item->widget());
                    delete widget;
                }
                else
                {
                    layout->removeItem(item);
                    delete item;
                }
            }
        }

        layout->addWidget(label,3 + i,0,1,1);
        layout->addWidget(label2,3 + i,1,1,1);
    }
}

void AirEcgMain::receiveSingleProcessingResult(bool succeeded, EcgData *data)
{
    if(succeeded)
    {
        if(this->hash =="ECG_BASELINE")
        {
            this->drawEcgBaseline(data);
        }
        else if(this->hash == "R_PEAKS")
        {
            this->drawRPeaks(data);
        }
        else if(this->hash == "WAVES")
        {
            this->drawWaves(data);
        }
        else if(this->hash == "HRV1")
        {
            this->drawHrv1(data);
        }
        else if(this->hash == "HRV2")
        {
            this->drawHrv2(data);
        }
        else if(this->hash == "HRV_DFA")
        {
            this->drawHrvDfa(data);
        }
        else if(this->hash == "QRS_CLASS")
        {
            this->drawQrsClass(data);
        }
        else if(this->hash == "T_WAVE_ALT")
        {
            this->drawTwa(data);
        }
    }
    else
    {
        //Tutaj mozna wypisac, ze modul nie moze byc przetworzony bo nie jest gotowy
        notready dialogNotReady;
        dialogNotReady.setModal(true);
        dialogNotReady.exec();
    }
}


void AirEcgMain::on_pushButton_2_clicked()
{        
    emit this->runEcgBaseline();
}

void AirEcgMain::on_pushButton_3_clicked()
{
    //this->hash = "R_PEAKS";
    //emit this->runSingle(this->hash);
    emit this->runAtrialFibr();
}

void AirEcgMain::on_pushButton_5_clicked()
{
    this->hash = "WAVES";
    emit this->runSingle(this->hash);
}

void AirEcgMain::on_pushButton_6_clicked()
{
    this->hash = "HRV1";
    emit this->runSingle(this->hash);
}

void AirEcgMain::on_pushButton_7_clicked()
{
    this->hash = "HRV2";
    emit this->runSingle(this->hash);
}

void AirEcgMain::on_pushButton_8_clicked()
{
    this->hash = "HRV_DFA";
    emit this->runSingle(this->hash);
}

void AirEcgMain::on_pushButton_9_clicked()
{
    this->hash = "T_WAVE_ALT";
    emit this->runSingle(this->hash);
}

void AirEcgMain::on_pushButton_10_clicked()
{
    this->hash = "QRS_CLASS";
    emit this->runSingle(this->hash);
}

void AirEcgMain::on_pushButton_12_clicked()
{
    emit this->run();
    ui->progressBar->setVisible(true);
}

void AirEcgMain::on_checkBox_toggled(bool checked)
{
    if (checked)
        emit this->switchTWA(1);
    else
        emit this->switchTWA(0);
}

void AirEcgMain::on_qrsSettingsResultsButton_clicked()
{
    this->ui->stackedWidget->setCurrentIndex(0);
}

void AirEcgMain::on_qrscClassSettingsButton_clicked()
{
    this->ui->stackedWidget->setCurrentIndex(1);
}

void AirEcgMain::on_comboBox_currentIndexChanged(int index)
{
    this->ui->qrsClustererSettingsStackWidget->setCurrentIndex(index);

    if(index == 0)
    {
        emit qrsClustererChanged(KMeansClusterer);
        emit qrsMaxIterationsChanged(ui->qrsSetKMaxIterSpinBox->value());
        emit qrsParallelExecutionChanged(ui->qrsSetKMeansParallelCheckBox->isEnabled());
        emit qrsKClustersNumberChanged(ui->qrsSetKClusterNumSpinBox->value());
    }
    else
    {
        emit qrsClustererChanged(GMeansClusterer);
        emit qrsMaxIterationsChanged(ui->qrsSetGMaxItersSpinBox->value());
        emit qrsGMaxKIterations(ui->qrsSetGinKMaxIterations->value());
        emit qrsGMinClustersChanged(ui->qrsSetGMinClusterSpinBox->value());
        emit qrsGMaxClustersChanged(ui->qrsSetGMaxClusterSpinBox->value());
        emit qrsParallelExecutionChanged(ui->qrsSettingsGMeansParallelCheckBox->isEnabled());
    }
}

void AirEcgMain::on_qrsSetGMaxItersSpinBox_valueChanged(int arg1)
{
    emit qrsMaxIterationsChanged(arg1);
}

void AirEcgMain::on_qrsSetGinKMaxIterations_valueChanged(int arg1)
{
    emit qrsGMaxKIterations(arg1);
}

void AirEcgMain::on_qrsSetGMinClusterSpinBox_valueChanged(int arg1)
{
    ui->qrsSetGMaxClusterSpinBox->setMinimum(arg1);

    emit qrsGMinClustersChanged(arg1);
}

void AirEcgMain::on_qrsSetGMaxClusterSpinBox_valueChanged(int arg1)
{
    emit qrsGMaxClustersChanged(arg1);
}

void AirEcgMain::on_qrsSettingsGMeansParallelCheckBox_toggled(bool checked)
{
    emit qrsParallelExecutionChanged(checked);
}

void AirEcgMain::on_qrsSetKMaxIterSpinBox_valueChanged(int arg1)
{
    emit qrsMaxIterationsChanged(arg1);
}

void AirEcgMain::on_qrsSetKClusterNumSpinBox_valueChanged(int arg1)
{
    emit qrsKClustersNumberChanged(arg1);
}

void AirEcgMain::on_qrsSetKMeansParallelCheckBox_toggled(bool checked)
{
    emit qrsParallelExecutionChanged(checked);
}

void AirEcgMain::on_radioButton_clicked()
{
    emit this->switchSignal(0);
}

void AirEcgMain::on_radioButton_2_clicked()
{
    emit this->switchSignal(1);
}

void AirEcgMain::on_p_onset_toggled(bool checked)
{
    emit this->switchWaves_p_onset(checked);
}
 void  AirEcgMain::on_butterworthRadioButton_clicked()
 {

 }
