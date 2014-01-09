#include "butter.h"

#include <QtAlgorithms>
#include <cmath>
#include <limits>

namespace detail
{
double abs(const complex& c)
{
    return std::sqrt(c.real()*c.real() + c.imag()*c.imag());
}

double angle(const complex& c)
{
    return std::atan2(c.imag(), c.real());
}
}

QVector<ButterCoefficients> gCoeffs;
bool initButter()
{
    gCoeffs.clear();

    // old
    double bwMatrix1[] = {
        0.993848328562109, -1.987696657124219, 0.993848328562109, -1.987658813704708, 0.987734500543730,
        0.991435680867689, -1.982871361735378, 0.991435680867689, -1.982833610182527, 0.982909113285588,
        0.778659906650650, -1.557319813301300, 0.778659906650650, -1.997197410205620, 0.997273460258999,
        1.000000000000000, -2.000000000000000, 1.000000000000000, -1.984493614355032, 0.984569180668372,
        1.000000000000000, -2.000000000000000, 1.000000000000000, -1.992031885495420, 0.992107738853956
    };
    complex bwPoles1[] = {
        complex(0.993829406852354, +0.006132749728139),
        complex(0.993829406852354, -0.006132749728139),
        complex(0.991416805091264, +0.001353465263365),
        complex(0.991416805091264, -0.001353465263365),
        complex(0.998598705102810, +0.008607347209794),
        complex(0.998598705102810, -0.008607347209794),
        complex(0.992246807177516, +0.003931197577972),
        complex(0.992246807177516, -0.003931197577972),
        complex(0.996015942747710, +0.007744717318717),
        complex(0.996015942747710, -0.007744717318717)
    };
    double bwGain1 = 1.267517578260692;
    ButterCoefficients bwCoeffs1 = ButterCoefficients("Baseline wander removal (Fs: 360Hz) - [HP, Order: 10, -3db: 0.5Hz]",
                                                      360, bwGain1,
                                                      bwPoles1, sizeof(bwPoles1) / sizeof(complex),
                                                      bwMatrix1, sizeof(bwMatrix1) / sizeof(double));
    gCoeffs.append(bwCoeffs1);

    // 4
    double bwMatrix2[] =
    {
        1, -2, 1, -1.99591858803741, 0.996076189822218,
        1, -2, 1, -1.98849798039776, 0.988654996236322,
        1, -2, 1, -1.98222892979253, 0.982385450614125,
        1, -2, 1, -1.97769893355394, 0.977855096677832,
        1, -2, 1, -1.97532566755629, 0.975481643282286
    };
    complex bwPoles2[] =
    {
        complex(0.997959294018703, +0.0123869812266304),
        complex(0.997959294018703, -0.0123869812266304),
        complex(0.994248990198879, +0.0111329117858040),
        complex(0.994248990198879, -0.0111329117858040),
        complex(0.991114464896264, +0.0088072746929365),
        complex(0.991114464896264, -0.0088072746929365),
        complex(0.988849466776969, +0.0056416959096415),
        complex(0.988849466776969, -0.0056416959096415),
        complex(0.987662833778147, +0.0019416631032573),
        complex(0.987662833778147, -0.0019416631032573)
    };
    double bwGain2 = 0.960630619771866;
    ButterCoefficients bwCoeffs2 = ButterCoefficients("Baseline wander removal (Fs: 250Hz) [HP, Order: 10, -3db: 0.5Hz]",
                                                      250, bwGain2,
                                                      bwPoles2, sizeof(bwPoles2) / sizeof(complex),
                                                      bwMatrix2, sizeof(bwMatrix2) / sizeof(double));
    gCoeffs.append(bwCoeffs2);

    // 1
    double acMatrix1[] =
    {
        1, -1.28562417200218, 1, -1.26937327532045,	0.994584016960900,
        1, -1.28562417200218, 1, -1.29478401747452,	0.994658733271036,
        1, -1.28562417200218, 1, -1.26873553509657,	0.985919061981878,
        1, -1.28562417200218, 1, -1.28442034996815,	0.986038923952812,
        1, -1.28562417200218, 1, -1.27450176361204,	0.982697263115690
    };
    complex acPoles1[] =
    {
        complex(0.634686637660226, +0.769257361964419),
        complex(0.634686637660226, -0.769257361964419),
        complex(0.647392008737258, +0.758645055539264),
        complex(0.647392008737258, -0.758645055539264),
        complex(0.634367767548287, +0.763869489819878),
        complex(0.634367767548287, -0.763869489819878),
        complex(0.642210174984075, +0.757367160035168),
        complex(0.642210174984075, -0.757367160035168),
        complex(0.637250881806022, +0.759347467733407),
        complex(0.637250881806022, -0.759347467733407)
    };
    double acGain1 = 0.972154570931970;
    ButterCoefficients acCoeffs1 = ButterCoefficients("AC 50Hz noise removal (Fs: 360Hz) [BS, Order: 10]",
                                                      360, acGain1,
                                                      acPoles1, sizeof(acPoles1) / sizeof(complex),
                                                      acMatrix1, sizeof(acMatrix1) / sizeof(double));
    gCoeffs.append(acCoeffs1);


    // 2
    double acMatrix2[] =
    {
        1, 1.73211676126764, 1,	1.71899593699765, 0.994544262243386,
        1, 1.73211676126764, 1,	1.73568383808774, 0.994698492564272,
        1, 1.73211676126764, 1,	1.71472803758620, 0.985855276540036,
        1, 1.73211676126764, 1,	1.72512958096071, 0.986102721276756,
        1, 1.73211676126764, 1,	1.71713158098109, 0.982697263115690
    };
    complex acPoles2[] =
    {
        complex(-0.859497968498826, +0.505774163426502),
        complex(-0.859497968498826, -0.505774163426502),
        complex(-0.867841919043871, +0.491476241658255),
        complex(-0.867841919043871, -0.491476241658255),
        complex(-0.857364018793099, +0.500781604912744),
        complex(-0.857364018793099, -0.500781604912744),
        complex(-0.862564790480355, +0.492021039692754),
        complex(-0.862564790480355, -0.492021039692754),
        complex(-0.858565790490543, +0.495542174305113),
        complex(-0.858565790490543, -0.495542174305113)
    };
    double acGain2 = 0.972154570931970;
    ButterCoefficients acCoeffs2 = ButterCoefficients("AC 50Hz 3rd harm. noise removal (Fs: 360Hz) [BS, Order: 10]",
                                                      360, acGain2,
                                                      acPoles2, sizeof(acPoles2) / sizeof(complex),
                                                      acMatrix2, sizeof(acMatrix2) / sizeof(double));
    gCoeffs.append(acCoeffs2);

    // 3
    double acMatrix3[] =
    {
        1, -0.618082789968685, 1, -0.638258379756396, 0.992294218920832,
        1, -0.618082789968685, 1, -0.592950693262421, 0.992234368213770,
        1, -0.618082789968685, 1, -0.625753393665374, 0.979919684752645,
        1, -0.618082789968685, 1, -0.597905224192090, 0.979824027498247,
        1, -0.618082789968685, 1, -0.610411726197078, 0.975177876180649
    };
    complex acPoles3[] =
    {
        complex(0.319129189878198, +0.943636995400518),
        complex(0.319129189878198, -0.943636995400518),
        complex(0.296475346631210, +0.950966212361761),
        complex(0.296475346631210, -0.950966212361761),
        complex(0.312876696832687, +0.939163381596468),
        complex(0.312876696832687, -0.939163381596468),
        complex(0.298952612096045, +0.943637304910737),
        complex(0.298952612096045, -0.943637304910737),
        complex(0.305205863098539, +0.939163062152108),
        complex(0.305205863098539, -0.939163062152108)
    };
    double acGain3 = 0.960148851838195;
    ButterCoefficients acCoeffs3 = ButterCoefficients("AC 50Hz noise removal (Fs: 250Hz) [BS, Order: 10]",
                                                      250, acGain3,
                                                      acPoles3, sizeof(acPoles3) / sizeof(complex),
                                                      acMatrix3, sizeof(acMatrix3) / sizeof(double));
    gCoeffs.append(acCoeffs3);

    return true;
}

