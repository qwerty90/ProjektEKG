#include <QString>
#include <QtTest>

#include <cmath>
#include <numeric>
#include <iostream>
#include "../src/RRIntervals.h"
#include "../src/AtrialFibrApi.h"
#include "../src/PWave.h"
#include <stdlib.h>
#include <QVector>

using namespace std;
using namespace Ecg::AtrialFibr;

class RRSanityTest : public QObject {
  Q_OBJECT
  RRIntervalMethod rrmethod;

public:
  RRSanityTest();

private
Q_SLOTS:
  void initTestCase();
  void countRRIntervalsOneInterval();
  void countRRIntervalsThreeIntervals();
  void classifyIntervalsTest();
  void countTransitionsTest();
  void normalizeMarkovTableTest();
  void RRRunTest();
  void entropyBig();
  void entropySmall();
  void KLDivergenceEqualMatrix();
  void KLDivergenceTest();
  void JKDivergenceEqualMatrix();
  void JKDivergenceTest();

  void GetEntropyTest();
  void GetDivergenceTest();

  void correlation_ObviousCases();
  void pWaveOccurence_AllFound();
  void pWaveOccurence_HalfFound();
  void pWaveOccurence_ThrowIfPWaveStartTooCloseToEndOfSignal();
  void GetPWaveOccurenceRatioTest();
};

RRSanityTest::RRSanityTest() {}

void RRSanityTest::initTestCase() {}

void RRSanityTest::countRRIntervalsOneInterval() {
  // Arrange
  QVector<double> RRTime = { 0.1, 0.2 };

  // Act
  QVector<double> intervals = rrmethod.countRRInvervals(RRTime);

  // Assert
  QCOMPARE(intervals.front(), 0.1);
  QCOMPARE(intervals.size(), RRTime.size() - 1);
}

void RRSanityTest::countRRIntervalsThreeIntervals() {
  // Arrange
  QVector<double> RRTime = { 0.1, 0.2, 0.5, 0.7, 0.9 };
  QVector<double> ExpIntervals = { 0.2 - 0.1, 0.5 - 0.2, 0.7 - 0.5, 0.9 - 0.7 };

  // Act
  QVector<double> intervals = rrmethod.countRRInvervals(RRTime);

  // Assert
  QCOMPARE(intervals.size(), RRTime.size() - 1);
  QCOMPARE(intervals, ExpIntervals);
}

void RRSanityTest::classifyIntervalsTest() {
  // Arrange
  QVector<double> intervals = { 1, 1, 1.5, 0.5 };
  QVector<classification> expectedIntervals = { Regular, Regular, Long, Short };

  // Act
  rrmethod.countAverageInterval(intervals);
  QVector<classification> classifiedIntervals =
      rrmethod.classifyIntervals(intervals);

  // Assert
  QVERIFY(classifiedIntervals == expectedIntervals);
}

void RRSanityTest::countTransitionsTest() {
  // Arrange
  QVector<double> intervals = { 1, 1, 1.5, 0.5 };
  std::array<std::array<double, 3>, 3> ExpectedArray = {
    { { { 0, 0, 0 } }, { { 0, 1, 1 } }, { { 1, 0, 0 } } }
  };

  // Act
  rrmethod.countAverageInterval(intervals);
  QVector<classification> classifiedIntervals =
      rrmethod.classifyIntervals(intervals);
  rrmethod.countTransitions(classifiedIntervals);

  // Assert
  QVERIFY(rrmethod.getMarkovTable() == ExpectedArray);
}

void RRSanityTest::normalizeMarkovTableTest() {
  // Arrange
  QVector<double> intervals = { 1, 1, 1, 1.5, 0.5 };
  std::array<std::array<double, 3>, 3> ExpectedArray = {
    { { { 0, 0, 0 } }, { { 0, 0.5, 0.25 } }, { { 0.25, 0, 0 } } }
  };

  // Act
  rrmethod.countAverageInterval(intervals);
  QVector<classification> classifiedIntervals =
      rrmethod.classifyIntervals(intervals);
  rrmethod.countTransitions(classifiedIntervals);
  rrmethod.normalizeMarkovTable();

  // Assert
  QVERIFY(rrmethod.getMarkovTable() == ExpectedArray);
}

