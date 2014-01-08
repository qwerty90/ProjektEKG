#ifndef UTILS_H
#define UTILS_H

#include <assert.h>
#include <limits>
#include <numeric>
#include <random>
#include <functional>

#include <QStringList>
#include <QFile>
#include <QString>
#include <QTextStream>
#include <QVectorIterator>
#include <QDebug>

template<class Type>
class BaseLineUtils {
public:
    static Type getMaxValue(const QVector<Type>& vec, int start, int end) {
        Type valueMax = std::numeric_limits<Type>::min();
        for(typename QVector<Type>::const_iterator it = vec.begin()+start; it < vec.begin() + end; it++) {
            valueMax = std::max(valueMax, *it);
        }
        return valueMax;
    }

    static Type getMaxValue(const QVector<Type>& vec) {
        return BaseLineUtils<Type>::getMaxValue(vec, 0, vec.size());
    }

    static QVector<Type> abs(const QVector<Type>& vec) {
        QVector<Type> output(vec.size());

        for(int i = 0; i < output.size(); i++) {
            output[i] = std::abs(vec[i]);
        }

        return output;
    }

    static Type max(const QVector<Type>& vec) {
        return BaseLineUtils<Type>::getMaxValue(vec);
    }

    static QVector<Type> getFullCopy(const QVector<Type>& input) {
        QVector<Type> output(input.size(), 0);
        qCopy(input.begin(), input.end(), output.begin());

        return output;
    }

    static Type getMinValue(const QVector<Type>& vec, int start, int end) {
        Type valueMin = std::numeric_limits<Type>::max();
        for(typename QVector<Type>::const_iterator it = vec.begin() + start; it < vec.begin() + end; it++) {
            valueMin = std::min(valueMin, *it);
        }
        return valueMin;
    }

    static Type getMinValue(const QVector<Type>& vec) {
        return BaseLineUtils<Type>::getMinValue(vec, 0, vec.size());
    }

    static Type mean(const QVector<Type>& input) {
        return mean(input, 0, input.size());
    }

    static Type mean(const QVector<Type>& input, const QVector<int>& indexes) {
        Type sum = 0;
        Type output;

        for(int i = 0; i < indexes.size(); i++) {
            sum += input[indexes[i]];
        }

        output = sum / (double)indexes.size();

        return output;
    }

    static Type mean(const QVector<Type>& input, int startIndex, int endIndex) {
        Type sum;
        Type output;

        sum = std::accumulate(input.begin()+startIndex, input.begin() + endIndex, 0);
        output = sum / (double)(endIndex - startIndex);

        return output;
    }

    static Type sum(const QVector<Type>& input) {
        return mean(input) * input.size();
    }

    static QVector<Type> mul(const QVector<Type>& vec, Type value) {
        QVector<Type> output(vec.size(), 0);
        for(int i = 0; i < vec.size(); i++) {
            output[i] = vec[i] * value;
        }

        return output;
    }

    static Type dot(const QVector<Type>& inputA, const QVector<Type>& inputB, const QVector<int>& indexes) {
        Type output = 0;

        for(int i = 0; i < indexes.size(); i++) {
            output += inputA[indexes[i]] * inputB[indexes[i]];
        }

        return output;
    }

    static Type dot(const QVector<Type>& inputA, const QVector<Type>& inputB) {
        assert(inputA.size() == inputB.size());
        Type output = 0;

        for(int i = 0; i < inputA.size(); i++) {
            output += inputA[i] * inputB[i];
        }

        return output;
    }

    static Type sd(const QVector<Type>& input) {

        Type mean = BaseLineUtils<Type>::mean(input);
        Type sq_sum = BaseLineUtils<Type>::dot(input, input);
        Type stdDev = std::sqrt(sq_sum / (double)input.size() - mean*mean);

        return stdDev;
    }

    static Type sd(const QVector<Type>& input, const QVector<int>& indexes) {
        Type mean = BaseLineUtils<Type>::mean(input, indexes);
        Type sq_sum = BaseLineUtils<Type>::dot(input, input, indexes);
        Type stdDev = std::sqrt(sq_sum / (double)indexes.size() - mean*mean);

        return stdDev;
    }

