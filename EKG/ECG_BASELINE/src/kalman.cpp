#include <functional>
#include <memory>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <fstream>

#include "kalman.h"
#include "butter.h"
#include "movAvg.h"
#include "utils.h"
#include "sgolay.h"
#include "QDebug"
#include "ECG_BASELINE/3rdparty/Eigen/Core"
#include "ECG_BASELINE/3rdparty/Eigen/LU"

#include "ECG_BASELINE/3rdparty/alglib/interpolation.h"

typedef BaseLineUtils<double> BLUtils;
#define PI 3.14159265

using alglib::real_2d_array;
using alglib::real_1d_array;
using alglib::ae_int_t;
using alglib::lsfitstate;
using alglib::lsfitreport;
using alglib::lsfitcreatef;
using alglib::lsfitsetcond;
using alglib::lsfitfit;
using alglib::lsfitresults;

/*my cmath is weird.*/
namespace Kalman {
    int round(double number) {
        return int(number + 0.5);
    }

    double rem(double numer, double denom) {
        int rqout = round (numer / denom);
        return (numer - rqout * denom);
    }
}

void fitFunc(const real_1d_array &c, const real_1d_array &x, double &func, void* /*ptr*/)
{
    func = KalmanFilter::syntheticEcgModelPoint(x[0], c);
}

KalmanFilter::KalmanFilter(): SNR(10.0), theta(0.0), bins(250), samplingFrequency(500), rrPeriod(samplingFrequency), gamma(1.0) {

}

//interface
//test against matlab impl without noise and bsRemoval
QVector<double> KalmanFilter::processKalman(const QVector<double> &ecgData) {
    QVector<double> output;
    QVector<double> preprocessedInput = removeBaseLine(ecgData);
    //EcgData noisyInput = addGaussianNoise(preprocessedInput, SNR); //optional
    QVector<double> rPeaks = findPeaks(preprocessedInput, rrPeriod);

    QVector<double> phaseVector = calculatePhase(rPeaks);
    shiftPhase(phaseVector, theta);

    EcgParams ecgParameters = calculateEcgParameters(preprocessedInput, phaseVector, bins);
    std::tie(ecgMean, ecgSD, ecgMeanPhase) = ecgParameters;

    QVector<double> pointsToOptimize = getEquidistPoints(20, 240, 15);
    QVector<double> peaks = findPeaks(ecgData, 250);
    pointsToOptimize.append(peaks.indexOf(1,0));

    QVector<double> optModelParameters = optimizeModelParameters(pointsToOptimize, ecgMean, ecgMeanPhase);

    QVector<double> finalSignal = performKalmanSmoothing(optModelParameters, rPeaks, preprocessedInput, phaseVector, ecgParameters);

    output = QVector<double>(finalSignal.size());
    qCopy(finalSignal.begin(), finalSignal.end(), output.begin());
    return output;
}

double KalmanFilter::syntheticEcgModelPoint(const double phase, const real_1d_array &params)
{
    int kernels = params.length() / 3;

    QVector<double> alpha(kernels);
    QVector<double> bi(kernels);
    QVector<double> teta(kernels);

    for(int i = 0; i < kernels; i++) {
        alpha[i] = params[i];
        bi[i] = params[i + kernels];
        teta[i] = params[i + 2 * kernels];
    }

    double output = 0;
    double dtetai = 0;

    for(int i = 0; i < kernels; i++) {
        dtetai = fmod(phase - teta[i] + PI, (2 * PI)) - PI;
        output = output + alpha[i] * exp(-pow(dtetai, 2) / (2 * pow(bi[i], 2)));
    }

    return output;
}

QVector<double> KalmanFilter::syntheticEcgModelVector(const QVector<double> &phase, const QVector<double> &params)
{
    int kernels = params.size() / 3;
    int size = phase.size();

    QVector<double> alpha(kernels);
    QVector<double> bi(kernels);
    QVector<double> teta(kernels);

    for(int i = 0; i < kernels; i++) {
        alpha[i] = params[i];
        bi[i] = params[i + kernels];
        teta[i] = params[i + 2 * kernels];
    }

    QVector<double> output(size);
    QVector<double> dtetai(size);

    for(int i = 0; i < kernels; i++) {
        for(int j = 0; j < size; j++) {
            dtetai[j] = fmod(phase[j] - teta[i] + PI, (2 * PI)) - PI;
            output[j] = output[j] + alpha[i] * exp(-pow(dtetai[j], 2) / (2 * pow(bi[i], 2)));
        }
    }

    return output;
}