void RRSanityTest::RRRunTest() {
  // Arrange
  QVector<double> signal;
  for (int i = 0; i < 100; i++)
    signal.push_back(i);

  QVector<CIterators> RRPeaksIterators;
  RRIntervalMethod a;
  for (QVector<double>::const_iterator iters = signal.begin();
       iters < signal.end(); iters += 10) {
    RRPeaksIterators.push_back(iters);
  }
  // Act
  a.RunRRMethod(RRPeaksIterators);
  Matrix3_3 markovTable = a.getMarkovTable();
  Matrix3_3 ExpectedArray = { { { { 0, 0, 0 } }, { { 0, 1, 0 } },
                                { { 0, 0, 0 } } } };
  // Assert
  QVERIFY(markovTable == ExpectedArray);
}
void RRSanityTest::entropyBig() {
  // Arrange
  std::array<std::array<double, 3>, 3> arr = { { { { 0.11, 0.11, 0.11 } },
                                                 { { 0.11, 0.11, 0.11 } },
                                                 { { 0.11, 0.11, 0.11 } } } };

  // Assert
  QVERIFY(abs(entropy(arr) - 1.0) < 0.1);
}

void RRSanityTest::entropySmall() {
  // Arrange
  std::array<std::array<double, 3>, 3> arr = {
    { { { 0.001, 0.001, 0.001 } }, { { 0.001, 1.000, 0.001 } },
      { { 0.001, 0.001, 0.001 } } }
  };

  // Assert
  QVERIFY(entropy(arr) < 0.1);
  QVERIFY(entropy(arr) > -0.1);
}

void RRSanityTest::KLDivergenceEqualMatrix() {
  // Arrange
  std::array<std::array<double, 3>, 3> pattern = {
    { { { 0.11, 0.11, 0.11 } }, { { 0.11, 0.11, 0.11 } },
      { { 0.11, 0.11, 0.11 } } }
  };
  std::array<std::array<double, 3>, 3> arr = pattern;
  // Assert
  QVERIFY(KLdivergence(arr, pattern) == 0);
}

void RRSanityTest::KLDivergenceTest() {
  std::array<std::array<double, 3>, 3> pattern = {
    { { { 0.11, 0.11, 0.11 } }, { { 0.11, 0.11, 0.11 } },
      { { 0.11, 0.11, 0.11 } } }
  };
  std::array<std::array<double, 3>, 3> arr = {
    { { { 0.001, 0.001, 0.001 } }, { { 0.001, 1.000, 0.001 } },
      { { 0.001, 0.001, 0.001 } } }
  };
  // Assert
  QVERIFY(KLdivergence(arr, pattern) > 3.8);
  QVERIFY(KLdivergence(arr, pattern) < 3.9);
}

void RRSanityTest::JKDivergenceEqualMatrix() {
  // Arrange
  std::array<std::array<double, 3>, 3> pattern = {
    { { { 0.11, 0.11, 0.11 } }, { { 0.11, 0.11, 0.11 } },
      { { 0.11, 0.11, 0.11 } } }
  };
  std::array<std::array<double, 3>, 3> arr = pattern;
  // Assert
  QVERIFY(JKdivergence(arr, pattern) == 0);
}

void RRSanityTest::JKDivergenceTest() {
  std::array<std::array<double, 3>, 3> pattern = {
    { { { 0.11, 0.11, 0.11 } }, { { 0.11, 0.11, 0.11 } },
      { { 0.11, 0.11, 0.11 } } }
  };
  std::array<std::array<double, 3>, 3> arr = {
    { { { 0.001, 0.001, 0.001 } }, { { 0.001, 1.000, 0.001 } },
      { { 0.001, 0.001, 0.001 } } }
  };
  // Assert
  QVERIFY(JKdivergence(arr, pattern) > 0.49);
  QVERIFY(JKdivergence(arr, pattern) < 0.5);
}

