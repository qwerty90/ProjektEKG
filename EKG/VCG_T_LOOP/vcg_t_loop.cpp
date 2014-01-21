#include "vcg_t_loop.h"
#include "math.h"
#include <iostream>

#define PI 3.14159265

using namespace std;

VCG_T_LOOP::VCG_T_LOOP(const QVector<double> &d1,
                       const QVector<double> &d2,
                       const QVector<double> &d3,
                       const QVector<double> &d4,
                       const QVector<double> &d5,
                       const QVector<double> &d6,
                       const QVector<double> &i,
                       const QVector<double> &ii,
                       const QVector<QVector<double>::const_iterator> &QRSStarts,
                       const QVector<QVector<double>::const_iterator> &TWaveStarts,
                       const QVector<QVector<double>::const_iterator> &TWaveEnds)
                       :d1(d1),d2(d2),d3(d3),d4(d4),d5(d5),d6(d6),I(i),II(ii),
                        QRSStarts(QRSStarts),TWaveStarts(TWaveStarts), TWaveEnds(TWaveEnds),
                        Dower({{-0.172, -0.074,  0.122,  0.231, 0.239, 0.194,  0.156, -0.010 },
                               {0.057, -0.019, -0.106, -0.022, 0.041, 0.048, -0.227,  0.887 },
                               {-0.229, -0.310, -0.246, -0.063, 0.055, 0.108,  0.022,  0.102 }})
{
}

void VCG_T_LOOP::SynthesizeVCG()
{
   int signalSize=d1.size();
   X.reserve(signalSize);
   Y.reserve(signalSize);
   Z.reserve(signalSize);
   for(int i=0;i<signalSize;i++)
   {
       X.append(Dower[0][0]*d1[i]+Dower[0][1]*d2[i]+Dower[0][2]*d3[i]+Dower[0][3]*d4[i]+
               Dower[0][4]*d5[i]+Dower[0][5]*d6[i]+Dower[0][6]*I[i]+Dower[0][7]*II[i]);
       Y.append(Dower[1][0]*d1[i]+Dower[1][1]*d2[i]+Dower[1][2]*d3[i]+Dower[1][3]*d4[i]+
               Dower[1][4]*d5[i]+Dower[1][5]*d6[i]+Dower[1][6]*I[i]+Dower[1][7]*II[i]);
       Z.append(Dower[2][0]*d1[i]+Dower[2][1]*d2[i]+Dower[2][2]*d3[i]+Dower[2][3]*d4[i]+
               Dower[2][4]*d5[i]+Dower[2][5]*d6[i]+Dower[2][6]*I[i]+Dower[2][7]*II[i]);
   }
}