//private

QVector<double> KalmanFilter::removeBaseLine(const QVector<double> &ecgData){
    return processMovAvg(ecgData, 100); //processButter(ecgData, predefinedButterCoefficientSets()[0]);
}

QVector<double> KalmanFilter::addGaussianNoise(QVector<double> &input, double SNR) {
    QVector<double> noisyOutput = BLUtils::getFullCopy(input);
    double signalPower = BLUtils::mean(BLUtils::pow(input, 2.0));
    double noisePower = signalPower / pow(10,(SNR/10));
    QVector<double> noise = BLUtils::randn(input.size());

    for(int i = 0; i < noisyOutput.size(); i++) {
        noisyOutput[i] += sqrt(noisePower) * noise[i];
    }

    return noisyOutput;
}

QVector<double> KalmanFilter::performKalmanSmoothing(const QVector<double> &modelParams, const QVector<double> &rPeaks, const QVector<double> &signal, const QVector<double> &phase, const EcgParams &signalParams) {
    QVector<double> finalSignal(signal.size());

    setEksInitParameters(modelParams, rPeaks, signal, phase, signalParams);
    constructEksMatrices(signalParams);

    QVector<double> filteredSignal = forwardFilter();
    QVector<double> smoothedSignal = backwardSmoothen();

    qCopy(smoothedSignal.begin(),smoothedSignal.end(), finalSignal.begin());
    return finalSignal;
}

QVector<double> KalmanFilter::forwardFilter() {
    EVector Yminus, C, G;
    int progress = 0;

    //set static variables inside functions. Refactoring very welcome.
    linearizeStates(Inits);
    updateStates(Inits);

    EMatrix a(nSamples, 2);
    qDebug() << "forward filtering start";
    for(int i = 0; i < nSamples; i++) {

        if(i > 0 && ((i%(nSamples/10)) == 0)){
            progress += 10;
            qDebug() << "forward filtering "<<progress<<"%";
        }

        if(abs(Xminus[0] - Y(i, 0)) > PI) {
            Xminus[0] = Y(i, 0);
        }

        //store results
        Xbar.col(i) = Xminus;
        Pbar[i] = Pminus;

        EVector XX(Xminus), Xplus;
        EMatrix PP(Pminus);
        EMatrix CC, GG, A, F;

        for(int j = 0; j < 2; j++) {
            //measurement update
            Yminus = updateObservations(XX, Vmean);
            double YY = Yminus(j);
            std::tie(CC, GG) = linearizeOutputEquations();
            C = CC.row(j);
            G = GG.row(j);

            double denominator = C.transpose()*PP*C;

            denominator += alpha * G.transpose() * R(j,j) * G;

            EVector K = (PP * C) / denominator;

            EMatrix PPelem = (EMatrix::Identity(L,L) - K*C.transpose());
            PP = (PPelem*PP*PPelem.transpose() + K*G.transpose()*R(j,j)*G*K.transpose())/alpha;
            XX = XX + K*(Y(i,j) - YY);
        }
        //innovation variance
        //qDebug() << "size Y, Yminus, C, Pminus" << Y.rows()<<","<<Y.cols()<<":"<<Yminus.rows()<<","<<Yminus.cols()<<":"<<C.rows()<<","<<C.cols()<<":"<<Pminus.rows()<<","<<Pminus.cols();
        EVector innVarK = Y.row(i).transpose() - Yminus;
        double Yk = C.transpose() * Pminus * C;
        Yk += G.transpose() * R * G;

        //circular buffer simulation
        for(int k = mem1.rows()-1; k > 0; k--) {
            mem1.row(k) = mem1.row(k-1);
        }
        for(int k = mem2.rows()-1; k > 0; k--) {
            mem2.row(k) = mem2.row(k-1);
        }

        mem1.row(0) << (pow(innVarK(0),2) / Yk), (pow(innVarK(1), 2) / Yk);
        mem2.row(0) << (pow(innVarK(0),2)), (pow(innVarK(1), 2));

        a.row(i) << mem1.col(0).mean(), mem1.col(1).mean();

        R(1,1) = gamma * R(1,1) + (1-gamma) * mem2.row(1).mean();

        Xplus = XX;
        Pplus = (PP + PP.transpose())/2;

        Xminus = updateStates(Xplus);
        std::tie(A,F) = linearizeStates(Xplus);
        Pminus = A*Pplus*A.transpose() + F*Q*F.transpose();

        //store results
        Xhat.col(i) = Xplus;
        Phat[i] = Pplus.transpose();
    }

    qDebug() << "forward filtering complete";
    QVector<double> filteredSignal(nSamples);
    EVector amplitudeVector = Xhat.row(1);

    std::copy(amplitudeVector.data(), amplitudeVector.data() + nSamples, filteredSignal.begin());

    return filteredSignal;
}

