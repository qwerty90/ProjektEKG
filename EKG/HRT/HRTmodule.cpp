#include "HRTmodule.h"


using namespace HRT;


void HRTmodule::calculateHRT(QVector<unsigned int> Rpeaks, int samplingFrequency)
//void HRTmodule::calculateHRT(vector<unsigned int> Rpeaks, int samplingFrequency)
{
	//////init
	n_R=Rpeaks.size();
	frequency=(double)samplingFrequency;
	for (int i=0;i<n_R;i++)
		this->R_peaks.push_back((int)(Rpeaks[i]*1000.0)/frequency);//transform rpeaks

	/////init

	VEBlist=findVEB();
	if(VEBlist.size()>0)
	{
		VEBlist2=filterVEB();
		if(VEBlist2.size()>0)
		{
			RRlist=createRRlist();

			HRT_RR = filterRR();
			if(HRT_RR.size()>0)
			{
				GlobalAverage();
				tacho=tachogram();
				totalVEBcount=HRT_RR.size();
			}
			else
				totalVEBcount=0;
		}
		else 
			totalVEBcount=0;
	}
	else
		totalVEBcount=0;
	
}


//vector<double> 	HRTmodule::get_tachogram()
QVector<double> 	HRTmodule::get_tachogram()
{

	return tacho;
}



int 			HRTmodule::get_VEBcount()
{
	return totalVEBcount;
}
double 			HRTmodule::get_TS()
{
	return TS;
}
double			HRTmodule::get_TO()
{
	return TO;
}
double			HRTmodule::get_a()
{
	return TS;
}
double			HRTmodule::get_b()
{
	return TS_B;
}


vector<double> HRTmodule::findVEB()  
{
	vector<double> VEBlistT;
	double RR1=R_peaks[1]-R_peaks[0];
	double RR2=R_peaks[2]-R_peaks[1];
	double RR3=R_peaks[3]-R_peaks[2];
	double RR4=R_peaks[4]-R_peaks[3];
	double RR5=R_peaks[5]-R_peaks[4];
	double RR_v1, RR_v2, RR_av;
	for (int i=n_R_l; i<n_R-n_R_u; ++i) 
	{
		RR_v1=R_peaks[i]-R_peaks[i-1]; 
		RR_v2=R_peaks[i+1]-R_peaks[i]; 
		if( (RR_v1>200) && (RR_v2>200))
		{

		RR_av=(RR1+RR2+RR3+RR4+RR5)/5;
		
		if( ((RR_av - RR_v1) / RR_av > 0.2) && ((RR_v2 - RR_av) / RR_av > 0.2)  ) 
		VEBlistT.push_back(i);

		RR1=RR2;
		RR2=RR3;
		RR3=RR4;
		RR4=RR5;
		RR5=RR_v1;
		}
	}
	return VEBlistT;
}

vector<double> HRTmodule::filterVEB()
{
	int n=VEBlist.size();
	vector<double> TempList;
	if (n > 2)
	{
	
		if( (VEBlist[1] - VEBlist[0]) > Rdist )
			TempList.push_back(VEBlist[0]);
    
		for (int i=1; i<n-1; ++i)
		{
			if( ( (VEBlist[i+1] - VEBlist[i]) > Rdist ) && ((VEBlist[i] - VEBlist[i-1]) > Rdist ) )
				TempList.push_back((int)VEBlist[i]);
		}
    
		if( (VEBlist[VEBlist.size()-1] - VEBlist[VEBlist.size()-2]) > Rdist )
			TempList.push_back(VEBlist.back());
	}
	else
		if(n==1)
			TempList=VEBlist;
		else
			if((VEBlist[1] -VEBlist[0]) > Rdist )
				TempList=VEBlist;

	return TempList;
}

vector<vector<double>> HRTmodule::createRRlist()
{
	vector<vector<double>> RRlistT;
	vector<double> temp;
	int n=VEBlist2.size();
	
	for (int i=0; i<n; ++i)
	{
		temp.push_back(VEBlist2[i]);
		RRlistT.push_back(temp);
		vector<double>().swap(temp);
	}
	
	for (int j=0; j<n; ++j)
	{
		for (int i=0; i<25; ++i)
		{
			RRlistT[j].push_back((R_peaks[(int)VEBlist2[j] - 3 + i] - R_peaks[(int)VEBlist2[j] - 4 + i]));
		}
	}
	return RRlistT;
}

