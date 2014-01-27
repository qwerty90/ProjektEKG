#include <QString>
#include <QtTest>
#include <QFile>
#include <QDebug>
#include <cstdlib>

//------------------------------------------------------------

#include "../ecgstanalyzer.h"
#include "ecgstdata.h"

//------------------------------------------------------------

namespace TestConsts
{
const int INDEX_TOLERANCE = 3;
}

//------------------------------------------------------------

class TestEcgStInterval : public QObject
{
    Q_OBJECT

public:
    TestEcgStInterval();

private Q_SLOTS:
    void testDataB100();
    void testDataB103();
    void testDataB105();
    void testDataB112();

    void testNoSamples();
    void testIncompleteJData();
    void testIncompleteTEndData();
    void testIncorrectJPosition();

private:
    EcgStData data;
    EcgStAnalyzer analyzer;

    void runTest(const QString &fileName, int num,
                 int stOn[], int stEnd[],
                 EcgStPosition pos[], EcgStShape shape[],
                 int remJ = -1, int remT = -1);

    bool loadTestData(const QString &fileName);

    void parseSamples(QVector<double> *dst, const QString &line,
                      char separator, double scale = 1.0,
                      int length = 10000);

    void parseIdx(QVector<QVector<double>::const_iterator> *dst,
                  const QString &line, char separator,
                  int length = 10000);
};

//------------------------------------------------------------

TestEcgStInterval::TestEcgStInterval()
{
    analyzer.setSmoothSize(4);
    analyzer.setDetectionSize(30);
    analyzer.setMorphologyCoeff(6.0);
    analyzer.setAlgorithm(EcgStAnalyzer::LINEAR);
    analyzer.setLevelThreshold(0.15);
    analyzer.setSlopeThreshold(35.0);
}

//------------------------------------------------------------

void TestEcgStInterval::testDataB100()
{
    int stOn[] = { 95, 387, 677, 963, 1244 };
    int stEnd[] = { 184, 445, 761, 1047, 1308 };
    EcgStPosition pos[] = {
        ST_POS_DEPRESSION,
        ST_POS_NORMAL,
        ST_POS_NORMAL,
        ST_POS_NORMAL,
        ST_POS_NORMAL
    };
    EcgStShape shape[] = {
        ST_SHAPE_HORIZONTAL,
        ST_SHAPE_HORIZONTAL,
        ST_SHAPE_HORIZONTAL,
        ST_SHAPE_HORIZONTAL,
        ST_SHAPE_HORIZONTAL
    };

    runTest("data/b100.m", 5, stOn, stEnd, pos, shape);
}

//------------------------------------------------------------

void TestEcgStInterval::testDataB103()
{
    int stOn[] = { 281, 590, 890, 1194 };
    int stEnd[] = { 338, 639, 940, 1246 };
    EcgStPosition pos[] = {
        ST_POS_DEPRESSION,
        ST_POS_DEPRESSION,
        ST_POS_DEPRESSION,
        ST_POS_DEPRESSION,
        ST_POS_DEPRESSION
    };
    EcgStShape shape[] = {
        ST_SHAPE_CONCAVE,
        ST_SHAPE_CONCAVE,
        ST_SHAPE_CONCAVE,
        ST_SHAPE_CONCAVE,
        ST_SHAPE_CONCAVE
    };

    runTest("data/b103.m", 4, stOn, stEnd, pos, shape);
}

//------------------------------------------------------------

void TestEcgStInterval::testDataB105()
{
    int stOn[] = { 221, 488, 740, 995, 1251 };
    int stEnd[] = { 273, 532, 784, 1040, 1294 };
    EcgStPosition pos[] = {
        ST_POS_DEPRESSION,
        ST_POS_DEPRESSION,
        ST_POS_DEPRESSION,
        ST_POS_DEPRESSION,
        ST_POS_DEPRESSION
    };
    EcgStShape shape[] = {
        ST_SHAPE_UPSLOPING,
        ST_SHAPE_UPSLOPING,
        ST_SHAPE_UPSLOPING,
        ST_SHAPE_UPSLOPING,
        ST_SHAPE_UPSLOPING
    };

    runTest("data/b105.m", 5, stOn, stEnd, pos, shape);
}

//------------------------------------------------------------

void TestEcgStInterval::testDataB112()
{
    int stOn[] = { 149, 410, 668, 917, 1172 };
    int stEnd[] = { 225, 482, 740, 954, 1205 };
    EcgStPosition pos[] = {
        ST_POS_DEPRESSION,
        ST_POS_DEPRESSION,
        ST_POS_DEPRESSION,
        ST_POS_DEPRESSION,
        ST_POS_DEPRESSION
    };
    EcgStShape shape[] = {
        ST_SHAPE_DOWNSLOPING,
        ST_SHAPE_DOWNSLOPING,
        ST_SHAPE_DOWNSLOPING,
        ST_SHAPE_DOWNSLOPING,
        ST_SHAPE_DOWNSLOPING
    };

    runTest("data/b112.m", 5, stOn, stEnd, pos, shape);
}

//------------------------------------------------------------

void TestEcgStInterval::testNoSamples()
{
    QCOMPARE(analyzer.analyze(QVector<double>(),
                              QVector<EcgSampleIter>(),
                              QVector<EcgSampleIter>(),
                              QVector<EcgSampleIter>(), 360.0), false);

    QCOMPARE(analyzer.getLastError(), EcgStAnalyzer::NO_SAMPLES_PROVIDED);
}