QVector<double> KalmanFilter::backwardSmoothen() {
    qDebug() << "xHat in backward loop";
    for(int i = 0; i < 100; i++) {
        qDebug() << Xhat(i);
    }

    EMatrix3 PSmoothed = create3dMatrix(L, L, nSamples);
    EMatrix X = EMatrix::Zero(L, nSamples);
    EMatrix A, F;
    PSmoothed[nSamples -1] = Phat[nSamples-1];

    int progress = 0;
    for(int i = nSamples-2; i >=0; i--) {

        if(i > 0 && ((i%(nSamples/10)) == 0)){
            progress += 10;
            qDebug() << "smoothing "<<progress<<"%";
        }

        std::tie(A, F) = linearizeStates(Xhat.col(i));
        Eigen::FullPivLU<EMatrix> lu(Pbar[i+1]);
        EMatrix S = Phat[i] * A.transpose() * lu.inverse();
        X.col(i) = Xhat.col(i) + S*(X.col(i+1) - Xbar.col(i+1));
        PSmoothed[i] = Phat[i] - S * (Pbar[i+1] - PSmoothed[i+1]) * S.transpose();
    }

    QVector<double> finalSignal(nSamples);
    EVector amplitudeVector = X.row(1);

    std::copy(amplitudeVector.data(), amplitudeVector.data() + nSamples, finalSignal.begin());

    return finalSignal;
}

EVector KalmanFilter::updateObservations(const EVector &x, const EVector &w) {
    EVector y(2);

    y(0) = x(0) + w(0);
    y(1) = x(1) + w(1);

    return y;
}

EVector KalmanFilter::updateStates(const EVector &x) {
    EVector xout(2);
    static bool initialized = false;

    static int L = (x.rows()-2)/3;
    EMatrix M(2,2), N(2, 3*L + 2);
    static EVector alphaI = x.segment(0,L);
    static EVector bI = x.segment(L, L);
    static EVector tetaI = x.segment(2*L, L);
    static double w = x(3*L);
    static double fs = x(3*L + 1);
    static double dt = 1.0 / fs;

    if(!initialized) {
        initialized = true;
        return xout;
    }

    xout(0) = x(0) + w*dt; //teta state variable
    if(xout(0) > PI) {
        xout(0) -= 2*PI;
    }

    EVector dTetaI(L);
    for(int i = 0 ; i < L; i++) {
        dTetaI(i) = Kalman::rem(xout(0) - tetaI(i), 2*PI);
    }

    double sum=0.0;
    for(int i = 0; i < L; i++) {
        sum += w*(alphaI(i)/pow(bI(i),2))*dTetaI(i)*exp(-pow(dTetaI(i),2)/(2*bI(i)*bI(i)));
    }
    xout(1) = x(1) - dt * sum; //z state variable

    return xout;
}

Linears KalmanFilter::linearizeOutputEquations() {
    EMatrix M(2,2), N(2,2);

    M(0,0) = 1;
    M(0,1) = 0;
    M(1,0) = 0;
    M(1,1) = 1;

    N(0,0) = 1;
    N(0,1) = 0;
    N(1,0) = 0;
    N(1,1) = 1;

    return std::make_tuple(M,N);
}