double VCG_T_LOOP::CalculateMA(double qrsStart, double Ton, double Tend)
{
    int qrsSize = Ton-qrsStart;
    int tSize = Tend-Ton;
    double* Xqrs = new double[qrsSize];
    double* Yqrs = new double[qrsSize];
    double* Zqrs = new double[qrsSize];
    double* Xt = new double[tSize];
    double* Yt = new double[tSize];
    double* Zt = new double[tSize];
    for(int i=qrsStart;i<Ton;i++)
    {
        Xqrs[i-(int)qrsStart]=X.at(i);
        Yqrs[i-(int)qrsStart]=Y.at(i);
        Zqrs[i-(int)qrsStart]=Z.at(i);
    }
    for(int i=Ton;i<Tend;i++)
    {
        Xt[i-(int)Ton]=X.at(i);
        Yt[i-(int)Ton]=Y.at(i);
        Zt[i-(int)Ton]=Z.at(i);
    }
    double** XYqrs = new double*[2];
    double** XZqrs= new double*[2];
    double** ZYqrs= new double*[2];
    double** XYt= new double*[2];
    double** XZt= new double*[2];
    double** ZYt= new double*[2];
    for(int i=0;i<2;i++)
    {
        XYqrs[i] = new double[qrsSize];
        XZqrs[i] = new double[qrsSize];
        ZYqrs[i] = new double[qrsSize];
        XYt[i] = new double[tSize];
        XZt[i] = new double[tSize];
        ZYt[i] = new double[tSize];
    }
    for(int i=0;i<qrsSize;i++)
    {
        XYqrs[0][i] = Xqrs[i];
        XYqrs[1][i] = Yqrs[i];
        XZqrs[0][i] = Xqrs[i];
        XZqrs[1][i] = Zqrs[i];
        ZYqrs[0][i] = Zqrs[i];
        ZYqrs[1][i] = Yqrs[i];
    }

    for(int i=0;i<tSize;i++)
    {
        XYt[0][i] = Xt[i];
        XYt[1][i] = Yt[i];
        XZt[0][i] = Xt[i];
        XZt[1][i] = Zt[i];
        ZYt[0][i] = Zt[i];
        ZYt[1][i] = Yt[i];
    }
    double* XYqrsNorm = new double[qrsSize];
    double* XZqrsNorm = new double[qrsSize];
    double* ZYqrsNorm = new double[qrsSize];
    double* XYtNorm = new double[tSize];
    double* XZtNorm = new double[tSize];
    double* ZYtNorm = new double[tSize];
    for(int i =0;i<qrsSize;i++)
    {
        XYqrsNorm[i]=sqrt(pow(XYqrs[0][i],2)+pow(XYqrs[1][i],2));
        XZqrsNorm[i]=sqrt(pow(XZqrs[0][i],2)+pow(XZqrs[1][i],2));
        ZYqrsNorm[i]=sqrt(pow(ZYqrs[0][i],2)+pow(ZYqrs[1][i],2));
        cout<<XYqrsNorm[i]<<endl;
    }
    for(int i =0;i<tSize;i++)
    {
        XYtNorm[i]=sqrt(pow(XYt[0][i],2)+pow(XYt[1][i],2));
        XZtNorm[i]=sqrt(pow(XZt[0][i],2)+pow(XZt[1][i],2));
        ZYtNorm[i]=sqrt(pow(ZYt[0][i],2)+pow(ZYt[1][i],2));
    }

    int XYqrsMaxElementIndex ;
    int XZqrsMaxElementIndex ;
    int ZYqrsMaxElementIndex ;
    double maxXY=0;
    double maxZY=0;
    double maxXZ=0;
    for(int i=0;i<qrsSize;i++)
    {
        if(XYqrsNorm[i]>maxXY)
        {
            maxXY=XYqrsNorm[i];
            XYqrsMaxElementIndex = i;
        }
        if(XZqrsNorm[i]>maxXZ)
        {
            maxXZ=XZqrsNorm[i];
            XZqrsMaxElementIndex = i;
        }
        if(ZYqrsNorm[i]>maxZY)
        {
            maxZY=ZYqrsNorm[i];
            ZYqrsMaxElementIndex = i;
        }
    }
    int XYtMaxElementIndex ;
    int XZtMaxElementIndex ;
    int ZYtMaxElementIndex ;
    maxXY=0;
    maxZY=0;
    maxXZ=0;
    for(int i=0;i<tSize;i++)
    {
        if(XYtNorm[i]>maxXY)
        {
            maxXY=XYtNorm[i];
            XYtMaxElementIndex = i;
        }
        if(XZtNorm[i]>maxXZ)
        {
            maxXZ=XZtNorm[i];
            XZtMaxElementIndex = i;
        }
        if(ZYtNorm[i]>maxZY)
        {
            maxZY=ZYtNorm[i];
            ZYtMaxElementIndex = i;
        }
    }
  double asd = XYqrsNorm[XYqrsMaxElementIndex];
    double cos[3];
    cos[0] = (XYqrs[0][XYqrsMaxElementIndex]*XYt[0][XYtMaxElementIndex]
            + XYqrs[1][XYqrsMaxElementIndex]*XYt[1][XYtMaxElementIndex])
            /(XYqrsNorm[XYqrsMaxElementIndex]*XYtNorm[XYtMaxElementIndex]);
    cos[1] = (XZqrs[0][XZqrsMaxElementIndex]*XZt[0][XZtMaxElementIndex]
            + XZqrs[1][XZqrsMaxElementIndex]*XZt[1][XZtMaxElementIndex])
            /(XZqrsNorm[XZqrsMaxElementIndex]*XZtNorm[XZtMaxElementIndex]);
    cos[2] = (ZYqrs[0][ZYqrsMaxElementIndex]*ZYt[0][ZYtMaxElementIndex]
            + ZYqrs[1][ZYqrsMaxElementIndex]*ZYt[1][ZYtMaxElementIndex])
            /(ZYqrsNorm[ZYqrsMaxElementIndex]*ZYtNorm[ZYtMaxElementIndex]);
    for(int i=0;i<3;i++)
    {
        cos[i]=acos(cos[i])*180/PI;
    }
    double maxElement =  *max_element(cos, cos+sizeof(cos)/sizeof(double));

    delete [] Xqrs;
    delete [] Zqrs;
    delete [] Yqrs;
    delete [] Yt;
    delete [] Zt;
    delete [] Xt;
    for(int i=0;i<2;i++)
    {
        delete[] XYqrs[i];
        delete[] XZqrs[i];
        delete[] ZYqrs[i];
        delete[] XYt[i] ;
        delete[] XZt[i];
        delete[] ZYt[i];
    }
    delete [] XYqrs;
    delete [] XZqrs;
    delete [] ZYqrs;
    delete [] XYt;
    delete [] XZt;
    delete [] ZYt;
    delete [] XYqrsNorm;
    delete [] XZqrsNorm;
    delete [] ZYqrsNorm;
    delete [] XYtNorm;
    delete [] XZtNorm;
    delete [] ZYtNorm;

    return maxElement;
}