vector<vector<double>> HRTmodule::filterRR()
{
	bool flag=false;
	vector<vector<double>> HRT_RR;
	int columns = RRlist[0].size();
	int rows = RRlist.size();
	for (int i=0; i<rows; ++i)
	{
		for (int j=6; j<columns-1; ++j)
		{
			if( RRlist[i][j]<300 || RRlist[i][j]>2000 || (RRlist[i][j+1] - RRlist[i][j])>200 )
			flag=true;
		}
		if ( flag != true )
		{
			HRT_RR.push_back(RRlist[i]);
		}

		flag=false;
	}
	return HRT_RR;
}

void HRTmodule::GlobalAverage()
{
	int n = HRT_RR.size();
	int m = HRT_RR[0].size();
	double TO_temp=0;
	double RR1, RR2, RR_1, RR_2;
	double sum=0;
	double TS_T;
	double B;		
	vector<double> TO_T;
	vector<double> temporaryVector;
	vector<vector<double>> temporaryMatrix;
	vector<double> TS_a;
	vector<double> TS_b;
	vector<vector<double>> XY;
	vector<double> y;
	vector<double> TO_TS_slope;
	vector<double> xytemp(20);

	XY.push_back(xytemp); 
	XY.push_back(xytemp); 

	for (int i=0; i<n; ++i)
	{
		RR1=HRT_RR[i][2];
		RR2=HRT_RR[i][3];
		RR_1=HRT_RR[i][6];
		RR_2=HRT_RR[i][7];
		TO_T.push_back(((RR1+RR2)-(RR_1+RR_2))/(RR_1+RR_2) * 100.0);
	}
	
	for(int i=0;i<n;i++)
		TO_temp=TO_temp+TO_T[i];
	TO_temp = TO_temp/(double)n;
	
	if (n>1)
	{
		for (int i=6; i<26; ++i)
		{
			for (int j=0; j<n; j++)
			{
			sum=sum+HRT_RR[j][i];
			}
			XY[1][i-6]=sum/(double)n;
			XY[0][i-6]=i;
			sum=0;
		}
	}
	else
	{
		for (int i=6; i<26; ++i)
		{
			XY[1][i-6]=HRT_RR[0][i];
			XY[0][i-6]=i;
		}
	}

	for (int i=0; i<16; ++i)
	{
		TS_a.push_back(( 5.0*(XY[0][i]*XY[1][i]+XY[0][i+1]*XY[1][i+1]+XY[0][i+2]*XY[1][i+2]+XY[0][i+3]*XY[1][i+3]+XY[0][i+4]*XY[1][i+4]) - 
			(XY[0][i]+XY[0][i+1]+XY[0][i+2]+XY[0][i+3]+XY[0][i+4])*(XY[1][i]+XY[1][i+1]+XY[1][i+2]+XY[1][i+3]+XY[1][i+4]) )/
            (5.0*(pow(XY[0][i],2)+pow(XY[0][i+1],2)+pow(XY[0][i+2],2)+pow(XY[0][i+3],2)+pow(XY[0][i+4],2)) 
			- pow(XY[0][i]+XY[0][i+1]+XY[0][i+2]+XY[0][i+3]+XY[0][i+4],2)));
		TS_b.push_back(1.0/5.0*((XY[1][i]+XY[1][i+1]+XY[1][i+2]+XY[1][i+3]+XY[1][i+4]) - TS_a[i]*(XY[0][i]+XY[0][i+1]+XY[0][i+2]+XY[0][i+3]+XY[0][i+4])));
	}

	auto Biggest = max_element(begin(TS_a),end(TS_a));
	TS_T=*Biggest;
	B=TS_b[distance(begin(TS_a),Biggest)];

	this->TO=TO_temp;	
	this->TS=TS_T;	
	this->TS_B=B;
} 

Qvector<double> HRTmodule::tachogram()
{
	int i,j;
	int n = HRT_RR.size();
	int m = HRT_RR[0].size();
	double av=0;
	//vector<double> Tacho;
	QVector<double> Tacho;

	for (j=1;j<m;j++)
	{
		for (i=0;i<n;i++)
		{
			av=av+HRT_RR[i][j]/n;
		}
		Tacho.push_back(av);
		av=0;
	}

	return Tacho;
}