Linears KalmanFilter::linearizeStates(const EVector &x) {
    static bool initialized = false;

    static int L = (std::max(x.rows(),x.cols())-2)/3;
    EMatrix M(2,2), N(2, 3*L + 2);
    static EVector alphaI = x.segment(0,L);
    static EVector bI = x.segment(L, L);
    static EVector tetaI = x.segment(2*L, L);
    static double w = x(3*L);
    static double fs = x(3*L + 1);
    static double dt = 1.0 / fs;

    if(!initialized) {
        initialized = true;
        return std::make_tuple(M,N);
    }

    EVector dTetaI(L);
    for(int i = 0 ; i < L; i++) {
        dTetaI(i) = Kalman::rem(x(0) - tetaI(i), 2*PI);
    }

    M(0,0) = 1; //dF1/theta
    M(0,1) = 0; //dF1/dz

    double sum=0.0;
    for(int i = 0; i < L; i++) {
        sum += w*(alphaI(i)/pow(bI(i),2))*(1 - (pow(dTetaI(i),2)/pow(bI(i),2)) )*exp(-(pow(dTetaI(i),2)/(2*bI(i)*bI(i))));
    }

    M(1,0) = -dt * sum; //dF2/dtheta
    M(1,1) = 1; //dF2/dz

    N.row(0) = EVector::Zero(3*L+2);
    N(0,3*L) = dt;

    sum=0.0;
    for(int i = 0; i < L; i++) {
        sum = (-dt * w/pow(bI(i),2))* dTetaI(i)*exp(-(pow(dTetaI(i),2)/(2*bI(i)*bI(i))));
        N(1, i) = sum;
    }

    for(int i = 0; i < L; i++) {
        sum = 2*dt*alphaI(i)*w*(dTetaI(i)/pow(bI(i),3))*(1 - (pow(dTetaI(i),2)/2*pow(bI(i), 2)))*exp(-pow(dTetaI(i),2)/(2*pow(bI(i),2)));
        N(1, L+i) = sum;
    }

    for(int i = 0; i < L; i++) {
        sum = dt*w*alphaI(i)/(bI(i)*bI(i))*exp(-pow(dTetaI(i),2)/(2*bI(i)*bI(i)))*(1 - pow(dTetaI(i),2)/(bI(i)*bI(i)));
        N(1, 2*L+i) = sum;
    }

    sum = 0.0;
    for(int i = 0; i < L; i++) {
        sum += dt * alphaI(i)*dTetaI(i)/(bI(i)*bI(i))*exp(-pow(dTetaI(i),2)/(2*bI(i)*bI(i)));
    }
    N(1, 3*L) = -sum;
    N(1, 3*L+1) = 1;

    return std::make_tuple(M, N);
}

void KalmanFilter::constructEksMatrices(const EcgParams& signalParams) {

    std::tie(ecgMean, ecgSD, ecgMeanPhase) = signalParams;

    nSamples = Y.rows();
    L = std::max(x0.rows(), x0.cols());
    Pminus = P0;
    Pplus = EMatrix::Zero(L, L);
    Xminus = x0;
    Xbar = EMatrix::Zero(L, nSamples);
    Pbar = create3dMatrix(L, L, nSamples);
    Xhat = EMatrix::Zero(L, nSamples);
    Phat = create3dMatrix(L, L, nSamples);

    double meanHRsd = BLUtils::mean( ecgSD, 0, Kalman::round(0.1 * ecgSD.size()));
    int adaptiveWindow = std::ceil(0.5 * samplingFrequency);
    mem1 = EMatrix::Ones(innovationWindow, 2);
    mem2 = EMatrix::Ones(adaptiveWindow, 2) * meanHRsd;

    alpha = 1; //forgetting factor
    R = R0;
}

EMatrix3 KalmanFilter::create3dMatrix(int rows, int cols, int slices){
    EMatrix3 output;
    EMatrix element(rows, cols);

    for(int i = 0; i < slices; i++) {
        output.push_back(element);
    }

    return output;
}

