#include <QString>
#include <QtTest>
#include "../src/rrintervalmethod.h"
class UnitTestsTest : public QObject {
  Q_OBJECT

public:
  UnitTestsTest();

private Q_SLOTS:
  void countRRIntervalsOneInterval();
  void countRRIntervalsThreeIntervals();
  void classifyIntervalsTest();
};

UnitTestsTest::UnitTestsTest() {}

void UnitTestsTest::countRRIntervalsOneInterval() {
  vector<double> RRTime = { 0.1, 0.2 };
  RRIntervalMethod a;
  vector<double> intervals = a.countRRInvervals(RRTime);
  QCOMPARE(intervals.front(), 0.1);
  QCOMPARE(intervals.size(), RRTime.size() - 1);
}
void UnitTestsTest::countRRIntervalsThreeIntervals() {
  vector<double> RRTime = { 0.1, 0.2, 0.5, 0.7, 0.9 };
  vector<double> ExpIntervals = { 0.2 - 0.1, 0.5 - 0.2, 0.7 - 0.5, 0.9 - 0.7 };
  RRIntervalMethod a;
  vector<double> intervals = a.countRRInvervals(RRTime);
  QCOMPARE(intervals.size(), RRTime.size() - 1);
  QVERIFY(intervals == ExpIntervals);
}

void UnitTestsTest::classifyIntervalsTest() {
  vector<double> intervals = { 1, 1, 1.5, 0.5 };
  vector<classification> expectedIntervals = { Regular, Regular, Long, Short };
  RRIntervalMethod a;
  a.countAvarageInterval(intervals);
  vector<classification> classifiedIntervals = a.classifyIntervals(intervals);
  QVERIFY(classifiedIntervals == expectedIntervals);
}

QTEST_APPLESS_MAIN(UnitTestsTest)

#include "tst_unitteststest.moc"
