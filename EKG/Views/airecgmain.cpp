#include "airecgmain.h"
#include "ui_airecgmain.h"
#include "about.h"
#include "filebrowser.h"
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
#include <qwt_curve_fitter.h>

#include <qwt_scale_engine.h>
#include <qpainter.h>
#include <qwt_plot_layout.h>
#include <qwt_scale_draw.h>
#include <qwt_scale_widget.h>
#include <qwt_legend_item.h>
#include "time_scal.h"

#include "ECG_BASELINE/src/butter.h"

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

    this->stIntervalPlot = NULL;
    this->stIntervalZoomer = NULL;

    connect(ui->butterworthRadioButton, SIGNAL(clicked()), baselineSignalMapper, SLOT(map()));
    connect(ui->movingAverageRadioButton, SIGNAL(clicked()), baselineSignalMapper, SLOT(map()));
    connect(ui->savitzkyGolayRadioButton, SIGNAL(clicked()), baselineSignalMapper, SLOT(map()));
    connect(ui->kalmanRadioButton      , SIGNAL(clicked()), baselineSignalMapper, SLOT(map()));
    baselineSignalMapper->setMapping(ui->butterworthRadioButton, 0);
    baselineSignalMapper->setMapping(ui->movingAverageRadioButton, 1);
    baselineSignalMapper->setMapping(ui->savitzkyGolayRadioButton, 2);
    baselineSignalMapper->setMapping(ui->kalmanRadioButton       , 3);
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
    ui->busy_label->setVisible(false);
    initEcgBaselineGui();
    initStIntervalGui();
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
    emit this->busy(true);
    fileBrowser dialogFileBrowser;
    this->connect(&dialogFileBrowser, SIGNAL(fbLoadEntity(QString,QString)), this, SLOT(fbLoadData(QString,QString)));
    dialogFileBrowser.setModal(true);
    dialogFileBrowser.exec();
    emit this->busy(false);
}

void AirEcgMain::fbLoadData(const QString &directory, const QString &name)
{
    emit loadEntity(directory, name);
    ui->pushButton_2->setEnabled(true);
   // ui->pushButton_3_2->setEnabled(true);
    ui->pushButton_5->setEnabled(true);
    ui->pushButton_6->setEnabled(true);
    ui->pushButton_10->setEnabled(true);
    ui->pushButton_12->setEnabled(true);
   ui->rpeaksGroupBox_2->setEnabled(true);
    ui->baselineGroupBox->setEnabled(true);
    ui->tabWidget_5->setEnabled(true);
    ui->tabHrv->setEnabled(true);
    ui->pushButton_18->setEnabled(true);

    ui->qrsClustererSettingsGroupBox->setEnabled(true);
    ui->qrsClustererSettingsGroupBox->setToolTip("");
    ui->qrsFeaturesSettingsGroupBox->setEnabled(true);
    ui->qrsFeaturesSettingsGroupBox->setToolTip("");
}