void KalmanFilter::setEksInitParameters(const QVector<double> &optModelParams, const QVector<double> &rPeaks, const QVector<double> &signal, const QVector<double>& phase, const EcgParams &signalParams) {

    std::tie(ecgMean, ecgSD, ecgMeanPhase) = signalParams;

    int N = optModelParams.size() / 3;
    QVector<int> rPeakIndexes = BLUtils::find(rPeaks);
    QVector<double> rPeakDiffs = BLUtils::diffD(rPeakIndexes);
    QVector<double> heartBeatRates = BLUtils::mul(rPeakDiffs, 1.0/samplingFrequency);
    QVector<double> heartBeatRads = BLUtils::mul(heartBeatRates, 2*PI);
    double HRaverage = BLUtils::mean(heartBeatRads);
    double HRsd = BLUtils::sd(heartBeatRads);
    double meanHRsdStart = BLUtils::mean( ecgSD, 0, Kalman::round(0.1 * ecgSD.size()));

    Y = EMatrix::Zero(signal.size(), 2);
    copyToY(Y, phase, signal);

    x0 = EVector(2);
    x0[0] = -PI; x0[1] = 0;

    P0 = EMatrix::Zero(2,2);
    P0(0,0) = pow((2*PI), 2);
    P0(1,1) = pow(10 * std::abs(BLUtils::getMaxValue(signal)), 2);

    EVector Qinit = EVector(3*N + 2);

    for(int i = 0; i < N; i++) {
        Qinit(i) = pow(0.1 * optModelParams[i], 2);
    }
    for(int i = 0; i < 2*N; i++) {
        Qinit(N+i) = pow(0.05, 2);
    }

    Qinit(3*N) = pow(HRsd, 2);
    Qinit(3*N+1) = pow(0.05*meanHRsdStart, 2);

    Q = Qinit.asDiagonal();
    R0 = EMatrix::Zero(2,2);

    R0(0,0) = pow(HRaverage / samplingFrequency, 2) / 12.0;
    R0(1,1) = pow(meanHRsdStart, 2);

    Wmean = EVector(3*N + 2);
    for(int i = 0; i < 3*N; i++) {
        Wmean[i] = optModelParams[i];
    }
    Wmean[3*N] = HRaverage;
    Wmean[3*N+1] = 0;

    Vmean = EVector::Zero(2);

    Inits= EVector(3*N + 2);
    for(int i = 0; i < 3*N; i++) {
        Inits[i] = optModelParams[i];
    }
    Inits[3*N] = HRaverage;
    Inits[3*N+1] = samplingFrequency;

    innovationWindow = std::ceil(0.5 * samplingFrequency);
}

void KalmanFilter::copyToY(EMatrix &Y, const QVector<double> &phase, const QVector<double> &signal) {
    for(int i = 0; i < signal.size(); i++) {
        Y(i, 0) = phase[i];
        Y(i, 1) = signal[i];
    }
}

QVector<double> KalmanFilter::runFitting(const QVector<double> &initParams, const QVector<double> &ecgMean, const QVector<double> &phaseMean)
{
    assert(ecgMean.size() == phaseMean.size());

    double *x_arr = new double[phaseMean.size()];
    double *y_arr = new double[ecgMean.size()];
    double *params_arr = new double[initParams.size()];

    for(int i = 0; i < phaseMean.size(); i++) {
        x_arr[i] = phaseMean[i];
        y_arr[i] = ecgMean[i];
    }

    for(int i = 0; i < initParams.size(); i++) {
        params_arr[i] = initParams[i];
    }

    real_2d_array x;
    x.setcontent(phaseMean.size(), 1, x_arr);
    //qDebug() << x.tostring(6).data();

    real_1d_array y;
    y.setcontent(ecgMean.size(), y_arr);
    //qDebug() << y.tostring(6).data();

    real_1d_array c;
    c.setcontent(initParams.size(), params_arr);
    //qDebug() << c.tostring(6).data();

    delete [] x_arr;
    delete [] y_arr;
    delete [] params_arr;

    double epsf = 0;
    double epsx = 0.000000001;
    ae_int_t maxits = 1000;
    ae_int_t info;
    lsfitstate state;
    lsfitreport rep;
    double diffstep = 0.000000001;

    lsfitcreatef(x, y, c, diffstep, state);
    lsfitsetcond(state, epsf, epsx, maxits);
    lsfitfit(state, ::fitFunc);
    lsfitresults(state, info, c, rep);

    qDebug() << "---------------------------";
    qDebug() << "Optimal synthetic ECG model parameters:";
    qDebug() << c.tostring(6).c_str();
    qDebug() << "---------------------------";

    QVector<double> optParams(c.length());
    for(int i = 0; i < c.length(); i++) {
        optParams[i] = c[i];
    }

    return optParams;
}

