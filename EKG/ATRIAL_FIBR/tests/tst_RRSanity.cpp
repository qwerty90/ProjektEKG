#include <QString>
#include <QtTest>

#include <cmath>
#include <numeric>

#include "../src/RRIntervals.h"
#include "../src/PWave.h"

using namespace std;
using namespace Ecg::AtrialFibr;

class RRSanityTest : public QObject {
  Q_OBJECT

public:
  RRSanityTest();

private Q_SLOTS:
  void countRRIntervalsOneInterval();
  void countRRIntervalsThreeIntervals();
  void classifyIntervalsTest();
  void countTransitionsTest();
  void entropyBig();
  void entropySmall();
  void KLDivergenceEqualMatrix();
  void KLDivergenceTest();
  void JKDivergenceEqualMatrix();
  void JKDivergenceTest();
  void normalizeMarkovTableTest();

  void correlation_ObviousCases();
  void pWaveOccurence_AllFound();
  void pWaveOccurence_HalfFound();
  void pWaveOccurence_ThrowIfPWaveStartTooCloseToEndOfSignal();
};

RRSanityTest::RRSanityTest() {}

void RRSanityTest::countRRIntervalsOneInterval() {
  // Arrange
  vector<double> RRTime = { 0.1, 0.2 };
  RRIntervalMethod a;

  // Act
  vector<double> intervals = a.countRRInvervals(RRTime);

  // Assert
  QCOMPARE(intervals.front(), 0.1);
  QCOMPARE(intervals.size(), RRTime.size() - 1);
}

void RRSanityTest::countRRIntervalsThreeIntervals() {
  // Arrange
  vector<double> RRTime = { 0.1, 0.2, 0.5, 0.7, 0.9 };
  vector<double> ExpIntervals = { 0.2 - 0.1, 0.5 - 0.2, 0.7 - 0.5, 0.9 - 0.7 };
  RRIntervalMethod a;

  // Act
  vector<double> intervals = a.countRRInvervals(RRTime);

  // Assert
  QCOMPARE(intervals.size(), RRTime.size() - 1);
  QCOMPARE(intervals, ExpIntervals);
}

void RRSanityTest::classifyIntervalsTest() {
  // Arrange
  vector<double> intervals = { 1, 1, 1.5, 0.5 };
  vector<classification> expectedIntervals = { Regular, Regular, Long, Short };
  RRIntervalMethod a;

  // Act
  a.countAverageInterval(intervals);
  vector<classification> classifiedIntervals = a.classifyIntervals(intervals);

  // Assert
  QVERIFY(classifiedIntervals == expectedIntervals);
}

void RRSanityTest::countTransitionsTest() {
  // Arrange
  vector<double> intervals = { 1, 1, 1.5, 0.5 };
  RRIntervalMethod a;
  std::array<std::array<double, 3>, 3> ExpectedArray = {
    { { { 0, 0, 0 } }, { { 0, 1, 1 } }, { { 1, 0, 0 } } }
  };

  // Act
  a.countAverageInterval(intervals);
  vector<classification> classifiedIntervals = a.classifyIntervals(intervals);
  a.countTransitions(classifiedIntervals);

  // Assert
  QVERIFY(a.getMarkovTable() == ExpectedArray);
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

void RRSanityTest::normalizeMarkovTableTest() {
  // Arrange
  vector<double> intervals = { 1, 1, 1, 1.5, 0.5 };
  RRIntervalMethod a;
  std::array<std::array<double, 3>, 3> ExpectedArray = {
    { { { 0, 0, 0 } }, { { 0, 0.5, 0.25 } }, { { 0.25, 0, 0 } } }
  };

  // Act
  a.countAverageInterval(intervals);
  vector<classification> classifiedIntervals = a.classifyIntervals(intervals);
  a.countTransitions(classifiedIntervals);
  a.normalizeMarkovTable();
  auto b = a.getMarkovTable();

  // Assert
  QVERIFY(b == ExpectedArray);
}

void RRSanityTest::correlation_ObviousCases() {
  // Assert
  QCOMPARE(correlation({ 1, 2, 3, 4, 5 }, { 2, 4, 6, 8, 10 }), 1.0);
  QCOMPARE(correlation({ 1, 2, 3, 4, 5 }, { 5, 4, 3, 2, 1 }), -1.0);
}

void RRSanityTest::pWaveOccurence_AllFound() {
  // Arrange
  vector<double> signal(200);
  vector<vector<double>::iterator> pWaveStarts = { signal.begin() + 10,
                                                   signal.begin() + 70 };
  vector<vector<double>::const_iterator> pWaveStartsC = { signal.begin() + 10,
                                                          signal.begin() + 70 };
  for (auto it : pWaveStarts)
    copy(begin(averagePWave), end(averagePWave), it);

  // Assert
  QCOMPARE(pWaveOccurenceRatio(pWaveStartsC, end(signal)), 1.0);
}

void RRSanityTest::pWaveOccurence_HalfFound() {
  // Arrange
  vector<double> signal(200);
  vector<vector<double>::const_iterator> pWaveStarts = { signal.begin() + 10,
                                                         signal.begin() + 70 };
  copy(begin(averagePWave), end(averagePWave), begin(signal) + 10);

  // Assert
  QCOMPARE(pWaveOccurenceRatio(pWaveStarts, end(signal)), 1.0 / 2);
}

void RRSanityTest::pWaveOccurence_ThrowIfPWaveStartTooCloseToEndOfSignal() {
  // Arrange
  vector<double> signal(100);
  copy(begin(averagePWave), begin(averagePWave) + 10, begin(signal) + 90);
  vector<vector<double>::const_iterator> pWaveStarts = { signal.begin() + 90 };
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

QTEST_APPLESS_MAIN(RRSanityTest)

#include "tst_RRSanity.moc"
