#ifndef AIRECGMAIN_H
#define AIRECGMAIN_H

#include <QMainWindow>
//#include "Common/ecginfo.h"
//#include "Common/ecgdata.h"
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <QListWidgetItem>
#include "scrollzoomer.h"

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


/*  to trzeba zostawić - ti będzie obiekt results
    void drawEcgBaseline(EcgData* data);
    void drawRPeaks(EcgData* data);
    void drawHrv1(EcgData* data);
    void drawHrv2(EcgData* data);
    void drawHrvDfa(EcgData* data);
    void drawTwa(EcgData* data);
    void drawWaves(EcgData* data);
    void drawQrsClass(EcgData* data);
    void drawStInterval(EcgData* data);
    void drawHrt(EcgData *data);

    void resetQrsToolbox(EcgData* data);*/
public:

    explicit AirEcgMain(QWidget *parent = 0);
    ~AirEcgMain();
    QwtPlot* plotPlot(QList<int> &y, float freq);
    QwtPlot* plotPlot(const QList<double>& y, float freq);
    QwtPlot* plotHrt(QList<double>& y);
    QwtPlot* plotLogPlot(QList<double> &x, QList<double> &y, int rodzaj);
    QwtPlot* plotBarChart(QList<unsigned int> &x, QList<int> &y);
    QwtPlot* plotPointsPlot(QList<unsigned int> &p, QList<int> &y, float freq);
    //HRV1*****************
    QwtPlot* plotPointsPlotDoubleToDouble(QList<double> &x, QList<double> &y);
    QwtPlot* plotLogPlotF(QList<double> &x,QList<double> &y,int rodzaj);
    QwtPlot* plotPoints(QList<double> &x, QList<double> &y,
                        QList<double> fftSamplesX, QList<double> fftSamplesY,
                        QList<double> interpolateX,  QList<double> interpolateY);
    //*********************
    QwtPlot* plotPoincarePlot(QList<unsigned int> &x, QList<int> &y, double &sd1, double &sd2);
    QwtPlot* plotTWAPlot(QList<double> &VI_values, QList<unsigned int> &TWA_positive, QList<unsigned int> &TWA_negative, float freq);
    QwtPlot* plotTWAPlot2(QList<unsigned int> &TWA_positive, QList<double> &TWA_positive_value, QList<unsigned int> &TWA_negative, QList<double> &TWA_negative_value);
    QwtPlot* plotPlotdfa(QList<double> &y1, QList<double> &y2);
    QwtPlot* plotPointsPlotDFA(QList<double> &x, QList<double> &y, double &wsp_a, double &wsp_b);
 QwtPlot* plotIntervalPlot(QList<double> &ecgbaselined, QList<int> &stbegin, QList<int> &stend, double samplingFrequency);

signals:
    void loadEntity(const QString &directory, const QString &name);   //podpiety
    void switchSignal(int index);
    void switchEcgBaseline(int type);
    void switchRPeaks(unsigned char type);
    void switchTWA(unsigned char type);
    void run();
    void test(int index, int type);
    void runSingle(QString hash);
    void closeDialog();

    void qrsClassChanged(int index, int type);
       void qrsMaxIterationsChanged(int maxIters);
    void qrsParallelExecutionChanged(bool flag);
    void qrsGMinClustersChanged(int minClusters);
    void qrsGMaxClustersChanged(int maxClusters);
    void qrsGMaxKIterations(int maxIters);
    void qrsKClustersNumberChanged(int noClusters);

public slots:

//    void receivePatientData(EcgData *data);
//    void receiveResults(EcgData *data);
      void fbLoadData(const QString &directory, const QString &name);
//    void receiveSingleProcessingResult(bool succeeded, EcgData *data);
//    void processingDialog();

private slots:
    void on_actionO_programie_triggered();
    void on_actionWczytaj_triggered();
    void on_load_clicked();

    void on_Hilbert_radiobutton_clicked();

    void on_PanTompkins_radiobutton_clicked();

    void on_qrsclassestoolbox_changed(int index);

    void qrssample_changed(QString text);

    void qrcclasslabel_changed(QString value);



    void on_checkBox_toggled(bool checked);

    void on_qrsSettingsResultsButton_clicked();

    void on_qrscClassSettingsButton_clicked();

    //void on_comboBox_currentIndexChanged(int index);

    void on_qrsSetGMaxItersSpinBox_valueChanged(int arg1);

    void on_qrsSetGinKMaxIterations_valueChanged(int arg1);

    void on_qrsSetGMinClusterSpinBox_valueChanged(int arg1);

    void on_qrsSetGMaxClusterSpinBox_valueChanged(int arg1);

    void on_qrsSettingsGMeansParallelCheckBox_toggled(bool checked);

    void on_qrsSetKMaxIterSpinBox_valueChanged(int arg1);

    void on_qrsSetKClusterNumSpinBox_valueChanged(int arg1);

    void on_qrsSetKMeansParallelCheckBox_toggled(bool checked);

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_9_clicked();

    void on_pushButton_10_clicked();

   // void on_pushButton_12_clicked();

    void on_radioButton_clicked();

    void on_radioButton_2_clicked();

    void on_p_onset_toggled(bool checked);

    void on_pushButton_12_clicked();

private:
    Ui::AirEcgMain *ui;
};

#endif // AIRECGMAIN_H