int KalmanFilter::findMaxIndex(const QVector<double>& absEcgMean)
{
    double maxValue = std::numeric_limits<double>::min();
    for(int i = 0; i < absEcgMean.size(); i++) {
        if(absEcgMean[i] > maxValue) {
            maxValue = absEcgMean[i];
        }
    }

    return 0;
}

QVector<double> KalmanFilter::optimizeModelParameters(const QVector<double> selectedTs, const QVector<double> &ecgMean, const QVector<double> &ecgPhase)
{
    int ecgLen = ecgMean.size();
    int tCount = selectedTs.size();

    QVector<int> selIndexes(tCount);

    QVector<double> absEcgMean = BLUtils::abs(ecgMean);
    int maxIndex = findMaxIndex(absEcgMean);

    selIndexes[0] = maxIndex;
    for(int i = 1; i < tCount; i++) {
        int index = Kalman::round(selectedTs[i]);
        selIndexes[i] = index < ecgLen ? index : ecgLen - 1;
    }

    std::sort(selIndexes.begin(), selIndexes.end());

    int selCnt = selIndexes.size();
    QVector<double> initTeta;
    QVector<double> initAlpha;
    QVector<double> initBi(selCnt);
    for(int i = 0; i < selCnt; i++) {
        initTeta.append(ecgPhase[selIndexes[i]]);
        initAlpha.append(1.2 * ecgMean[selIndexes[i]]);
        initBi[i] = 0.04;
    }

    // [alpha bi teta]
    QVector<double> initialParams(3 * selCnt);
    for(int i = 0; i < selCnt; i++) {
        initialParams[i] = initAlpha[i];
        initialParams[i + selCnt] = initBi[i];
        initialParams[i + 2 * selCnt] = initTeta[i];
    }

    return runFitting(initialParams, ecgMean, ecgPhase);
}

QVector<double> KalmanFilter::getEquidistPoints(const double begin, const double end, const int count)
{
    QVector<double> ret(count);
    for(int i = 0; i < count; i++) {
        ret[i] = begin + i * (end - begin) / count;
    }
    return ret;
}

QVector<double> KalmanFilter::findPeaks(const QVector<double> &input, double rrPeriod) {
    assert(input.size() > 2*rrPeriod);
    QVector<double> rPeaks(input.size(), 0);
    int windowLength = (int)std::floor(0.5*rrPeriod);
    double absMax = std::abs(BLUtils::getMaxValue(input));
    double absMin = std::abs(BLUtils::getMinValue(input));

    performPreliminaryIndexation(input, rPeaks, windowLength, (absMax > absMin));
    filterOutFakePeaks(rPeaks, windowLength);

    debugPrintRPeaks(rPeaks);
    return rPeaks;
}

/*Create 'chainsaw' from -PI to PI, Phase at R peak = 0
* Handle beginning and ending of a signal separately,
* by extrapolating R peaks if necessary.
*/
QVector<double> KalmanFilter::calculatePhase(const QVector<double> &rPeaks) {
    QVector<double> output = QVector<double>(rPeaks.size(), 0);

    QVector<int> rPeaksIndexes = BLUtils::find(rPeaks);
    for(int i = 0; i < rPeaksIndexes.size()-1; i++) {
        BLUtils::linspace(output, rPeaksIndexes[i]-1, rPeaksIndexes[i+1], 0.0, 2*PI);
    }

    //handle beginning of the signal
    double beginningDistance = rPeaksIndexes[1] - rPeaksIndexes[0];
    double startValue = 2*PI - (rPeaksIndexes[0]-1)*2*PI/beginningDistance;
    double endValue = 2*PI;
    BLUtils::linspace(output, 0, rPeaksIndexes[0], 2*PI/beginningDistance, startValue, endValue);

    //handle the end of the signal
    double endDistance = rPeaksIndexes[rPeaksIndexes.size()-1] - rPeaksIndexes[rPeaksIndexes.size()-2];
    int dataPointsLeft = rPeaks.size() - rPeaksIndexes[rPeaksIndexes.size()-1];

    int lastPeak = rPeaksIndexes[rPeaksIndexes.size()-1];
    BLUtils::linspace(output, lastPeak+1,
            rPeaks.size(),2*PI/endDistance,2*PI/endDistance, dataPointsLeft*2*PI/endDistance);

    BLUtils::modulo(output, 2*PI); //trim beginning and end especially ('extrapolating')
    scaleToPi(output);

    return output;
}

