#ifndef AIRECGMAIN_H
#define AIRECGMAIN_H

#include <QMainWindow>
#include "Common/ecginfo.h"
#include "Common/ecgdata.h"
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <QListWidgetItem>
#include "scrollzoomer.h"
#include <QsLog/QsLog.h>

class QSignalMapper;

namespace Ui {
class AirEcgMain;
}

class AirEcgMain : public QMainWindow
{
    Q_OBJECT

    QwtPlot *qrsClassPlot;
    QwtPlotCurve *currentQrsClassPlot;
    int qrsClassCurveMaxIndex;
    double tScale;

    ScrollZoomer *zoom;

    QSignalMapper* baselineSignalMapper;
    QString hash;

    EcgData *currentEcgData;

    QwtPlot *stIntervalPlot;
    ScrollZoomer *stIntervalZoomer;

    QwtPlot *BaseLinePlot;
    ScrollZoomer *BaseLineZoomer;


    void resetQrsToolbox(EcgData* data);
    void populareQRSClassBox(QRSClass currentClass, int type);
public:

    explicit AirEcgMain(QWidget *parent = 0);
    ~AirEcgMain();
    QwtPlot* plotPlot(QList<int> &y, float freq);
    QwtPlot* plotPlot(const QVector<double> &xData, const QVector<double> &yData);
    QwtPlot* plotPlotRR(const QVector<double>& yData,const QVector<double>& xData);
    QwtPlot* plotPlot(const QVector<double> &yData, float freq);
    QwtPlot* plotPlot_SIG_EDR(const QVector<QVector<double>::const_iterator> &p,const QVector<double>& yData,const QVector<double>& yData1,const QVector<double>& yData2, float freq, unsigned int no);
    QwtPlot* plotPlotBSiG(const QVector<double>& yData, float freq);
    QwtPlot* plotHrt(QVector<double>& yData, double a, double b);
    QwtPlot* plotLogPlot(QList<double> &x, QList<double> &y, int rodzaj);
    QwtPlot* plotBarChart(QList<unsigned int> &x, QList<int> &y);
    QwtPlot* plotPointsPlot(const QVector<QVector<double>::const_iterator> &p,const QVector<double> &y, float freq);
    QwtPlot* plotPointsPlot_uint(QVector<unsigned int> p, const QVector<double> &yData, float freq);

    QwtPlot* plotPlotVCG(const QVector<double>& yData,const QVector<double>& xData);

    //HRV1*****************
    QwtPlot* plotPointsPlotDoubleToDouble(QList<double> &x, QList<double> &y);
    QwtPlot* plotLogPlotF(QList<double> &x,QList<double> &y,int rodzaj);
    QwtPlot* plotPoints(QList<double> &x, QList<double> &y,
                        QList<double> fftSamplesX, QList<double> fftSamplesY,
                        QList<double> interpolateX,  QList<double> interpolateY);
    //*********************
    QwtPlot* plotPoincarePlot(QList<unsigned int> &x, QList<int> &y, double &sd1, double &sd2);
    QwtPlot* plotTWAPlot(const QVector<double> &VI_values, QList<unsigned int> &TWA_positive, QList<unsigned int> &TWA_negative, float freq);
    QwtPlot* plotTWAPlot2(QList<unsigned int> &TWA_positive, QList<double> &TWA_positive_value, QList<unsigned int> &TWA_negative, QList<double> &TWA_negative_value);
    QwtPlot* plotPlotdfa(QList<double> &y1, QList<double> &y2);
    QwtPlot* plotPointsPlotDFA(QList<double> &x, QList<double> &y, double &wsp_a, double &wsp_b);


    QwtPlot* plotWavesPlot(const QVector<double> &ecgSignal, Waves_struct &ecgFrames, float samplingFrequency);
    QwtPlot* plotIntervalPlot(QList<double> &ecgbaselined, QList<int> &stbegin, QList<int> &stend, double samplingFrequency);
//////////
    QwtPlot* plotSleep_Apnea(const QVector<double>& yData,const QVector<double>& xData, double threshold, QVector<BeginEndPair> sleep_apnea_pairs,float freq);
    QwtPlot* plotSleep_ApneaFreq(const QVector<double>& yData,const QVector<double>& xData, double threshold, QVector<BeginEndPair> sleep_apnea_pairs,float freq);



signals:
    void loadEntity(const QString &directory, const QString &name);
    void switchSignal(int index);
    void switchSignal_SIGEDR(int index);
    void vcg_loop_change(int index);
    void switchEcgBaseline(int type);
    void switchRPeaks(unsigned char type);
    void switchTWA(unsigned char type);
    void switchWaves_p_onset(bool check);
    void run();
    void test(int index, int type);
    void runSingle(QString hash);


    //modules invoke
    void runEcgBaseline();//example
    void runAtrialFibr();
    void runStInterval();
    void runRPeaks();
    void runHRV1();
    void runWaves();
    void runSigEdr();
    void runQrsClass();
    void runSleepApnea();
    void runVcgLoop();
    void runQtDisp();
    void runHRT();