//------------------------------------------------------------

void TestEcgStInterval::testIncompleteJData()
{
    int stOn[] = { 95, 387, 677, 1244 };
    int stEnd[] = { 184, 445, 761, 1308 };
    EcgStPosition pos[] = {
        ST_POS_DEPRESSION,
        ST_POS_NORMAL,
        ST_POS_NORMAL,
        ST_POS_NORMAL
    };
    EcgStShape shape[] = {
        ST_SHAPE_HORIZONTAL,
        ST_SHAPE_HORIZONTAL,
        ST_SHAPE_HORIZONTAL,
        ST_SHAPE_HORIZONTAL
    };

    runTest("data/b100.m", 4, stOn, stEnd, pos, shape, 3);
}

//------------------------------------------------------------

void TestEcgStInterval::testIncompleteTEndData()
{
    int stOn[] = { 95, 677, 963, 1244 };
    int stEnd[] = { 184, 761, 1047, 1308 };
    EcgStPosition pos[] = {
        ST_POS_DEPRESSION,
        ST_POS_NORMAL,
        ST_POS_NORMAL,
        ST_POS_NORMAL
    };
    EcgStShape shape[] = {
        ST_SHAPE_HORIZONTAL,
        ST_SHAPE_HORIZONTAL,
        ST_SHAPE_HORIZONTAL,
        ST_SHAPE_HORIZONTAL
    };

    runTest("data/b100.m", 4, stOn, stEnd, pos, shape, -1, 1);
}

//------------------------------------------------------------

void TestEcgStInterval::testIncorrectJPosition()
{
    int stOn[] = { 95, 387, 963, 1244 };
    int stEnd[] = { 184, 445, 1047, 1308 };
    EcgStPosition pos[] = {
        ST_POS_DEPRESSION,
        ST_POS_NORMAL,
        ST_POS_NORMAL,
        ST_POS_NORMAL
    };
    EcgStShape shape[] = {
        ST_SHAPE_HORIZONTAL,
        ST_SHAPE_HORIZONTAL,
        ST_SHAPE_HORIZONTAL,
        ST_SHAPE_HORIZONTAL
    };

    runTest("data/b100_err.m", 4, stOn, stEnd, pos, shape);
}

//------------------------------------------------------------

void TestEcgStInterval::runTest(const QString &fileName, int num,
                                int stOn[], int stEnd[],
                                EcgStPosition pos[], EcgStShape shape[],
                                int remJ, int remT)
{
    QVERIFY2(loadTestData(fileName), "Failed to load test data");

    if (remJ > 0)
        data.jData.remove(remJ);
    if (remT > 0)
        data.rData.remove(remT);

    QVERIFY(analyzer.analyze(data.ecgSamples, data.rData, data.jData, data.tEndData, 360.0));
    QCOMPARE(analyzer.getLastError(), EcgStAnalyzer::NO_ERROR);

    QList<EcgStDescriptor> res = analyzer.getResult();
    QCOMPARE(res.size(), num);

    for (int i = 0; i < num; i++)
    {
        QVector<double>::const_iterator stOnIter = data.ecgSamples.begin() + (stOn[i] - 1);
        QVector<double>::const_iterator stEndIter = data.ecgSamples.begin() + (stEnd[i] - 1);

        QCOMPARE(res[i].position, pos[i]);
        QCOMPARE(res[i].shape, shape[i]);
        QVERIFY(abs(stOnIter - res[i].STOn) < TestConsts::INDEX_TOLERANCE);
        QVERIFY(abs(stEndIter - res[i].STEnd) < TestConsts::INDEX_TOLERANCE);
    }
}

//------------------------------------------------------------

bool TestEcgStInterval::loadTestData(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QTextStream in(&file);

    data.ecgSamples.clear();
    data.rData.clear();
    data.jData.clear();
    data.tEndData.clear();

    parseSamples(&data.ecgSamples, in.readLine(), ';', 0.01, 1500);
    parseIdx(&data.rData, in.readLine(), ' ');
    parseIdx(&data.jData, in.readLine(), ' ');
    parseIdx(&data.tEndData, in.readLine(), ' ');

    file.close();

    return true;
}

//------------------------------------------------------------

void TestEcgStInterval::parseSamples(QVector<double> *dst,
                                     const QString &line, char separator,
                                     double scale, int length)
{
    QString str = line;
    int pos = str.indexOf('[');
    str = str.mid(pos + 1).trimmed();
    str = str.left(str.length() - 2);

    QStringList strList = str.split(separator);
    int i = 0;
    foreach (QString item, strList)
    {
        dst->push_back(item.toDouble() * scale);

        i++;
        if (i >= length)
            break;
    }
}

//------------------------------------------------------------

void TestEcgStInterval::parseIdx(QVector<QVector<double>::const_iterator> *dst,
                                 const QString &line, char separator,
                                 int length)
{
    QString str = line;
    int pos = str.indexOf('[');
    str = str.mid(pos + 1).trimmed();
    str = str.left(str.length() - 2);

    QStringList strList = str.split(separator);
    int i = 0;
    foreach (QString item, strList)
    {
//        dst->push_back(item.toInt() - 1);
        int idx = item.toInt() - 1;
        dst->push_back(data.ecgSamples.begin() + idx);

        i++;
        if (i >= length)
            break;
    }
}

//------------------------------------------------------------

QTEST_APPLESS_MAIN(TestEcgStInterval)

//------------------------------------------------------------

#include "tst_testecgstinterval.moc"