double VCG_T_LOOP::CalculateRMMV(double qrsStart, double Tend)
{
    int tSize = Tend-qrsStart;
    double* Xt = new double[tSize];
    double* Yt = new double[tSize];
    double* Zt = new double[tSize];
    double* V = new double[tSize];
    double Vsum =0;
    for(int i=qrsStart;i<Tend;i++)
    {
        Xt[i-(int)qrsStart]=X.at(i);
        Yt[i-(int)qrsStart]=Y.at(i);
        Zt[i-(int)qrsStart]=Z.at(i);
    }
    for(int i=0;i<tSize;i++)
    {
        V[i]=sqrt(pow(Xt[i],2)+pow(Yt[i],2)+pow(Zt[i],2));
        Vsum=Vsum+V[i];
        cout<<V[i]<<endl;
    }
    Vsum=Vsum/tSize;
    double Vmax=0;
    for(int i=0;i<tSize;i++)
    {
        if(V[i]>Vmax)
        {
            Vmax=V[i];
        }
    }
    delete [] Xt;
    delete [] Yt;
    delete [] Zt;
    delete [] V;
    return Vmax/Vsum;
}
double VCG_T_LOOP::CalculateDEA(double Ton, double Tend)
{
    int tSize = Tend-Ton;
    double* Xt = new double[tSize];
    double* Yt = new double[tSize];
    double* Zt = new double[tSize];
    double* VNorm = new double[tSize];
    double* alfa = new double[tSize];
    double* beta = new double[tSize];
    for(int i=Ton;i<Tend;i++)
    {
        Xt[i-(int)Ton]=X.at(i);
        Yt[i-(int)Ton]=Y.at(i);
        Zt[i-(int)Ton]=Z.at(i);
    }
    for(int i=0;i<tSize;i++)
    {
        VNorm[i]=sqrt(pow(Xt[i],2)+pow(Yt[i],2)+pow(Zt[i],2));
    }
    double sum=0;
    for(int i=0;i<tSize;i++)
    {
        double cosAlfa = Xt[i]/VNorm[i];
        double cosBeta = Zt[i]/VNorm[i];
        alfa[i] = acos(cosAlfa)*180/PI;
        beta[i] = acos(cosBeta)*180/PI;
        sum=sum+MyAbs(alfa[i]-beta[i]);
    }
    delete [] Xt;
    delete [] Yt;
    delete [] Zt;
    delete [] VNorm;
    delete [] alfa;
    delete [] beta;
    return sum/tSize;

}

double VCG_T_LOOP::MyAbs(double a)
{
    if(a>0)
        return a;
    else
        return-a;
}

void VCG_T_LOOP::Run()
{
    SynthesizeVCG();
    for(int i=0;i<QRSStarts.size();i++)
    {
       MA.append(CalculateMA(distance(d1.begin(),QRSStarts.at(i)),distance(d1.begin(),TWaveStarts.at(i)),distance(d1.begin(),TWaveEnds.at(i))));
       RMMV.append(CalculateRMMV(distance(d1.begin(),QRSStarts.at(i)),distance(d1.begin(),TWaveEnds.at(i))));
       DEA.append(CalculateDEA(distance(d1.begin(),TWaveStarts.at(i)),distance(d1.begin(),TWaveEnds.at(i))));
    }
}
QVector<double> VCG_T_LOOP::getX(){
    return X;
}
QVector<double> VCG_T_LOOP::getY(){
    return Y;
}
QVector<double> VCG_T_LOOP::getZ(){
    return Z;
}
QVector<double> VCG_T_LOOP::getMA()
{
    return MA;
}
QVector<double> VCG_T_LOOP::getRMMV()
{
    return RMMV;
}
QVector<double> VCG_T_LOOP::getDEA()
{
    return DEA;
}