    void qrsClassChanged(int index, int type);
    void qrsMaxIterationsChanged(int maxIters);
    void qrsGMaxKIterations(int maxIters);
    void qrsClustererChanged(ClustererType type);

    void ecgBase_CzasUsrednieniaChanged(const QString &arg1);
    void ecgBase_WindowSizeChanged(const QString &arg1);
    void ecgBase_Kalman1Changed(const QString &arg1);
    void ecgBase_Kalman2Changed(const QString &arg1);
    void ecgBase_ButterworthCoeffSetChanged(int set);

    void stInterval_detectionWidthChanged(int arg1);
    void stInterval_smoothingWidthChanged(int arg1);
    void stInterval_morphologyChanged(double arg1);
    void stInterval_levelThresholdChanged(double arg1);
    void stInterval_slopeThresholdChanged(double arg1);
    void stInterval_algorithmChanged(int index);

public slots:
    void receivePatientData(EcgData *data);

    void receiveQRSData(QRSClass currClass, int type);
    void fbLoadData(const QString &directory, const QString &name);

    //modules recieve
    void drawEcgBaseline(EcgData* data);//example
    void drawAtrialFibr(EcgData* data);  //to zostawiam Krzyskowi
    void drawStInterval(EcgData* data);
    void drawHrv1(EcgData *data);
    void drawRPeaks(EcgData *data);
    void drawWaves(EcgData *data);
    void drawQrsClass(EcgData *data);
    void drawSigEdr(EcgData *data);
    void drawVcgLoop(EcgData* data);
    void drawSleep_Apnea(EcgData *data);
    void drawQtDisp(EcgData *data);
    void drawHrt(EcgData *data);

    //busy
    void busy(bool);

private slots:
    void on_actionO_programie_triggered();
    void on_actionWczytaj_triggered();
    void on_Hilbert_radiobutton_clicked();

    //MENU RUN

    void on_actionECG_BASELINE_triggered();
    void on_actionR_PEAKS_triggered();
    void on_actionWAVES_triggered();
    void on_actionHRV1_triggered();
    void on_actionSettings_triggered();
    void on_actionATRIAL_FIBR_triggered();
    void on_actionQT_DISP_triggered();
    void on_actionSLEEP_APNEA_triggered();
    void on_actionST_INTERVAL_triggered();
    void on_actionSIG_EDR_triggered();
    void on_actionVCG_LOOP_triggered();
    void on_actionHRT_triggered();





    void on_PanTompkins_radiobutton_clicked();

    void on_qrsclassestoolbox_changed(int index);

    void qrssample_changed(QString text);

    void qrcclasslabel_changed(QString value);


    void on_checkBox_toggled(bool checked);


    void on_qrsSettingsResultsButton_clicked();

    void on_qrscClassSettingsButton_clicked();

    void on_comboBox_currentIndexChanged(int index);

    void on_qrsSetKMaxIterSpinBox_valueChanged(int arg1);

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();
    void on_pushButton_10_clicked();

    void on_pushButton_12_clicked();

    void on_radioButton_clicked();

    void on_radioButton_2_clicked();

    void on_p_onset_toggled(bool checked);

    void on_butterworthRadioButton_clicked();

    void on_movingAverageRadioButton_clicked();

    void on_savitzkyGolayRadioButton_clicked();

    //void on_radioButton_5_clicked();

    void on_maTimeSpinBox_valueChanged(const QString &arg1);

    void on_ButterworthcomboBox_currentIndexChanged(int index);

    void on_Falkowa_radiobutton_clicked();

    void on_checkBox_2_clicked(bool checked);

    void on_pushButton_clicked();

    //void on_radioButton_5_clicked();

    void on_kalmanRadioButton_clicked();

    void on_pushButton_17_clicked();

    void on_pushButton_4_clicked();

    void on_radioButton_3_clicked();

    void on_radioButton_4_clicked();

    void on_pushButton_next_vcg_clicked();

    void on_pushButton_prev_vcg_clicked();

    void on_RUN_VCG_pushButton_clicked();

    void on_st_interval_detection_width_valueChanged(int arg1);

    void on_st_interval_smoothing_width_valueChanged(int arg1);

    void on_st_interval_morphology_valueChanged(double arg1);

    void on_st_interval_level_threshold_valueChanged(double arg1);

    void on_st_interval_slope_threshold_valueChanged(double arg1);

    void on_detectionratesquare_clicked();

    void on_detectionratelinear_clicked();

    void on_pushButton_11_clicked();

    void initEcgBaselineGui();

    void on_pushButton_16_clicked();

    void on_pushButton_18_clicked();

    void initStIntervalGui();
    void stItemSelected(int row, int column);
    void nextStAbnormality();
    void prevStAbnormality();

    void on_G_QRS_toggled(bool checked);

    void on_K_Iteration_valueChanged(int arg1);

    void on_G_Iteration_valueChanged(int arg1);

    void on_K_QRS_toggled(bool checked);

private:
    Ui::AirEcgMain *ui;
};

#endif // AIRECGMAIN_H
