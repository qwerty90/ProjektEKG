#include <QString>
#include <QtTest>
#include "../rrintervalmethod.h"
class UnitTestsTest : public QObject
{
    Q_OBJECT

public:
    UnitTestsTest();

private Q_SLOTS:
    void countRRIntervalsOneInterval();
    void countRRIntervalsThreeIntervals();
    void classifyIntervalsTest();
};

UnitTestsTest::UnitTestsTest()
{
}

void UnitTestsTest::countRRIntervalsOneInterval(){
    vector<double> RRTime = {0.1,0.2};
    RRIntervalMethod a;
    vector<double> intervals = a.countRRInvervals(RRTime);
    QCOMPARE(*intervals.begin(),0.1);
    QCOMPARE(intervals.size(),RRTime.size()-1);
}
void UnitTestsTest::countRRIntervalsThreeIntervals(){
    vector<double> RRTime = {0.1,0.2,0.5,0.7,0.9};
    vector<double> ExpIntervals = {0.1,0.3,0.2,0.2};
    RRIntervalMethod a;
    vector<double> intervals = a.countRRInvervals(RRTime);
    QCOMPARE(intervals.size(),RRTime.size()-1);
    auto itExp = ExpIntervals.begin();
    for(auto it = intervals.begin(); it != intervals.end(); ++it,++itExp){
        QCOMPARE(*it,*itExp);
    }
}

void UnitTestsTest::classifyIntervalsTest()
{
    vector<double> intervals = {1,1,1.5,0.5};
    vector<classification>expectedIntervals = {Regular,Regular,Long,Short};
    RRIntervalMethod a;
    a.countAvarageInterval(intervals);
    vector<classification>classifiedIntervals = a.classifyIntervals(intervals);
    auto itExp = expectedIntervals.begin();
    for(auto it = classifiedIntervals.begin(); it != classifiedIntervals.end(); ++it,++itExp){
        QCOMPARE(*it,*itExp);
    }
}



QTEST_APPLESS_MAIN(UnitTestsTest)

#include "tst_unitteststest.moc"