bool initialized = initButter();

const QVector<ButterCoefficients>& predefinedButterCoefficientSets()
{
    return gCoeffs;
}

QVector<double> sosfilt(const QVector<double>& sosMatrix,
                        double gain,
                        const QVector<double>& x)
{
    Q_ASSERT(!(sosMatrix.size() % 5));

    int length = x.size();
    QVector<double> y(length, 0);
    QVector<double> tmp = x;
    int sosLength = sosMatrix.size() / 5;

    for(int k = 0; k < sosLength; ++k)
    {
        double v1 = 0.0f, v2 = 0.0f;

        double b0 = sosMatrix[k * 5 + 0];
        double b1 = sosMatrix[k * 5 + 1];
        double b2 = sosMatrix[k * 5 + 2];
        double a1 = sosMatrix[k * 5 + 3];
        double a2 = sosMatrix[k * 5 + 4];

        for(int n = 0; n < length; ++n)
        {
            // Assuming a0 = 1:
            // y[n] + a1*y[n-1] + a2*y[n-2] = b0*x[n] + b1*x[n-1] + b2*x[n-2]
            double v0 = tmp[n] - a1*v1 - a2*v2;
            y[n] = b0*v0 + b1*v1 + b2*v2;
            v2 = v1;
            v1 = v0;
        }

        tmp = y;
    }

    if(fabsf(gain - 1.0f) > std::numeric_limits<double>::epsilon())
    {
        for(int n = 0; n < length; ++n)
            y[n] *= gain;
    }

    return y;
}

