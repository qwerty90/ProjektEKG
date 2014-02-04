#ifndef VCG_T_LOOP_H
#define VCG_T_LOOP_H
#include <QVector>

class VCG_T_LOOP
{
  const QVector<double> d1;
  const QVector<double> d2;
  const QVector<double> d3;
  const QVector<double> d4;
  const QVector<double> d5;
  const QVector<double> d6;
  const QVector<double> I;
  const QVector<double> II;

  QVector<QVector<double>::const_iterator> QRSStarts;
  QVector<QVector<double>::const_iterator> TWaveStarts;
  QVector<QVector<double>::const_iterator> TWaveEnds;
  double Dower[3][8];
  QVector<double> X;
  QVector<double> Y;
  QVector<double> Z;
  QVector<double> MA;
  QVector<double> DEA;
  QVector<double> RMMV;
private:
  double MyAbs(double a);
public:
    VCG_T_LOOP(const QVector<double> &d1,
               const QVector<double> &d2,
               const QVector<double> &d3,
               const QVector<double> &d4,
               const QVector<double> &d5,
               const QVector<double> &d6,
               const QVector<double> &i,
               const QVector<double> &ii,
               const QVector<QVector<double>::const_iterator> &QRSStarts,
               const QVector<QVector<double>::const_iterator> &TWaveStarts,
               const QVector<QVector<double>::const_iterator> &TWaveEnds);
    void SynthesizeVCG();
    double CalculateMA(double qrsStart, double Ton, double Tend);
    double CalculateRMMV(double qrsStart, double Tend);
    double CalculateDEA(double Ton, double Tend);
	//Wektor kolejnych próbek VCG w płaszczyźnie X
    QVector<double> getX();
	//Wektor kolejnych próbek VCG w płaszczyźnie Y
    QVector<double> getY();
	//Wektor kolejnych próbek VCG w płaszczyźnie ZS
    QVector<double> getZ();
	//Wektor parametrów MA, każda kolejna wartość to wyliczony parametr dla kolejnego cyklu
    QVector<double> getMA();
	//Wektor parametrów RMMV, każda kolejna wartość to wyliczony parametr dla kolejnego cyklu
    QVector<double> getRMMV();
	//Wektor parametrów DEA, każda kolejna wartość to wyliczony parametr dla kolejnego cyklu
    QVector<double> getDEA();
	//Główna metoda uruchamiająca wszystkie obliczenia
    void Run();
};

#endif // VCG_T_LOOP_H


