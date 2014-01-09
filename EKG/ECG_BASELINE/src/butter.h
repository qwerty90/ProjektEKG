#pragma once

#include <QString>
#include <QVector>
#include <complex>

typedef std::complex<double> complex;


class ButterCoefficients
{
public:
    ButterCoefficients()
    { }

    ButterCoefficients(const QString &name, const int samplingHz, const double sosGain, const complex sosPoles[],
                       const int polesSize, const double sosCoeffs[], const int coeffsSize) :
        m_Name(name), m_SosGain(sosGain), m_SamplingHz(samplingHz)
    {
        if((polesSize / 2) == (coeffsSize / 5)) {
            m_SosPoles = QVector<complex>(polesSize);
            for(int i = 0; i < polesSize; i++) {
                m_SosPoles[i] = sosPoles[i];
            }

            m_SosCoefficients = QVector<double>(coeffsSize);
            for(int i = 0; i < coeffsSize; i++) {
                m_SosCoefficients[i] = sosCoeffs[i];
            }
        }
    }

    QString name() const
    {
        return m_Name;
    }

    bool valid() const
    {
        return (((m_SosCoefficients.size() / 5) == (m_SosPoles.size() * 2))
                && m_SosPoles.size() && m_SosCoefficients.size());
    }

    double gain() const
    {
        return m_SosGain;
    }

    QVector<complex> poles() const
    {
        return m_SosPoles;
    }

    QVector<double> coefficients() const
    {
        return m_SosCoefficients;
    }

    int samplingFreqHz() const
    {
        return m_SamplingHz;
    }

private:
    QString m_Name;
    double m_SosGain;
    QVector<complex> m_SosPoles;
    QVector<double> m_SosCoefficients;
    int m_SamplingHz;
};

const QVector<ButterCoefficients>& predefinedButterCoefficientSets();

// Second-order (biquadratic) IIR filtering
QVector<double> sosfilt(const QVector<double>& sosMatrix,
                        double gain,
                        const QVector<double>& x);

// Second-order (biquadratic) IIR Zero-phase digital filtering
QVector<double> sosfiltfilt(const QVector<double>& sosMatrix,
                            float gain,
                            const QVector<complex>& poles,
                            const QVector<double>& x);

QVector<double> processButter(const QVector<double>& signal, const ButterCoefficients &coefficients);