QVector<double> sosfiltfilt(const QVector<double>& sosMatrix,
                            double gain,
                            const QVector<complex>& poles,
                            const QVector<double>& x)
{
    int transient = 0;
    static const double PI = 3.141592653589793f;
    int l = x.size();

    // Approximate duration of startup transient
    for(int i = 0; i < poles.size(); ++i)
    {
        double t = PI / (detail::abs(poles[i]) * detail::angle(poles[i]));
        transient = int(ceilf(qMax(double(transient), t)));
    }
    transient = qMin(transient, l - 1);

    // Pad with reflected data to reduce startup transients
    double pre = x[0];
    double suf = x[l-1];

    QVector<double> xx;
    xx.reserve(transient*2 + l);
    for(int index = transient; index >= 1; --index)
        xx.append(2 * pre - x[index]);
    for(int index = 0; index < l; ++index)
        xx.append(x[index]);
    for(int index = l-2; index >= l-transient-1; --index)
        xx.append(2 * suf - x[index]);

    // Forward filter data
    QVector<double> y = sosfilt(sosMatrix, 1.0f, xx);

    // Reverse result
    int ll = y.size();
    QVector<double> yflip(ll);
    for(int i = 0; i < ll; ++i)
        yflip[i] = y[ll - i - 1];

    // Reverse filter data
    yflip = sosfilt(sosMatrix, 1.0f, yflip);

    // Reverse result
    for(int i = 0; i < ll; ++i)
        y[i] = yflip[ll - i - 1];

    // Remove transients
    QVector<double> tmp = y;
    y.resize(l);
    qCopy(tmp.begin() + transient, tmp.begin() + tmp.size() - transient, y.begin());


    if(fabsf(gain - 1.0f) > std::numeric_limits<double>::epsilon())
    {
        for(int n = 0; n < l; ++n)
            y[n] *= gain;
    }

    return y;
}

QVector<double> processButter(const QVector<double>& signal, const ButterCoefficients &coefficients)
{
    return sosfiltfilt(coefficients.coefficients(), coefficients.gain(), coefficients.poles(), signal);
}