void KalmanFilter::shiftPhase(QVector<double>& phaseInput, double theta) {
    BLUtils::add(phaseInput, theta);
    BLUtils::add(phaseInput, PI);
    BLUtils::modulo(phaseInput, 2*PI);
    BLUtils::add(phaseInput, -PI);
}

EcgParams KalmanFilter::calculateEcgParameters(const QVector<double> &signal,QVector<double> &phase, int bins) {
    QVector<double> ecgMean(bins,0), ecgSD(bins,0), ecgMeanPhase(bins,0);

    calculateFirstBin(ecgMean, ecgSD, ecgMeanPhase, signal, phase);

    for(int i = 1; i < bins; i++) {
        auto predicate = [=](double value){return ((value >= (2*PI*((double)i-0.5)/(double)bins) - PI) && (value < (2*PI*((double)i+0.5)/(double)bins) - PI));};
        QVector<int> indexes = BLUtils::find(phase, predicate);
        if(indexes.empty()) {
            ecgMeanPhase[i] = 0;
            ecgMean[i] = 0;
            ecgSD[i] = -1;
        }else {
            ecgMeanPhase[i] = BLUtils::mean(phase, indexes);
            ecgMean[i] = BLUtils::mean(signal, indexes);
            ecgSD[i] = BLUtils::sd(signal, indexes);
        }
    }

    smoothenEcgParameters(ecgMean, ecgSD, ecgMeanPhase);

    alignBaselineToZero(ecgMean);

    //debugPrintEcgParameters(ecgMean, ecgSD, ecgMeanPhase);
    //debugWriteEcgParametersToFile(ecgMean, ecgSD, ecgMeanPhase);
    return std::make_tuple(ecgMean, ecgSD, ecgMeanPhase);
}

void KalmanFilter::debugPrintEcgParameters(const QVector<double> &mean, const QVector<double> &sd, const QVector<double> &meanPhase) {
    qDebug() << mean;
    qDebug() << sd;
    qDebug() << meanPhase;
}

void KalmanFilter::debugWriteEcgParametersToFile(const QVector<double> &mean, const QVector<double> &sd, const QVector<double> &meanPhase) {
    std::ofstream outFile("cpp-params.txt");
    outFile << "ECGMean = " << std::endl;
    for(int i = 0; i < mean.size(); i++) {
        outFile <<  std::setprecision(3) << mean[i] << std::endl;
    }
    outFile << std::endl;
    outFile << "ECGsd = " << std::endl;
    for(int i = 0; i < sd.size(); i++) {
        outFile << std::setprecision(3) << sd[i] << std::endl;
    }
    outFile << std::endl;
    outFile << "meanphase = " << std::endl;
    for(int i = 0; i < meanPhase.size(); i++) {
        outFile << std::setprecision(3) << meanPhase[i] << std::endl;
    }

    outFile.close();
}

void KalmanFilter::calculateFirstBin(QVector<double>& ecgMean, QVector<double>& ecgSD, QVector<double>& ecgMeanPhase,const QVector<double>& signal,const QVector<double>& phase) {
    auto predicate = [=](double value) {return ((value >= (PI- PI/(double)bins)) || (value < (-PI + PI/(double)bins)));};
    QVector<int> indexes = BLUtils::find(phase, predicate);

    if(indexes.empty()) {
        ecgMeanPhase[0] = 0;
        ecgMean[0] = 0;
        ecgSD[0] = -1;
    }else {
        ecgMeanPhase[0] = -PI;
        ecgMean[0] = BLUtils::mean(signal, indexes);
        ecgSD[0] = BLUtils::sd(signal, indexes);
    }
}