void AirEcgMain::receivePatientData(EcgData *data)
{
    currentEcgData = data;

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
void AirEcgMain::on_Falkowa_radiobutton_clicked()
{
    emit this->switchRPeaks(3);
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

class TempScaleDraw: public QwtScaleDraw
{
public:
    TempScaleDraw()
    {
        setTickLength( QwtScaleDiv::MajorTick, 10 );
        setTickLength( QwtScaleDiv::MinorTick, 10 );
        setTickLength( QwtScaleDiv::MediumTick, 10 );

        setSpacing( 5 );
    }
};
QwtPlot* AirEcgMain::plotPlot(const QVector<double>& yData, float freq)
{
    QVector<double> sampleNo = QVector<double>(yData.size());

    double max = yData.first();
    double min = yData.first();

    double tos=1/freq;

    for (int i = 0; i < yData.size(); ++i)
    {
        sampleNo[i] = i*tos*1000; //*1000 przeliczenie na ms
        max = qMax(max, yData.at(i));
        min = qMin(min, yData.at(i));
    }

    QwtPlot* plot = new QwtPlot();
    plot->setCanvasBackground(Qt::white);
    plot->setAxisScale(QwtPlot::yLeft, min, max);
    plot->setAxisScale( QwtPlot::xBottom , 0, 4000.0);
    plot->setAxisScaleDraw( QwtPlot::xBottom, new TimeScaleDraw( QTime() ) );
    plot->axisAutoScale(QwtPlot::xBottom);
     QwtText xaxis("Time [mm:ss:zzz]");
    QwtText yaxis("Voltage [mV]");
    xaxis.setFont(QFont("Arial", 8));
    yaxis.setFont(QFont("Arial", 8));

    plot->setAxisTitle( QwtPlot::yLeft, yaxis );
    plot->setAxisTitle( QwtPlot::xBottom, xaxis );

    QwtPlotGrid* grid = new QwtPlotGrid();
    grid->setPen(QPen(QColor(255, 0, 0 ,127)));
    grid->enableYMin(true);
    grid->enableXMin(true);
    grid->setMajPen(QPen(Qt::red, 2, Qt::SolidLine));
    grid->setMinPen(QPen(Qt::red, 0 , Qt::SolidLine));

    grid->attach(plot);

    QwtPlotCurve* curve = new QwtPlotCurve();
    curve->setPen(QPen(Qt::blue, 2));
    curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    curve->setSamples(sampleNo, yData);
    curve->attach(plot);

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

QwtPlot* AirEcgMain::plotPlot_SIG_EDR(const QVector<QVector<double>::const_iterator> &p,const QVector<double>& yData,const QVector<double>& yData1,const QVector<double>& yData2, float freq, unsigned int no)
{
    double tos=1/freq;
    double max=0;
    double min=9999999;

    if(no == 0 || no == 2 )
    {
        for (int i = 0; i < yData2.size(); ++i)
        {
            max = qMax(max, yData2.at(i));
            min = qMin(min, yData2.at(i));
        }
        QLOG_TRACE() <<"SIGEDR:size1 = "<< QString::number(yData1.size());
    }
    if(no == 1 || no == 2 )
    {
        for (int i = 0; i < yData1.size(); ++i)
        {
            max = qMax(max, yData1.at(i));
            min = qMin(min, yData1.at(i));
        }
        QLOG_TRACE() <<"SIGEDR:size2 = "<< QString::number(yData2.size());
    }
    QVector<double> pDataX = QVector<double>(p.size());
    for (int i=0;i<p.size();i++)
    {
        pDataX[i] = ((unsigned int)(p.at(i)- yData.begin())*tos*1000);
    }

    QLOG_TRACE() <<"SIGEDR:MIN = "<< QString::number(min);
    QLOG_TRACE() <<"SIGEDR:MAX = "<< QString::number(max);

    QwtPlot* plot = new QwtPlot();
    plot->setCanvasBackground(Qt::white);
    plot->setAxisScale(QwtPlot::yLeft, min, max,0);
    plot->setAxisScale( QwtPlot::xBottom , 0, 4000.0);

    plot->setAxisScaleDraw( QwtPlot::xBottom, new TimeScaleDraw( QTime() ) );
    plot->axisAutoScale(QwtPlot::xBottom);
    QwtText xaxis("Time [mm:ss:zzz]");
    QwtText yaxis("Voltage [mV]");
    xaxis.setFont(QFont("Arial", 8));
    yaxis.setFont(QFont("Arial", 8));

    plot->setAxisTitle( QwtPlot::yLeft, yaxis );
    plot->setAxisTitle( QwtPlot::xBottom, xaxis );

    QwtPlotGrid* grid = new QwtPlotGrid();
    grid->setPen(QPen(QColor(255, 0, 0 ,127)));
    grid->enableYMin(true);
    grid->enableXMin(true);
    grid->setMajPen(QPen(Qt::red, 2, Qt::SolidLine));
    grid->setMinPen(QPen(Qt::red, 0 , Qt::SolidLine));
    grid->attach(plot);

    if(no == 1 || no == 2)
    {
        QwtPlotCurve *curve1 = new QwtPlotCurve();
        QwtSplineCurveFitter *fitter = new QwtSplineCurveFitter();
        fitter->setFitMode(QwtSplineCurveFitter::Spline);
        curve1->setSamples(pDataX, yData1);//pointsF);
        curve1->setRenderHint(QwtPlotItem::RenderAntialiased, true);
        curve1->setTitle("SIG EDR Rpeak");
        curve1->setStyle(QwtPlotCurve::Lines);
        curve1->setPen(QPen(Qt::blue, 2));
        curve1->setCurveAttribute(QwtPlotCurve::Fitted, true);
        curve1->attach(plot);
        fitter->setSplineSize(60000);
        curve1->setCurveFitter(fitter);
    }

    if(no == 0 || no == 2)
    {
        QwtPlotCurve *curve3 = new QwtPlotCurve();
        QwtSplineCurveFitter *fitter = new QwtSplineCurveFitter();
        fitter->setFitMode(QwtSplineCurveFitter::Spline);
        curve3->setSamples(pDataX, yData2);//pointsF);
        curve3->setRenderHint(QwtPlotItem::RenderAntialiased, true);
        curve3->setTitle("SIG EDR Wavse");
        curve3->setStyle(QwtPlotCurve::Lines);
        curve3->setPen(QPen(Qt::green, 2));
        curve3->setCurveAttribute(QwtPlotCurve::Fitted, true);
        curve3->attach(plot);
        fitter->setSplineSize(60000);
        curve3->setCurveFitter(fitter);
    }

    QwtLegend* legend = new QwtLegend();
    legend->setItemMode(QwtLegend::ReadOnlyItem);
    plot->insertLegend(legend, QwtPlot::BottomLegend);

    zoom = new ScrollZoomer(plot->canvas());
    zoom->setRubberBandPen(QPen(Qt::white));
    //zoom->setZoomBase( false );
    plot->canvas()->setGeometry(0,0,pDataX.last(),0);
    zoom->setZoomBase(plot->canvas()->rect());

    QwtPlotPanner* panner = new QwtPlotPanner(plot->canvas());
    panner->setMouseButton(Qt::MidButton);
    panner->setOrientations(Qt::Horizontal);

    QwtPlotMagnifier* magnifier = new QwtPlotMagnifier(plot->canvas());
    magnifier->setAxisEnabled(QwtPlot::yLeft, false);

    return plot;
}
//HRV1
QwtPlot* AirEcgMain::plotPlot(const QVector<double>& yData,const QVector<double>& xData)
{
    double maxy = yData.first();
    double miny = yData.first();

    for (int i = 0; i < yData.size(); ++i)
    {
        maxy = qMax(maxy, yData.at(i));
        miny = qMin(miny, yData.at(i));
    }
    double maxx = xData.first();
    double minx = xData.first();

    for (int i = 0; i < yData.size(); ++i)
    {
        maxx = qMax(maxx, xData.at(i));
        minx = qMin(minx, xData.at(i));
    }
    if(maxy>10000)
        maxy=10000;
    if(miny<-10000)
        miny=-10000;
    QwtPlot* plot = new QwtPlot();
    plot->setCanvasBackground(Qt::white);
    plot->setAxisScale(QwtPlot::yLeft, miny,maxy);
    plot->setAxisScale( QwtPlot::xBottom ,minx , maxx);

    QwtText xaxis("Frequence [Hz]");
    QwtText yaxis("Power [ms^2]");
    xaxis.setFont(QFont("Arial", 8));
    yaxis.setFont(QFont("Arial", 8));

    plot->setAxisTitle( QwtPlot::yLeft, yaxis );
    plot->setAxisTitle( QwtPlot::xBottom, xaxis );

    QwtPlotGrid* grid = new QwtPlotGrid();
    grid->setPen(QPen(QColor(255, 0, 0 ,127)));
    grid->enableYMin(true);
    grid->enableXMin(true);
    grid->setMajPen(QPen(Qt::red, 2, Qt::SolidLine));
    grid->setMinPen(QPen(Qt::red, 0 , Qt::SolidLine));
    grid->attach(plot);

    QwtPlotCurve* curve = new QwtPlotCurve();
    curve->setPen(QPen(Qt::blue, 2));
    curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    curve->setSamples(xData, yData);
    curve->attach(plot);

    zoom = new ScrollZoomer(plot->canvas());
    zoom->setRubberBandPen(QPen(Qt::white));
    //zoom->setZoomBase( false );
    plot->canvas()->setGeometry(0,0,xData.last(),0);
    zoom->setZoomBase(plot->canvas()->rect());

    QwtPlotPanner* panner = new QwtPlotPanner(plot->canvas());
    panner->setMouseButton(Qt::MidButton);
    panner->setOrientations(Qt::Horizontal);

    QwtPlotMagnifier* magnifier = new QwtPlotMagnifier(plot->canvas());
    magnifier->setAxisEnabled(QwtPlot::yLeft, false);

    return plot;
}
//Sleap
QwtPlot* AirEcgMain::plotSleep_Apnea(const QVector<double>& yData,const QVector<double>& xData, double threshold, QVector<BeginEndPair> sleep_apnea_pairs)
{
    double maxy = yData.first();
    double miny = yData.first();

    for (int i = 0; i < yData.size(); ++i)
    {
        maxy = qMax(maxy, yData.at(i));
        miny = qMin(miny, yData.at(i));
    }
    double maxx = xData.first();
    double minx = xData.first();

    for (int i = 0; i < yData.size(); ++i)
    {
        maxx = qMax(maxx, xData.at(i));
        minx = qMin(minx, xData.at(i));
    }


    QwtPlot* plot = new QwtPlot();
    plot->setCanvasBackground(Qt::white);
    plot->setAxisScale(QwtPlot::yLeft, miny,maxy);
    plot->setAxisScale( QwtPlot::xBottom ,minx , maxx);
    plot->setTitle(QwtText("Normalised Hilbert amplitude and Apnea Detections"));
    QwtText xaxis("Samples ");
    QwtText yaxis("Normalised Hilbert amplitude ");
    xaxis.setFont(QFont("Arial", 8));
    yaxis.setFont(QFont("Arial", 8));

    plot->setAxisTitle( QwtPlot::yLeft, yaxis );
    plot->setAxisTitle( QwtPlot::xBottom, xaxis );
    plot->setFixedHeight(230);
    QwtPlotGrid* grid = new QwtPlotGrid();
    grid->setPen(QPen(QColor(255, 0, 0 ,127)));
    grid->enableYMin(true);
    grid->enableXMin(true);
    grid->setMajPen(QPen(Qt::red, 2, Qt::SolidLine));
    grid->setMinPen(QPen(Qt::red, 0 , Qt::SolidLine));
    grid->attach(plot);

    QwtPlotCurve* curve = new QwtPlotCurve();
    curve->setPen(QPen(Qt::blue, 2));
    curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    curve->setSamples(xData, yData);
    curve->attach(plot);


    QwtPlotMarker *mY = new QwtPlotMarker();
    mY->setLineStyle( QwtPlotMarker::HLine );
    mY->setLinePen( QPen( Qt::green, 2, Qt::SolidLine ) );
    mY->setLabel(QwtText("Minimum Hilbert amplitude"));
    mY->setLabelOrientation(Qt::Horizontal);
    mY->setLabelAlignment(Qt::AlignTop);
    mY->setYValue( threshold);
    mY->attach( plot );

    QVector<QwtPlotMarker*> marker;
    QLOG_TRACE() <<"Sleep size= "<< QString::number( sleep_apnea_pairs.size());

    for(int i=0;i<sleep_apnea_pairs.size();i++)
    {
        marker.append(new QwtPlotMarker);
        marker.at(i)->setLineStyle( QwtPlotMarker::VLine );
        marker.at(i)->setLinePen( QPen( Qt::black, 2, Qt::SolidLine ) );
        marker.at(i)->setXValue( sleep_apnea_pairs[i].first);
        marker.at(i)->attach( plot );
        marker.append(new QwtPlotMarker);
        marker.at(i+1)->setLineStyle( QwtPlotMarker::VLine );
        marker.at(i+1)->setLinePen( QPen( Qt::black, 2, Qt::SolidLine ) );
        marker.at(i+1)->setXValue( sleep_apnea_pairs[i].first);
        marker.at(i+1)->attach( plot );

        QLOG_TRACE() <<"Sleep 1= "<< QString::number( sleep_apnea_pairs[i].first)<< "2 = " << QString::number( sleep_apnea_pairs[i].second);
    }

    zoom = new ScrollZoomer(plot->canvas());
    zoom->setRubberBandPen(QPen(Qt::white));
    //zoom->setZoomBase( false );
    plot->canvas()->setGeometry(0,0,xData.last(),0);
    zoom->setZoomBase(plot->canvas()->rect());

    QwtPlotPanner* panner = new QwtPlotPanner(plot->canvas());
    panner->setMouseButton(Qt::MidButton);
    panner->setOrientations(Qt::Horizontal);

    QwtPlotMagnifier* magnifier = new QwtPlotMagnifier(plot->canvas());
    magnifier->setAxisEnabled(QwtPlot::yLeft, false);

    return plot;
}
//Sleap Feq
QwtPlot* AirEcgMain::plotSleep_ApneaFreq(const QVector<double>& yData,const QVector<double>& xData, double threshold, QVector<BeginEndPair> sleep_apnea_pairs)
{
    double maxy = yData.first();
    double miny = yData.first();

    for (int i = 0; i < yData.size(); ++i)
    {
        maxy = qMax(maxy, yData.at(i));
        miny = qMin(miny, yData.at(i));
    }
    double maxx = xData.first();
    double minx = xData.first();

    for (int i = 0; i < yData.size(); ++i)
    {
        maxx = qMax(maxx, xData.at(i));
        minx = qMin(minx, xData.at(i));
    }


    QwtPlot* plot = new QwtPlot();
    plot->setCanvasBackground(Qt::white);
    plot->setAxisScale(QwtPlot::yLeft, miny,maxy);
    plot->setAxisScale( QwtPlot::xBottom ,minx , maxx);
    plot->setTitle(QwtText("Hilbert frequency and Apnea Detections"));
    plot->setFixedHeight(230);
    QwtText xaxis("Samples ");
    QwtText yaxis("Hilbert frequency [Hz] ");
    xaxis.setFont(QFont("Arial", 8));
    yaxis.setFont(QFont("Arial", 8));

    plot->setAxisTitle( QwtPlot::yLeft, yaxis );
    plot->setAxisTitle( QwtPlot::xBottom, xaxis );

    QwtPlotGrid* grid = new QwtPlotGrid();
    grid->setPen(QPen(QColor(255, 0, 0 ,127)));
    grid->enableYMin(true);
    grid->enableXMin(true);
    grid->setMajPen(QPen(Qt::red, 2, Qt::SolidLine));
    grid->setMinPen(QPen(Qt::red, 0 , Qt::SolidLine));
    grid->attach(plot);

    QwtPlotCurve* curve = new QwtPlotCurve();
    curve->setPen(QPen(Qt::blue, 2));
    curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    curve->setSamples(xData, yData);
    curve->attach(plot);


    QwtPlotMarker *mY = new QwtPlotMarker();
    mY->setLineStyle( QwtPlotMarker::HLine );
    mY->setLinePen( QPen( Qt::green, 2, Qt::SolidLine ) );
    mY->setLabel(QwtText("Maximum Hilbert frequency"));
    mY->setLabelOrientation(Qt::Horizontal);
    mY->setLabelAlignment(Qt::AlignTop);
    mY->setYValue( threshold);
    mY->attach( plot );

    QVector<QwtPlotMarker*> marker;
    QLOG_TRACE() <<"Sleep size= "<< QString::number( sleep_apnea_pairs.size());

    for(int i=0;i<sleep_apnea_pairs.size();i++)
    {
        marker.append(new QwtPlotMarker);
        marker.at(i)->setLineStyle( QwtPlotMarker::VLine );
        marker.at(i)->setLinePen( QPen( Qt::black, 2, Qt::SolidLine ) );
        marker.at(i)->setXValue( sleep_apnea_pairs[i].first);
        marker.at(i)->attach( plot );
        marker.append(new QwtPlotMarker);
        marker.at(i+1)->setLineStyle( QwtPlotMarker::VLine );
        marker.at(i+1)->setLinePen( QPen( Qt::black, 2, Qt::SolidLine ) );
        marker.at(i+1)->setXValue( sleep_apnea_pairs[i].first);
        marker.at(i+1)->attach( plot );

        QLOG_TRACE() <<"Sleep 1= "<< QString::number( sleep_apnea_pairs[i].first)<< "2 = " << QString::number( sleep_apnea_pairs[i].second);
    }

    zoom = new ScrollZoomer(plot->canvas());
    zoom->setRubberBandPen(QPen(Qt::white));
    //zoom->setZoomBase( false );
    plot->canvas()->setGeometry(0,0,xData.last(),0);
    zoom->setZoomBase(plot->canvas()->rect());

    QwtPlotPanner* panner = new QwtPlotPanner(plot->canvas());
    panner->setMouseButton(Qt::MidButton);
    panner->setOrientations(Qt::Horizontal);

    QwtPlotMagnifier* magnifier = new QwtPlotMagnifier(plot->canvas());
    magnifier->setAxisEnabled(QwtPlot::yLeft, false);

    return plot;
}
//RR
QwtPlot* AirEcgMain::plotPlotRR(const QVector<double>& yData,const QVector<double>& xData)
{
    double maxy = 0;
    double miny = 9999999;

    for (int i = 0; i < yData.size(); ++i)
    {
        maxy = qMax(maxy, yData.at(i));
        miny = qMin(miny, yData.at(i));
    }
    double maxx = 0;
    double minx = 9999999;

    for (int i = 0; i < yData.size(); ++i)
    {
        maxx = qMax(maxx, xData.at(i));
        minx = qMin(minx, xData.at(i));
    }

    QwtPlot* plot = new QwtPlot();
    plot->setCanvasBackground(Qt::white);
    plot->setAxisScale(QwtPlot::yLeft, miny,maxy);
    plot->setAxisScale( QwtPlot::xBottom ,minx , maxx);

    QwtText xaxis("Time [ms]");
    QwtText yaxis("Interval length [ms]");
    xaxis.setFont(QFont("Arial", 8));
    yaxis.setFont(QFont("Arial", 8));

    plot->setAxisTitle( QwtPlot::yLeft, yaxis );
    plot->setAxisTitle( QwtPlot::xBottom, xaxis );

    QwtPlotGrid* grid = new QwtPlotGrid();
    grid->setPen(QPen(QColor(255, 0, 0 ,127)));
    grid->enableYMin(true);
    grid->enableXMin(true);
    grid->setMajPen(QPen(Qt::red, 2, Qt::SolidLine));
    grid->setMinPen(QPen(Qt::red, 0 , Qt::SolidLine));
    grid->attach(plot);

    QwtPlotCurve* curve = new QwtPlotCurve();
    curve->setPen(QPen(Qt::blue, 2));
    curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    curve->setSamples(xData, yData);
    curve->attach(plot);

    zoom = new ScrollZoomer(plot->canvas());
    zoom->setRubberBandPen(QPen(Qt::white));
    //zoom->setZoomBase( false );
    plot->canvas()->setGeometry(0,0,xData.last(),0);
    zoom->setZoomBase(plot->canvas()->rect());

    QwtPlotPanner* panner = new QwtPlotPanner(plot->canvas());
    panner->setMouseButton(Qt::MidButton);
    panner->setOrientations(Qt::Horizontal);

    QwtPlotMagnifier* magnifier = new QwtPlotMagnifier(plot->canvas());
    magnifier->setAxisEnabled(QwtPlot::yLeft, false);

    return plot;
}

QwtPlot* AirEcgMain::plotHrt(QVector<double>& yData, double a, double b)
{
    QVector<double> sampleNo = QVector<double>(yData.size());

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
    plot->setAxisScale( QwtPlot::xBottom , 1, 24,1);

    QwtText xaxis("HRT");
    QwtText yaxis("Time [ms]");
    xaxis.setFont(QFont("Arial", 8));
    yaxis.setFont(QFont("Arial", 8));

    plot->setAxisTitle( QwtPlot::yLeft, yaxis );
    plot->setAxisTitle( QwtPlot::xBottom, xaxis );


    QwtPlotCurve* curve = new QwtPlotCurve();
    curve->setPen(QPen(Qt::blue, 1));
    curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    curve->setSamples(sampleNo, yData);
    curve->attach(plot);

    QVector<double> prosta_x = QVector<double>(2);
    QVector<double> prosta_y = QVector<double>(2);
    prosta_x[0] = 0;
    prosta_x[1] = 24;

    prosta_y[0] = b;
    prosta_y[1] = a*24  + b ;

    QwtPlotCurve* curve2 = new QwtPlotCurve();
    curve2->setPen(QPen(Qt::black, 1));
    curve2->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    curve2->setSamples(prosta_x, prosta_y);
    curve2->attach(plot);
/*
    // linia laczaca 2pkty
    QwtPlotCurve *curve2 = new QwtPlotCurve();
    curve2->setPen(QPen( Qt::blue, 3));
    curve2->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    QwtSymbol *symbol = new QwtSymbol( QwtSymbol::Ellipse,
                                      QBrush( Qt::yellow ), QPen( Qt::red, 2 ), QSize( 8, 8 ) );
    curve2->setSymbol( symbol );
    QPolygonF points;
    points << S1 << S2;
    curve2->setSamples( points );
    curve2->attach( plot );
*/
    // wstawianie lini poziomej
    QwtPlotMarker *mY = new QwtPlotMarker();
    mY->setLabel( QString::fromLatin1( "label" ) );
    mY->setLabelAlignment( Qt::AlignLeft | Qt::AlignBottom );
    mY->setLabelOrientation( Qt::Horizontal );
    mY->setLineStyle( QwtPlotMarker::HLine );
    mY->setLinePen( QPen( Qt::black, 0, Qt::DashDotLine ) );
    mY->setYValue( yData[3]);
    mY->attach( plot );


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
    curve->setPen(QPen( Qt::blue, 2));

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

QwtPlot* AirEcgMain::plotPoints(QList<double> &x, QList<double> &y, QList<double> fftSamplesX,
                       QList<double> fftSamplesY, QList<double> interpolateX, QList<double> interpolateY)
{
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
    plot->setAxisScaleDraw( QwtPlot::xBottom, new TimeScaleDraw( QTime() ) );
    plot->axisAutoScale(QwtPlot::xBottom);
    QwtText xaxis("Time [mm:ss:zzz]");
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
    grid->enableYMin(true);
    grid->enableXMin(true);
    grid->setMajPen(QPen(Qt::red, 2, Qt::SolidLine));
    grid->setMinPen(QPen(Qt::red, 0 , Qt::SolidLine));
    grid->attach( plot );
    //********************

    QwtPlotCurve *curve = new QwtPlotCurve();
    curve->setPen(QPen( Qt::blue, 2));
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


//R Peaks
QwtPlot* AirEcgMain::plotPointsPlot(const QVector<QVector<double>::const_iterator> &p, const QVector<double> &yData, float freq){
    QVector<double> yDataFin = QVector<double>(yData.size());
    QVector<double> sampleNo = QVector<double>(yData.size());

    int max=yData.first();
    int min=yData.first();

    double tos=1/freq;

    for (int i=0;i<yData.size();++i)
    {
        sampleNo[i]=(i)*tos*1000;
        yDataFin[i]=yData[i];
        if (max<yData[i]) max=yData[i];
        if (min>yData[i]) min=yData[i];
    }

    QwtPlot *plot = new QwtPlot();
    plot->setCanvasBackground( Qt::white );
    plot->setAxisScale( QwtPlot::yLeft, min-1, 1+max );
    plot->setAxisScale( QwtPlot::xBottom , 0, 4000.0);

    plot->setAxisScaleDraw( QwtPlot::xBottom, new TimeScaleDraw( QTime() ) );
    plot->axisAutoScale(QwtPlot::xBottom);
    QwtText xaxis("Time [mm:ss:zzz]");
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
    grid->enableYMin(true);
    grid->enableXMin(true);
    grid->setMajPen(QPen(Qt::red, 2, Qt::SolidLine));
    grid->setMinPen(QPen(Qt::red, 0 , Qt::SolidLine));
    grid->attach( plot );

    QwtPlotCurve *curve = new QwtPlotCurve();
    curve->setPen(QPen( Qt::blue, 2));
    curve->setRenderHint( QwtPlotItem::RenderAntialiased, true );

    curve->setSamples(sampleNo,yDataFin);
    curve->attach( plot );

    QVector<double> pDataY = QVector<double>(p.size());
    QVector<double> pDataX = QVector<double>(p.size());

    // MARKERY do zaznaczania r_peaks lub innych punktow charakterystycznych    
    for (int i=0;i<p.size();i++)
    {
        pDataX[i] = ((unsigned int)(p.at(i)- yData.begin())*tos*1000);
        pDataY[i] = (*p.at(i));
        //QLOG_TRACE() <<"Rpik:X = "<< QString::number( pDataX[i])<< "Y = " << QString::number( pDataY[i]);
    }


    QwtPlotCurve *points = new QwtPlotCurve();
    QwtSymbol *marker = new QwtSymbol( QwtSymbol::Ellipse, Qt::green, QPen( Qt::green ), QSize( 10, 10 ) );
    points->setSymbol(marker);
    points->setPen( QColor( Qt::green ) );
    points->setStyle( QwtPlotCurve::NoCurve );
    points->setSamples(pDataX,pDataY);
    points->attach( plot );

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

QwtPlot *AirEcgMain::plotWavesPlot(const QVector<double> &ecgSignal, Waves_struct &ecgFrames, float samplingFrequency)
{
   // QVector<int> yData = (QVector<int>)(ecgSignal);
    QVector<double> yDataFin = QVector<double>(ecgSignal.size());
    QVector<double> sampleNo = QVector<double>(ecgSignal.size());

    double max=ecgSignal.first();
    double min=ecgSignal.first();

    double dt = 1.0/samplingFrequency;
    for (int i=0;i<ecgSignal.size();++i)
    {
        sampleNo[i]=(i)*dt*1000;// *1000 ms
        //yDataFin[i]=yData[i];
        yDataFin[i]=ecgSignal[i];
        if (max<ecgSignal[i]) max=ecgSignal[i];
        if (min>ecgSignal[i]) min=ecgSignal[i];
    }

    QwtPlot *plot = new QwtPlot();
    plot->setCanvasBackground( Qt::white );
    plot->setAxisScale( QwtPlot::yLeft, min, max );
    plot->setAxisScale( QwtPlot::xBottom , 0, 4000.0);

    QwtPlotGrid *grid = new QwtPlotGrid();
    QPen *pen = new QPen;

    QColor kolor = QColor();
    kolor.setAlpha(127);
    kolor.setBlue(0);
    kolor.setRed(255);
    kolor.setBlue(0);
    pen->setColor(kolor);
    grid->setPen( *pen );
    grid->enableYMin(true);
    grid->enableXMin(true);
    grid->setMajPen(QPen(Qt::red, 2, Qt::SolidLine));
    grid->setMinPen(QPen(Qt::red, 0 , Qt::SolidLine));
    grid->attach( plot );

    QwtPlotCurve *curve = new QwtPlotCurve();
    curve->setPen(QPen( Qt::blue, 2));
    curve->setTitle("Signal");
    curve->setRenderHint( QwtPlotItem::RenderAntialiased, true );

    curve->setSamples(sampleNo,yDataFin);
    curve->attach( plot );
    //zoom->setZoomBase( false );
    plot->canvas()->setGeometry(0,0,ecgSignal.size()*dt,0);

    QLOG_INFO() << "GUI/  ecgFrames.Count..."<<QString::number(ecgFrames.Count);

    QVector<unsigned int> P_onsetData;
    QVector<unsigned int> P_endData;
    QVector<unsigned int> Qrs_onsetData;
    QVector<unsigned int> Qrs_endData;

    QVector<double> P_onsetDataX = QVector<double>(ecgFrames.Count);
    QVector<double> P_onsetDataY = QVector<double>(ecgFrames.Count);

    QVector<double> P_endDataX = QVector<double>(ecgFrames.Count);
    QVector<double> P_endDataY = QVector<double>(ecgFrames.Count);

    QVector<double> Qrs_onsetDataX = QVector<double>(ecgFrames.Count);
    QVector<double> Qrs_onsetDataY = QVector<double>(ecgFrames.Count);

    QVector<double> Qrs_endDataX = QVector<double>(ecgFrames.Count);
    QVector<double> Qrs_endDataY = QVector<double>(ecgFrames.Count);

    for(unsigned int i = 0; i < ecgFrames.Count; i++)
    {
        P_onsetData.append(ecgFrames.PWaveStart->at(i)-ecgSignal.begin());
        P_onsetDataX[i]=P_onsetData[i]*dt*1000;
        P_onsetDataY[i]=ecgSignal[P_onsetData[i]];

        P_endData.append(ecgFrames.PWaveEnd->at(i)-ecgSignal.begin());
        P_endDataX[i]=P_endData[i]*dt*1000;
        P_endDataY[i]=ecgSignal[P_endData[i]];

        Qrs_onsetData.append(ecgFrames.QRS_onset->at(i)-ecgSignal.begin());
        Qrs_onsetDataX[i]=Qrs_onsetData[i]*dt*1000;
        Qrs_onsetDataY[i]=ecgSignal[Qrs_onsetData[i]];

        Qrs_endData.append(ecgFrames.QRS_end->at(i)-ecgSignal.begin());
        Qrs_endDataX[i]=Qrs_endData[i]*dt*1000;
        Qrs_endDataY[i]=ecgSignal[Qrs_endData[i]];
    }


    if(ui->p_onset->isChecked() || ui->wave_all->isChecked())
    {
        QwtPlotCurve *P_onsetPoints = new QwtPlotCurve();
        QwtSymbol *P_onsetMarker = new QwtSymbol( QwtSymbol::Ellipse, Qt::green, QPen( Qt::green ), QSize( 10, 10 ) );
        P_onsetPoints->setSymbol(P_onsetMarker);
        P_onsetPoints->setTitle("P_onset");
        P_onsetPoints->setPen( QColor( Qt::green ) );
        P_onsetPoints->setStyle( QwtPlotCurve::NoCurve );
        P_onsetPoints->setSamples(P_onsetDataX,P_onsetDataY);
        P_onsetPoints->attach( plot );
    }

    if(ui->p_end->isChecked()|| ui->wave_all->isChecked())
    {
        QwtPlotCurve *P_endPoints = new QwtPlotCurve();
        QwtSymbol *P_endMarker = new QwtSymbol( QwtSymbol::Ellipse, Qt::cyan, QPen( Qt::cyan ), QSize( 10, 10 ) );
        P_endPoints->setSymbol(P_endMarker);
        P_endPoints->setTitle("P_end");
        P_endPoints->setPen( QColor( Qt::cyan ) );
        P_endPoints->setStyle( QwtPlotCurve::NoCurve );
        P_endPoints->setSamples(P_endDataX,P_endDataY);
        P_endPoints->attach( plot );
    }

    if(ui->qrs_onset->isChecked()|| ui->wave_all->isChecked())
    {
        QwtPlotCurve *Qrs_onsetPoints = new QwtPlotCurve();
        QwtSymbol *Qrs_onsetMarker = new QwtSymbol( QwtSymbol::Ellipse, Qt::darkYellow, QPen( Qt::darkYellow ), QSize( 10, 10 ) );
        Qrs_onsetPoints->setSymbol(Qrs_onsetMarker);
        Qrs_onsetPoints->setTitle("QRS_onset");
        Qrs_onsetPoints->setPen( QColor( Qt::red ) );
        Qrs_onsetPoints->setStyle( QwtPlotCurve::NoCurve );
        Qrs_onsetPoints->setSamples(Qrs_onsetDataX,Qrs_onsetDataY);
        Qrs_onsetPoints->attach( plot );
    }

    if(ui->qrs_end->isChecked()|| ui->wave_all->isChecked())
    {
        QwtPlotCurve *Qrs_endPoints = new QwtPlotCurve();
        QwtSymbol *Qrs_endMarker = new QwtSymbol( QwtSymbol::Ellipse, Qt::magenta, QPen( Qt::magenta ), QSize( 10, 10) );
        Qrs_endPoints->setSymbol(Qrs_endMarker);
        Qrs_endPoints->setTitle("QRS_end");
        Qrs_endPoints->setPen( QColor( Qt::magenta ) );
        Qrs_endPoints->setStyle( QwtPlotCurve::NoCurve );
        Qrs_endPoints->setSamples(Qrs_endDataX,Qrs_endDataY);
        Qrs_endPoints->attach( plot );
    }

    QwtLegend* legend = new QwtLegend();
    legend->setItemMode(QwtLegend::ReadOnlyItem);
    plot->insertLegend(legend, QwtPlot::BottomLegend);
    //

    plot->setAxisScaleDraw( QwtPlot::xBottom, new TimeScaleDraw( QTime() ) );
    plot->axisAutoScale(QwtPlot::xBottom);
    QwtText xaxis("Time [mm:ss:zzz]");
    QwtText yaxis("Voltage [mV]");
    xaxis.setFont(QFont("Arial", 8));
    yaxis.setFont(QFont("Arial", 8));

    plot->setAxisTitle( QwtPlot::yLeft, yaxis );
    plot->setAxisTitle( QwtPlot::xBottom, xaxis );
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
        sampleNo[i]=(i)*dt*1000;// *1000 ms
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
    QVector<unsigned int> st_midData;

    for(unsigned int i = 0; i < stbegin.size(); i++)
    {
        st_beginData.append(stbegin.at(i));
        st_endData.append(stend.at(i));
        st_midData.append((stbegin.at(i)+stend.at(i)/2));
    }

    QVector<double> st_beginDataX = QVector<double>(stbegin.size());
    QVector<double> st_beginDataY = QVector<double>(stend.size());

    QVector<double> st_endDataX = QVector<double>(stbegin.size());
    QVector<double> st_endDataY = QVector<double>(stend.size());

    QVector<double> st_midDataX = QVector<double>(stbegin.size());
    QVector<double> st_midDataY = QVector<double>(stend.size());

    for (int i=0; i < stbegin.size();++i)
    {
        st_beginDataX[i]=st_beginData[i]*dt;
        st_beginDataY[i]=yData[st_beginData[i]];

        st_endDataX[i]=st_endData[i]*dt;
        st_endDataY[i]=yData[st_endData[i]];

        st_midDataX[i]=st_midData[i]*dt;
        st_midDataY[i]=yData[st_endData[i]];

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

    QwtPlotCurve *st_midPoints = new QwtPlotCurve();
    QwtSymbol *st_midMarker = new QwtSymbol( QwtSymbol::Ellipse, Qt::black, QPen( Qt::blue ), QSize( 5, 5 ) );
    st_midPoints->setSymbol(st_midMarker);
    st_midPoints->setTitle("ST_mid");
    st_midPoints->setPen( QColor( Qt::black ) );
    st_midPoints->setStyle( QwtPlotCurve::NoCurve );
    st_midPoints->setSamples(st_midDataX,st_midDataY);
    st_midPoints->attach( plot );

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

    plot->setAxisScaleDraw( QwtPlot::xBottom, new TimeScaleDraw( QTime() ) );
    plot->axisAutoScale(QwtPlot::xBottom);
    QwtText xaxis("Time [mm:ss:zzz]");
    QwtText yaxis("Voltage [mV]");
    xaxis.setFont(QFont("Arial", 8));
    yaxis.setFont(QFont("Arial", 8));

    plot->setAxisTitle( QwtPlot::yLeft, yaxis );
    plot->setAxisTitle( QwtPlot::xBottom, xaxis );

    return plot;
}

void AirEcgMain::drawEcgBaseline(EcgData *data)
{
    //dla pierwszego taba
    QwtPlot *plotMLII = plotPlot(*(data->ecg_baselined),data->info->frequencyValue);
    ui->baselinedArea->setWidget(plotMLII);
    ui->baselinedArea->show();
}

void AirEcgMain::drawAtrialFibr(EcgData *data)
{
    QLOG_INFO() << "Start \"rysowania\" AtrialFibr";

    //wykres
    // QwtPlot *plotAtrialFibr;
    //ui->AtrialFibrArea->setWidget(plotAtrialFibr);
    //ui->AtrialFibrArea->show();

    //parametry
    ui->Param1->setText(QString::number((data->PWaveOccurenceRatio), 'f', 2) + " ");
    ui->Param2->setText(QString::number((data->RRIntEntropy), 'f', 2) + " ");
    ui->Param3->setText(QString::number((data->RRIntDivergence), 'f', 2) + "");

    //migotanie
    if(data->AtrialFibr)
    {
        ui->migotanie_frame->setStyleSheet("background-color: rgb(255, 0, 0);");

    }
    else
    {
        ui->migotanie_frame->setStyleSheet("background-color: rgb(0, 170, 0);");
    }

}

void AirEcgMain::drawRPeaks(EcgData *data)
{

    QLOG_TRACE() << "drawRPeaks";
    QwtPlot *plotVI = plotPointsPlot(*(data->Rpeaks),*(data->ecg_baselined),data->info->frequencyValue);
    //QwtPlot *plotVI = plotPointsPlot_uint((data->Rpeaks_uint),*(data->ecg_baselined),data->info->frequencyValue);
    ui->rpeaksArea->setWidget(plotVI);
    ui->rpeaksArea->show();
    emit busy(false);
}

void AirEcgMain::drawHrv1(EcgData *data)
{
    QLOG_DEBUG() << "GUI/HRV1 0";
    QLOG_INFO() << "GUI/ drawing hrv1..."<<QString::number(data->Mean);
    ui->Mean->setText("Mean = " % QString::number((data->Mean), 'f', 2) + " ms");
    ui->SDNN->setText("SDNN = " %QString::number((data->SDNN), 'f', 2) + " ms");
    ui->RMSSD->setText("RMSSD = " %QString::number((data->RMSSD), 'f', 2) + " ms");
    ui->RR50->setText("RR50 = " %QString::number((data->RR50), 'd', 2));
    ui->RR50Ratio->setText("RR50 Ratio = " %QString::number((data-> RR50Ratio), 'c', 2) + " %");
    ui->SDANN->setText("SDANN = " %QString::number((data->SDANN), 'f', 2) + " ms");
    ui->SDANNindex->setText("SDANN Index = " %QString::number((data->SDANNindex), 'f', 2) + " ms");
    ui->SDSD->setText("SDSD = " %QString::number((data->SDSD), 'f', 2) + " ms");

    QLOG_DEBUG() << "GUI/HRV1 1";

    //RR
    QwtPlot *plotRR = plotPlotRR(*(data->RR_y),*(data->RR_x));
    ui->scrollAreaRR->setWidget(plotRR);
    ui->scrollAreaRR->show();

    //Fourier    
    QwtPlot *plotFT = plotPlot(*(data->fft_y),*(data->fft_x));
    ui->scrollAreaFT->setWidget(plotFT);
    ui->scrollAreaFT->show();
    QLOG_DEBUG() << "GUI/HRV1 2";
    //Frequency Coefficients
    ui->TP->setText("TP=" %QString::number(((long)data->TP), 'f', 2) + " ms^2");
    ui->HF->setText("HF=" %QString::number(((long)data->HF), 'f', 2) + " ms^2");
    ui->LF->setText("LF=" %QString::number(((long)data->LF), 'f', 2) + " ms^2");
    ui->VLF->setText("VLF=" %QString::number(((long)data->VLF), 'd', 2) + " ms^2");
    ui->ULF->setText("ULF=" %QString::number(((long)data->ULF), 'c', 2) + " ms^2");
    ui->LFHF->setText("LFHF=" %QString::number(100*(data->LFHF), 'f', 2) + " %");
    QLOG_DEBUG() << "GUI/HRV1 3";
}

void AirEcgMain::drawSigEdr(EcgData *data)
{
    QLOG_INFO() << "Drawing SigEdr.";
    //if (data->SigEdr_r==NULL)
    //    QLOG_FATAL() << "SigEdr does not exist";
    //else
    {
        //Rysowanie Baselina dla sig edr
        QwtPlot *plotBaseEDR = plotPlot(*(data->ecg_baselined),data->info->frequencyValue);
        ui->Baseline_edr->setWidget(plotBaseEDR);
        ui->Baseline_edr->show();

        //dla sig edr
        unsigned int no=0;
        if(ui->checkBox_2->isChecked() && ui->checkBox_4->isChecked()) //oba sygnaly
        {
            no = 2;
        }
        else
        {
            if (ui->checkBox_2->isChecked()) //pierwszy
                no = 1;
            else //drugi
                no = 0;
        }
                                             //data waves            //data baseline
        QwtPlot *plotEDR = plotPlot_SIG_EDR(*(data->Rpeaks),*(data->ecg_baselined),*(data->SigEdr_r),*(data->SigEdr_q),data->info->frequencyValue, no);
        ui->scrollArea_2->setWidget(plotEDR);
        ui->scrollArea_2->show();
    }

   // QLOG_TRACE() << "GUI/ SigEdr has "<<QString::number(data->SigEdr_r->size())<< " points.";

}

void AirEcgMain::drawStInterval(EcgData *data)
{
    // draw baselined data
    stIntervalPlot = plotPlot(*(data->ecg_baselined), data->info->frequencyValue);
    stIntervalZoomer = zoom;
    ui->stIntervalArea->setWidget(stIntervalPlot);
    ui->stIntervalArea->show();

    if (data->STintervals == NULL)
        return;

    int num = data->STintervals->size();

    ui->stIntervalList->setRowCount(num);
    ui->stIntervalList->setEditTriggers(QAbstractItemView::NoEditTriggers); // disable list edit

    QVector<QPointF> stOn;
    QVector<QPointF> stMid;
    QVector<QPointF> stEnd;

    int abnormalNum = 0;

    for (int i = 0; i < num; i++)
    {
        EcgStDescriptor desc = data->STintervals->at(i);

        int on = desc.STOn - data->ecg_baselined->constBegin();
        int mid = desc.STMid - data->ecg_baselined->constBegin();
        int end = desc.STEnd - data->ecg_baselined->constBegin();

        double onTime = static_cast<double>(on) / data->info->frequencyValue * 1000;

        stOn.append(QPointF(onTime, *desc.STOn));
        stMid.append(QPointF(static_cast<double>(mid) / data->info->frequencyValue * 1000, *desc.STMid));
        stEnd.append(QPointF(static_cast<double>(end) / data->info->frequencyValue * 1000, *desc.STEnd));

        QVector<double> x;
        QVector<double> y;
        for (QVector<double>::const_iterator k = desc.STOn; k != desc.STEnd; ++k)
        {
            int sampleNum = k - data->ecg_baselined->constBegin();
            x.append(static_cast<double>(sampleNum) / data->info->frequencyValue * 1000);
            y.append(*k);
        }

        // create curves for ST intervals highlights
        QwtPlotCurve *st = new QwtPlotCurve();
        QPen pen(Qt::red);
        pen.setWidth(2);
        st->setPen(pen);
        st->setSamples(x, y);
        st->setItemAttribute(QwtPlotItem::Legend, false);
        st->attach(stIntervalPlot);

        QColor bgColor(255, 255, 255);

        bool abnormal = false;
        QString position;
        switch (desc.position)
        {
        case ST_POS_NORMAL:
            position = tr("normal");
            break;
        case ST_POS_ELEVATION:
            position = tr("elevation");
            abnormal = true;
            break;
        case ST_POS_DEPRESSION:
            position = tr("depression");
            bgColor = QColor(255, 0, 0, 60);
            abnormal = true;
            break;
        }

        if (abnormal)
        {
            bgColor = QColor(255, 0, 0, 60);
            abnormalNum++;
        }

        QString shape;
        switch (desc.shape)
        {
        case ST_SHAPE_HORIZONTAL:
            shape = tr("horizontal");
            break;
        case ST_SHAPE_DOWNSLOPING:
            shape = tr("downsloping");
            break;
        case ST_SHAPE_UPSLOPING:
            shape = tr("upsloping");
            break;
        case ST_SHAPE_CONCAVE:
            shape = tr("concave");
            break;
        case ST_SHAPE_CONVEX:
            shape = tr("convex");
            break;
        }

        // create STon time string
        int ms = static_cast<int>(onTime) % 1000;
        int ss = static_cast<int>(onTime) / 1000 % 60;
        int mm = static_cast<int>(onTime) / 60000;
        QString time = QString("%1:%2:%3").arg(mm, 2, 10, QChar('0')).arg(ss, 2, 10, QChar('0')).arg(ms, 3, 10, QChar('0'));

        // create list item
        QTableWidgetItem *newItem = new QTableWidgetItem(time);
        newItem->setBackgroundColor(bgColor);
        newItem->setData(Qt::UserRole, onTime);
        newItem->setData(Qt::UserRole + 1, abnormal);
        ui->stIntervalList->setItem(i, 0, newItem);

        newItem = new QTableWidgetItem(position);
        newItem->setBackgroundColor(bgColor);
        newItem->setData(Qt::UserRole, onTime);
        ui->stIntervalList->setItem(i, 1, newItem);

        newItem = new QTableWidgetItem(shape);
        newItem->setBackgroundColor(bgColor);
        newItem->setData(Qt::UserRole, onTime);
        ui->stIntervalList->setItem(i, 2, newItem);

        newItem = new QTableWidgetItem(QString::number(desc.offset, 'f', 3));
        newItem->setBackgroundColor(bgColor);
        newItem->setData(Qt::UserRole, onTime);
        ui->stIntervalList->setItem(i, 3, newItem);

        newItem = new QTableWidgetItem(QString::number(desc.slope1, 'f', 2));
        newItem->setBackgroundColor(bgColor);
        newItem->setData(Qt::UserRole, onTime);
        ui->stIntervalList->setItem(i, 4, newItem);

        newItem = new QTableWidgetItem(QString::number(desc.slope2, 'f', 2));
        newItem->setBackgroundColor(bgColor);
        newItem->setData(Qt::UserRole, onTime);
        ui->stIntervalList->setItem(i, 5, newItem);
    }

    // draw STon points
    QwtPlotCurve *stOnPoints = new QwtPlotCurve();
    QwtSymbol *stOnMarker = new QwtSymbol(QwtSymbol::Ellipse, Qt::red, QPen(Qt::red), QSize(6, 6));
    stOnPoints->setSymbol(stOnMarker);
    stOnPoints->setTitle("STon");
    stOnPoints->setPen(QColor(Qt::red));
    stOnPoints->setStyle(QwtPlotCurve::NoCurve);
    stOnPoints->setSamples(stOn);
    stOnPoints->attach(stIntervalPlot);

    // draw STmid points
    QwtPlotCurve *stMidPoints = new QwtPlotCurve();
    QColor color(11, 157, 0);
    QwtSymbol *stMidMarker = new QwtSymbol(QwtSymbol::Ellipse, QBrush(color), QPen(color), QSize(6, 6));
    stMidPoints->setSymbol(stMidMarker);
    stMidPoints->setTitle("STmid");
    stMidPoints->setPen(color);
    stMidPoints->setStyle(QwtPlotCurve::NoCurve);
    stMidPoints->setSamples(stMid);
    stMidPoints->attach(stIntervalPlot);

    // draw STend points
    QwtPlotCurve *stEndPoints = new QwtPlotCurve();
    QwtSymbol *stEndMarker = new QwtSymbol(QwtSymbol::Ellipse, Qt::blue, QPen(Qt::blue), QSize(6, 6));
    stEndPoints->setSymbol(stEndMarker);
    stEndPoints->setTitle("STend");
    stEndPoints->setPen(QColor(Qt::blue));
    stEndPoints->setStyle(QwtPlotCurve::NoCurve);
    stEndPoints->setSamples(stEnd);
    stEndPoints->attach(stIntervalPlot);

    double percentage = static_cast<double>(abnormalNum) / num * 100.0;

    ui->stAbnormalNum->setText(QString::number(abnormalNum));
    ui->stPercentage->setText(QString("%1%").arg(percentage, 0, 'f', 2));
}

void AirEcgMain::initStIntervalGui()
{
    const int INITIAL_SIZE = 250;
    QList<int> sizes;
    sizes.append(INITIAL_SIZE);
    sizes.append(ui->stSplitter->width() - INITIAL_SIZE);
    ui->stSplitter->setSizes(sizes);

    connect(ui->stIntervalList, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(stItemSelected(int,int)));
    connect(ui->stPrev, SIGNAL(clicked()), this, SLOT(prevStAbnormality()));
    connect(ui->stNext, SIGNAL(clicked()), this, SLOT(nextStAbnormality()));
}

void AirEcgMain::stItemSelected(int row, int column)
{
    QTableWidgetItem *item = ui->stIntervalList->item(row, column);
    if (item == NULL)
        return;

    bool ok;
    double onTime = item->data(Qt::UserRole).toDouble(&ok);
    if (!ok)
        return;

    if (stIntervalZoomer != NULL)
        stIntervalZoomer->moveToHPosition(onTime, true);
}

void AirEcgMain::nextStAbnormality()
{
    int curr = ui->stIntervalList->currentRow();
    for (int i = curr + 1; i < ui->stIntervalList->rowCount(); i++)
    {
        QTableWidgetItem *item = ui->stIntervalList->item(i, 0);
        if (item == NULL)
            continue;

        bool abnormal = item->data(Qt::UserRole + 1).toBool();
        if (abnormal)
        {
            ui->stIntervalList->selectRow(i);
            stItemSelected(i, 0);
            return;
        }
    }
}

void AirEcgMain::prevStAbnormality()
{
    int curr = ui->stIntervalList->currentRow();
    for (int i = curr - 1; i >= 0; i--)
    {
        QTableWidgetItem *item = ui->stIntervalList->item(i, 0);
        if (item == NULL)
            continue;

        bool abnormal = item->data(Qt::UserRole + 1).toBool();
        if (abnormal)
        {
            ui->stIntervalList->selectRow(i);
            stItemSelected(i, 0);
            return;
        }
    }
}

void AirEcgMain::drawSleep_Apnea(EcgData* data)
{
    QwtPlot *plotSleepApnea = plotSleep_Apnea(*(data->SleepApneaamp),*(data->SleepApneatime), data->SleepApnea_plot->at(0),*(data->SleepApnea));
    ui->sleepArea1->setWidget(plotSleepApnea);
    ui->sleepArea1->show();

    QwtPlot *plotSleepApneafrequence = plotSleep_ApneaFreq(*(data->SleepApneafreq),*(data->SleepApneatime), data->SleepApnea_plot->at(1),*(data->SleepApnea));
    ui->sleepArea2->setWidget(plotSleepApneafrequence);
    ui->sleepArea2->show();

    ui->sleepcnt->setText(QString::number(data->SleepApnea_plot->value(2))+" %");
    ui->sleepcntfrequence->setText(QString::number(data->SleepApnea_plot->value(3))+" %");
}
void AirEcgMain::drawVcgLoop(EcgData* data)
{

    ui->pushButton_prev_vcg->setEnabled(true);
    ui->pushButton_next_vcg->setEnabled(true);

    ui->vcg_dea->setText("-");//QString::number(*(data->SD2)));
    ui->vcg_ma->setText("-");//QString::number(*(data->SD2)));
    ui->vcg_rmmv->setText("-");//QString::number(*(data->SD2)));

    QwtPlot *plotVcgLoop1 = plotPlot(*(data->ecg_baselined),*(data->ecg_baselined) );
    ui->scrollArea_VcgLoop1->setWidget(plotVcgLoop1);
    ui->scrollArea_VcgLoop1->show();

    QwtPlot *plotVcgLoop2 = plotPlot(*(data->ecg_baselined),*(data->ecg_baselined) );
    ui->scrollArea_VcgLoop2->setWidget(plotVcgLoop2);
    ui->scrollArea_VcgLoop2->show();

    QwtPlot *plotVcgLoop3 = plotPlot(*(data->ecg_baselined),*(data->ecg_baselined) );
    ui->scrollArea_VcgLoop3->setWidget(plotVcgLoop3);
    ui->scrollArea_VcgLoop3->show();

    QwtPlot *plotVcgLoop4 = plotPlot(*(data->ecg_baselined),*(data->ecg_baselined) );
    ui->scrollArea_VcgLoop4->setWidget(plotVcgLoop4);
    ui->scrollArea_VcgLoop4->show();

}

void AirEcgMain::drawQrsClass(EcgData *data)
{
    this->resetQrsToolbox(data);
}

void AirEcgMain::drawHrt(EcgData *data)
{
    QwtPlot *hrtTachogram = plotHrt(*(data->hrt_tachogram),data->hrt_a,data->hrt_b);
    ui->scrollAreaHrt->setWidget(hrtTachogram);
    ui->scrollAreaHrt->show();
    ui->vpbs_detected_count->setText(QString::number((data->vpbs_detected_count), 'f', 0));
    ui->turbulence_onset_val->setText(QString::number((data->turbulence_onset), 'f', 2));
    ui->turbulence_slope_val->setText(QString::number((data->turbulence_slope), 'f', 2));
}

void AirEcgMain::drawQtDisp(EcgData *data)
{
    QwtPlot *plotQtDisp = plotPointsPlot(*(data->Waves->T_end),*(data->ecg_baselined),data->info->frequencyValue);
    ui->scrollArea_9->setWidget(plotQtDisp);
    ui->scrollArea_9->show();

    ui->DIS_Bazzet->setText(QString::number((data->evaluations->at(0).percentOfCorrectQT ), 'f', 0));
    ui->TL_Bazzet->setText(QString::number((data->evaluations->at(0).percentOfTooLowQT ), 'f', 0));
    ui->TH_Bazzet->setText(QString::number((data->evaluations->at(0).percentOfTooHighQT ), 'f', 0));
  // ui->name0->setText(QString(data->evaluations->at(0).nameOfEvaluation));

    ui->DIS_Frideric->setText(QString::number((data->evaluations->at(1).percentOfCorrectQT ), 'f', 0));
    ui->TL_Frideric->setText(QString::number((data->evaluations->at(1).percentOfTooLowQT ), 'f', 0));
    ui->TH_Frideric->setText(QString::number((data->evaluations->at(1).percentOfTooHighQT ), 'f', 0));
  //  ui->name1->setText(QString(data->evaluations->at(1).nameOfEvaluation));

    ui->DIS_Framingham->setText(QString::number((data->evaluations->at(2).percentOfCorrectQT ), 'f', 0));
    ui->TL_Framingham->setText(QString::number((data->evaluations->at(2).percentOfTooLowQT ), 'f', 0));
    ui->TH_Framingham->setText(QString::number((data->evaluations->at(2).percentOfTooHighQT ), 'f', 0));
   // ui->name2->setText(QString(data->evaluations->at(2).nameOfEvaluation));

    ui->DIS_Hodges->setText(QString::number((data->evaluations->at(3).percentOfCorrectQT ), 'f', 0));
    ui->TL_Hodges->setText(QString::number((data->evaluations->at(3).percentOfTooLowQT ), 'f', 0));
    ui->TH_Hodges->setText(QString::number((data->evaluations->at(3).percentOfTooHighQT ), 'f', 0));
   // ui->name3->setText(QString(data->evaluations->at(3).nameOfEvaluation));

    ui->avQTdis->setText((QString::number((data->evaluations->at(3).averageQT ), 'f', 0)));
            ui->deviationQtdis->setText(QString::number((data->evaluations->at(3).standardDeviationQT ), 'f', 0));

    QLOG_ERROR() << "GUI/ QtDist needs to be drawn.";
}
void AirEcgMain::drawWaves(EcgData *data)
{
    QLOG_FATAL() << "GUI/ drawWaves not done yet.";

    QwtPlot *wavesPlot = plotWavesPlot(*(data->ecg_baselined), *(data->Waves), data->info->frequencyValue );

    ui->scrollAreaWaves->setWidget(wavesPlot);
    ui->scrollAreaWaves->show();

}
void AirEcgMain::busy(bool state)
{
    ui->progressBar->setVisible(state);
    ui->busy_label->setVisible(state);
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



void AirEcgMain::on_pushButton_2_clicked()
{
    busy(true);
    emit this->runEcgBaseline();
}

void AirEcgMain::on_pushButton_3_clicked()
{
    emit this->runAtrialFibr();//linia 36
}

void AirEcgMain::on_pushButton_5_clicked()
{
    emit this->runWaves();
}

void AirEcgMain::on_pushButton_6_clicked()
{
    emit this->runHRV1();
}

void AirEcgMain::on_pushButton_10_clicked()
{
    emit this->runQrsClass();
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
        /*emit qrsClustererChanged(KMeansClusterer);
        emit qrsMaxIterationsChanged(ui->qrsSetKMaxIterSpinBox->value());
        emit qrsParallelExecutionChanged(ui->qrsSetKMeansParallelCheckBox->isEnabled());
        emit qrsKClustersNumberChanged(ui->qrsSetKClusterNumSpinBox->value());
        */
    }
    else
    {
        /*emit qrsClustererChanged(GMeansClusterer);
        emit qrsMaxIterationsChanged(ui->qrsSetGMaxItersSpinBox->value());
        emit qrsGMaxKIterations(ui->qrsSetGinKMaxIterations->value());
        emit qrsGMinClustersChanged(ui->qrsSetGMinClusterSpinBox->value());
        emit qrsGMaxClustersChanged(ui->qrsSetGMaxClusterSpinBox->value());
        emit qrsParallelExecutionChanged(ui->qrsSettingsGMeansParallelCheckBox->isEnabled());
        */
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
void AirEcgMain::on_movingAverageRadioButton_clicked()
{
    ui->MovingAvarangeGroupBox->setEnabled(true);
    ui->ButterworthcomboBox->setEnabled(false);
}

void AirEcgMain::on_savitzkyGolayRadioButton_clicked()
{
    ui->ButterworthcomboBox->setEnabled(false);
    ui->MovingAvarangeGroupBox->setEnabled(false);
}

void AirEcgMain::on_kalmanRadioButton_clicked()
{
    ui->ButterworthcomboBox->setEnabled(false);
}

void AirEcgMain::on_maTimeSpinBox_valueChanged(const QString &arg1)
{
    double time = arg1.toDouble();
    int windowSize = ceil(time * currentEcgData->info->frequencyValue);
    ui->windowSizeLabel->setText(QString::number(windowSize));

    emit ecgBase_WindowSizeChanged(QString::number(windowSize));
}

void AirEcgMain::on_ButterworthcomboBox_currentIndexChanged(int index)
{
    emit ecgBase_ButterworthCoeffSetChanged(index);
}

void AirEcgMain::on_checkBox_2_clicked(bool checked)
{
    ui->groupBox_11->setEnabled(checked);
}
 void AirEcgMain::on_pushButton_clicked()
 {
     emit this->runStInterval();
 }

void AirEcgMain::on_butterworthRadioButton_clicked()
{
    ui->ButterworthGroupBox->setEnabled(true);
    ui->ButterworthcomboBox->setEnabled(true);
    ui->MovingAvarangeGroupBox->setEnabled(false);
}

void AirEcgMain::on_pushButton_17_clicked()
{
    //TODO: Sprawdzanie czy wczesniej juz bylo policzone
    emit this->runEcgBaseline();
    emit this->runRPeaks();
}

void AirEcgMain::on_pushButton_4_clicked()
{
    emit this->runSigEdr();
}

void AirEcgMain::on_radioButton_3_clicked()
{
    emit this->switchSignal_SIGEDR(1);
}

void AirEcgMain::on_radioButton_4_clicked()
{
    emit this->switchSignal_SIGEDR(2);
}

void AirEcgMain::on_pushButton_next_vcg_clicked()
{
    emit this->vcg_loop_change(1);
}

void AirEcgMain::on_pushButton_prev_vcg_clicked()
{
    emit this->vcg_loop_change(0);
}

void AirEcgMain::on_RUN_VCG_pushButton_clicked()
{
    emit this->runVcgLoop();
}

void AirEcgMain::on_st_interval_detection_width_valueChanged(int arg1)
{
    emit stInterval_detectionWidthChanged(arg1);
}

void AirEcgMain::on_st_interval_smoothing_width_valueChanged(int arg1)
{
    emit stInterval_smoothingWidthChanged(arg1);
}

void AirEcgMain::on_st_interval_morphology_valueChanged(double arg1)
{
    emit stInterval_morphologyChanged(arg1);
}

void AirEcgMain::on_st_interval_level_threshold_valueChanged(double arg1)
{
    emit stInterval_levelThresholdChanged(arg1);
}

void AirEcgMain::on_st_interval_slope_threshold_valueChanged(double arg1)
{
    emit stInterval_slopeThresholdChanged(arg1);
}

void AirEcgMain::on_detectionratesquare_clicked()
{
    emit stInterval_algorithmChanged(1);
}

void AirEcgMain::on_detectionratelinear_clicked()
{
    emit stInterval_algorithmChanged(0);
}

void AirEcgMain::on_pushButton_11_clicked()
{
    emit runSleepApnea();
}

void AirEcgMain::initEcgBaselineGui()
{
    QStringList coeffList;
    foreach(const ButterCoefficients &coeff, predefinedButterCoefficientSets()) {
        coeffList.append(coeff.name());
    }

    ui->ButterworthcomboBox->addItems(coeffList);

    ecgBase_WindowSizeChanged("150");
    on_butterworthRadioButton_clicked();
}

void AirEcgMain::on_pushButton_16_clicked()
{
    emit runQtDisp();
}

void AirEcgMain::on_pushButton_18_clicked()
{
    emit runHRT();
}

/*
QwtPlot* AirEcgMain::plotPointsPlot_uint(QVector<unsigned int> p, const QVector<double> &yData, float freq){
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
    plot->setAxisScale( QwtPlot::yLeft, min-1, 1+max );
    plot->setAxisScale( QwtPlot::xBottom , 0, 4000.0);

    plot->setAxisScaleDraw( QwtPlot::xBottom, new TimeScaleDraw( QTime() ) );
    plot->axisAutoScale(QwtPlot::xBottom);
    QwtText xaxis("Time [mm:ss:zzz]");
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
    QVector<unsigned int> pData = QVector<unsigned int>(p.size());
    for (int i=0;i<p.size();i++)
    {
        pData[i]=((unsigned int)p.at(i));
        //QLOG_TRACE() << QString::number( p.at(i));
    }

    QVector<double> pDataX = QVector<double>(pData.size());
    QVector<double> pDataY = QVector<double>(pData.size());

    for (int i=0;i<pData.size();++i)
    {
        pDataX[i]=pData[i]*tos;
        pDataY[i]=yData[pData[i]];
    }

    QwtPlotCurve *points = new QwtPlotCurve();
    QwtSymbol *marker = new QwtSymbol( QwtSymbol::Ellipse, Qt::red, QPen( Qt::red ), QSize( 5, 5 ) );
    points->setSymbol(marker);
    points->setPen( QColor( Qt::red ) );
    points->setStyle( QwtPlotCurve::NoCurve );
    points->setSamples(pDataX,pDataY);
    points->attach( plot );

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
    plot->setAxisScale( QwtPlot::xBottom , 0, 4000.0);

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
    plot->setAxisScaleDraw( QwtPlot::xBottom, new TimeScaleDraw( QTime() ) );
    plot->axisAutoScale(QwtPlot::xBottom);
    QwtText xaxis("Time [mm:ss:zzz]");
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
    plot->setAxisScale( QwtPlot::xBottom , 0, 4000.0);

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
*/
/*
QwtPlot* AirEcgMain::plotPlot(QList<int> &y,float freq){

    QLOG_ERROR()<< "Executing wrong plot";

    QVector<int> yData = QVector<int>::fromList(y);
    QVector<double> yDataFin = QVector<double>(yData.size());
    QVector<double> sampleNo = QVector<double>(yData.size());


    float max=yData.first();
    float min=yData.first();

    double tos=1/freq;

    for (int i=0;i<yData.size();++i)
    {
        sampleNo[i]=(i)*tos;
        yDataFin[i]=yData[i]/200.0-5.0;
        if (max<yData[i]) max=yData[i];
        if (min>yData[i]&&min>0) min=yData[i];
    }
    max=max/200-5.0;
    min=min/200-5.0;

    QwtPlot *plot = new QwtPlot();

    plot->setCanvasBackground( Qt::white );
    plot->setAxisScale( QwtPlot::yLeft, min, max,0.5 );

    plot->setAxisScale( QwtPlot::xBottom , 0.0,4000.0,1);// sampleNo.last());

    QList<double> ticks;
    for(int i=0; i<100;i++)
    {
        ticks.append(i);
    }

    // wstawianie lini pionowych
    QwtPlotMarker *mX = new QwtPlotMarker();
    mX->setLabel( QString::fromLatin1( "label" ) );
    mX->setLabelAlignment( Qt::AlignLeft | Qt::AlignBottom );
    mX->setLabelOrientation( Qt::Vertical );
    mX->setLineStyle( QwtPlotMarker::VLine );
    mX->setLinePen( QPen( Qt::black, 0, Qt::DashDotLine ) );
    mX->setXValue( 2.0);
    mX->attach( plot );


    // 3d example
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

    plot->canvas()->setFrameStyle( QFrame::Box | QFrame::Plain );
    plot->canvas()->setLineWidth( 1 );
    plot->canvas()->setGeometry(0,0,sampleNo.last(),0);

    zoom = new ScrollZoomer(plot->canvas());
    zoom->setRubberBandPen(QPen(Qt::white));
    zoom->setZoomBase(plot->canvas()->rect());
    zoom->setZoomStack(zoom->zoomStack(), 100);

    return plot;
}
*/
/*
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
*/
/*
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

*/
/*
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
*/
/*
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


*/
/*
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
*/