void RRSanityTest::GetEntropyTest() {
  // Arrange
  QVector<double> signal;
  for (int i = 0; i < 1000; i += rand() % 20) {
    signal.push_back(i);
  }
  QVector<CIterators> RRPeaksIterators;
  RRIntervalMethod a;
  for (QVector<double>::const_iterator iters = signal.begin();
       iters < signal.end(); iters++)
    RRPeaksIterators.push_back(iters);

  // Act
  a.RunRRMethod(RRPeaksIterators);
  AtrialFibrApi AtrFibrApi(signal, RRPeaksIterators, RRPeaksIterators);

  // Assert
  QVERIFY(AtrFibrApi.GetRRIntEntropy() == entropy(a.getMarkovTable()));
}

void RRSanityTest::GetDivergenceTest() {
  // Arrange
  QVector<double> signal;
  for (int i = 0; i < 1000; i += rand() % 25) {
    signal.push_back(i);
  }
  QVector<CIterators> RRPeaksIterators;
  RRIntervalMethod a;
  for (QVector<double>::const_iterator iters = signal.begin();
       iters < signal.end(); iters++)
    RRPeaksIterators.push_back(iters);

  Matrix3_3 patternMatrix = { { { { 0.005, 0.023, 0.06 } },
                                { { 0.007, 0.914, 0.013 } },
                                { { 0.019, 0.006, 0.003 } } } };

  // Act
  a.RunRRMethod(RRPeaksIterators);
  AtrialFibrApi AtrFibrApi(signal, RRPeaksIterators, RRPeaksIterators);

  // Assert
  QVERIFY(AtrFibrApi.GetRRIntDivergence() ==
          JKdivergence(a.getMarkovTable(), patternMatrix));
}

void RRSanityTest::correlation_ObviousCases() {
  // Assert
  QCOMPARE(correlation({ 1, 2, 3, 4, 5 }, { 2, 4, 6, 8, 10 }), 1.0);
  QCOMPARE(correlation({ 1, 2, 3, 4, 5 }, { 5, 4, 3, 2, 1 }), -1.0);
}

void RRSanityTest::pWaveOccurence_AllFound() {
  // Arrange
  QVector<double> signal(200);
  QVector<QVector<double>::iterator> pWaveStarts = { signal.begin() + 10,
                                                   signal.begin() + 70 };
  QVector<QVector<double>::const_iterator> pWaveStartsC = { signal.begin() + 10,
                                                          signal.begin() + 70 };
  for (auto it : pWaveStarts)
    copy(begin(averagePWave), end(averagePWave), it);
  // Assert
  QCOMPARE(pWaveOccurenceRatio(pWaveStartsC, end(signal)), 1.0);
}

void RRSanityTest::pWaveOccurence_HalfFound() {
  // Arrange
  QVector<double> signal(200);
  QVector<QVector<double>::const_iterator> pWaveStarts = { signal.begin() + 10,
                                                         signal.begin() + 70 };
  copy(begin(averagePWave), end(averagePWave), begin(signal) + 10);

  // Assert
  QCOMPARE(pWaveOccurenceRatio(pWaveStarts, end(signal)), 1.0 / 2);
}

void RRSanityTest::pWaveOccurence_ThrowIfPWaveStartTooCloseToEndOfSignal() {
  // Arrange
  QVector<double> signal(100);
  copy(begin(averagePWave), begin(averagePWave) + 10, begin(signal) + 90);
  QVector<QVector<double>::const_iterator> pWaveStarts = { signal.begin() + 90 };
  bool thrown = false;

  // Act
  try {
    pWaveOccurenceRatio(pWaveStarts, end(signal));
  }
  catch (PWaveStartTooCloseToEndOfSignal) {
    thrown = true;
  }

  // Assert
  QVERIFY(thrown);
}

void RRSanityTest::GetPWaveOccurenceRatioTest() {
  // Arrange
  QVector<double> signal(200);
  QVector<QVector<double>::iterator> pWaveStarts = { signal.begin() + 10,
                                                   signal.begin() + 70 };
  QVector<QVector<double>::const_iterator> pWaveStartsC = { signal.begin() + 10,
                                                          signal.begin() + 70 };
  for (auto it : pWaveStarts)
    copy(begin(averagePWave), end(averagePWave), it);
  AtrialFibrApi AtrFibrApi(signal, pWaveStartsC, pWaveStartsC);
  // Assert
  QCOMPARE(AtrFibrApi.GetPWaveOccurenceRatio(), 1.0);
}
QTEST_APPLESS_MAIN(RRSanityTest)

#include "AfTests.moc"