    static QVector<Type> randn(int length) {
        QVector<Type> output(length, 0);
        std::default_random_engine generator;
        std::normal_distribution<Type> distribution(0.0, 1.0);
        auto f = std::bind(distribution, generator);

        for(int i = 0; i < output.size(); i++) {
            output[i] = f();
        }

        return output;
    }
    static QVector<Type> pow(const QVector<Type>& input, Type power) {
        QVector<Type> output = BaseLineUtils<Type>::getFullCopy(input);

        for(int i = 0; i < output.size(); i++ ){
            output[i] = ::pow(input[i], power);
        }

        return output;
    }

    static QVector<int> find(const QVector<Type>& input) {
        assert(input.size() > 0);
        QVector<int> output;

        auto predicate = [=](Type value){return (value > 0);};
        output = BaseLineUtils<Type>::find(input, predicate);

        return output;
    }

    static QVector<int> find(const QVector<Type>& input, std::function<bool(Type)> predicate) {
        QVector<int> output;

        for(int i = 0; i < input.size(); i++) {
            if(predicate(input[i])) output.push_back(i);
        }

        return output;
    }

    static QVector<int> diff(const QVector<int>& input) {
        QVector<int> output;

        for(int i = 1; i < input.size(); i++) {
            output.append(input[i] - input[i-1]);
        }

        return output;
    }

    static QVector<double> diffD(const QVector<int>& input) {
        QVector<double> output;

        for(int i = 1; i < input.size(); i++) {
            output.append((double)input[i] - (double)input[i-1]);
        }

        return output;
    }

    static QVector<Type> linspace(int nSteps, Type startValue, Type endValue ){
        QVector<Type> output(nSteps, 0);

        Type deltaV = (endValue - startValue)/(double)(nSteps - 1);
        for(int i = 0; i < output.size(); i++) {
            output[i] = startValue + i * deltaV;
        }

        return output;
    }

    static void linspace(QVector<double>& input, int startIndex, int endIndex, double startValue, double endValue ){

        double deltaV = (endValue - startValue)/(endIndex - startIndex - 1);
        for(int i = 0; i < (endIndex - startIndex); i++) {
            input[startIndex + i] = startValue + i * deltaV;
        }
    }

    /*unused argument to overload linspace (would be identical to the previous one) + adhereing to matlab syntax*/
    static void linspace(QVector<Type>& input, int startIndex, int endIndex, Type step, Type startValue, Type /*endValue*/){

        for(int i = 0; i < (endIndex - startIndex); i++) {
            input[startIndex + i] = startValue + i * step;
        }
    }

    static void modulo(QVector<Type>& input, Type value) {
        for(int i = 0; i < input.size(); i++){
            input[i] = fmod(input[i], value);
        }
    }

    static void add(QVector<Type>& input, Type value, int startIndex, int endIndex) {
        for(int i =0; i < endIndex - startIndex; i++) {
            input[startIndex + i] += value;
        }
    }

    static void add(QVector<Type>& input, Type value) {
        add(input, value, 0, input.size());
    }

    static QVector<Type> readFromFile(const QString &fileName) {
        QVector<Type> output;

        QFile csvFile(fileName);
        csvFile.open(QFile::ReadOnly);
        QTextStream in(&csvFile);
        while(!in.atEnd()) {
            QString line = in.readLine();
            QStringList strList = line.split(QRegExp("\\s"), QString::SkipEmptyParts);
            foreach(QString str, strList) {
                output.append(str.toDouble());
            }
        }

        return output;
    }

    static void writeToFile(const QString &fileName, const QVector<Type> &output) {
        QFile csvFile(fileName);
        csvFile.open(QFile::WriteOnly);
        QTextStream out(&csvFile);
        foreach(const Type val, output) {
            out << val << "\n";
        }
    }
};

#endif // UTILS_H