void KalmanFilter::smoothenEcgParameters(QVector<double> &ecgMean, QVector<double> &ecgSD, QVector<double> &ecgMeanPhase) {
    auto predicate = [](double value){return (value==-1);};
    QVector<int> indexes = BLUtils::find(ecgSD, predicate);
    qDebug() << "indexes = " << indexes;
    for(int i = 0 ;i < indexes.size(); i++) {
        if(indexes[i] == 0) {
            ecgMeanPhase[indexes[i]] = -PI;
            ecgMean[indexes[i]] = ecgMean[indexes[i] + 1];
            ecgSD[indexes[i]] = ecgSD[indexes[i] + 1];
        }else if(indexes[i] == bins-1) {
            ecgMeanPhase[indexes[i]] = PI;
            ecgMean[indexes[i]] = ecgMean[indexes[i] - 1];
            ecgSD[indexes[i]] = ecgSD[indexes[i] - 1];
        }else {
            ecgMeanPhase[indexes[i]] = 0.5 * (ecgMeanPhase[indexes[i] - 1] + ecgMeanPhase[indexes[i] + 1]);
            ecgMean[indexes[i]] = 0.5 * (ecgMean[indexes[i] - 1] + ecgMean[indexes[i] + 1]);
            ecgSD[indexes[i]] = 0.5 * (ecgSD[indexes[i] - 1] + ecgSD[indexes[i] + 1]);
        }
    }
}

void KalmanFilter::alignBaselineToZero(QVector<double> &ecgMean){
    int smoothingWindowLength = std::ceil(ecgMean.size()/10);

    double firstPartMean = BLUtils::mean(ecgMean, 0, smoothingWindowLength);
    BLUtils::add(ecgMean, -firstPartMean);

}

void KalmanFilter::scaleToPi(QVector<double> &phaseInput){
    for(int i = 0; i < phaseInput.size(); i++) {
        if(phaseInput[i] > PI)
            phaseInput[i] -= 2*PI;
    }
}

void KalmanFilter::performPreliminaryIndexation(const QVector<double>& input, QVector<double>& rPeaks, int windowLength, bool isAbsMaxGreater) {
    for(int i = 0; i < rPeaks.size(); i++) {
        int indexBegin = 0, indexEnd = 0;
        establishIndexBounds(rPeaks, i, windowLength, &indexBegin, &indexEnd);

        if(isAbsMaxGreater) {
            if(isMaxElementInWindow(input, i, indexBegin, indexEnd)) {
                rPeaks[i] = 1;
            }
        }else {
            if(isMinElementInWindow(input, i, indexBegin, indexEnd)) {
                rPeaks[i] = 1;
            }
        }
    }
}

/*remove peaks which frequency is more than two times higher than expected*/
void KalmanFilter::filterOutFakePeaks(QVector<double>& rPeaks, int windowLength) {
    QVector<int> indexes = BLUtils::find(rPeaks);
    QVector<int> diffs = BLUtils::diff(indexes);

    for(int i =0; i < diffs.size(); i++) {
        if(diffs[i] < windowLength) {
            rPeaks[indexes[i]] = 0;
        }
    }
}

void KalmanFilter::establishIndexBounds(const QVector<double>& input, int i, int windowLength, int* indexBegin, int* indexEnd) {
    if(i < windowLength) {
        *indexBegin = 0;
        *indexEnd = 2*windowLength;
    }else if(i >= windowLength && i < input.size() - windowLength) {
        *indexBegin = i - windowLength;
        *indexEnd = i + windowLength;
    }else {
        *indexBegin = input.size() - 2*windowLength;
        *indexEnd = input.size();
    }
}

bool KalmanFilter::isMaxElementInWindow(const QVector<double>& input,int index, int indexBegin, int indexEnd) {
    return (BLUtils::getMaxValue(input, indexBegin, indexEnd) == input[index]);
}

bool KalmanFilter::isMinElementInWindow(const QVector<double>& input,int index, int indexBegin, int indexEnd) {
    return (BLUtils::getMinValue(input, indexBegin, indexEnd) == input[index]);
}

void KalmanFilter::debugPrintRPeaks(const QVector<double> &rPeaks) {
    QVector<int> nonZeroIndexes = BLUtils::find(rPeaks);
    qDebug() << "rPeaks:";
    for(int i = 0; i < nonZeroIndexes.size(); i++) {
        qDebug() <<"peak("<<i<<") = "<<nonZeroIndexes[i];
    }
}
