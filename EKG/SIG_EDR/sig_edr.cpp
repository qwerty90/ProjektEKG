#include "sig_edr.h"
//konstruktor przypisujacy otrzymane sygnaly EKG z dwoch elektrod
//do odpowiednich pol w obiekcie
sig_edr::sig_edr(const QVector<double> &signal_one,
                 const QVector<double> &signal_two
                ): signal_one(signal_one), signal_two(signal_two)
{}

//konstruktor przypisujacy otrzymane sygnaly EKG z dwoch elektrod
//i iteratory R-pikow do odpowiednich pol w obiekcie
sig_edr::sig_edr(const QVector<double> &signal_one,
                 const QVector<double>::const_iterator &RPeaksIterators_one,
                 const QVector<double> &signal_two,
                 const QVector<double>::const_iterator &RPeaksIterators_two
                 ): signal_one(signal_one), signal_two(signal_two)
{
    QVector<double>::const_iterator iterator = signal_one.begin();
    QVector<double>::const_iterator RPeaks   = RPeaksIterators_one;

    EDRsignal_RPeaks_one.clear();

    while(iterator != signal_one.end())
    {
        if(iterator == RPeaks)
        {
            EDRsignal_RPeaks_one << *iterator;
            RPeaks++;
        }
        else
            iterator++;
    }

    iterator = signal_two.begin();
    RPeaks   = RPeaksIterators_two;

    EDRsignal_RPeaks_two.clear();

    while(iterator != signal_two.end())
    {
        if(iterator == RPeaks)
        {
            EDRsignal_RPeaks_two << *iterator;
            RPeaks++;
        }
        else
            iterator++;
    }
}

//konstruktor przypisujacy otrzymane sygnaly EKG z dwoch elektrod,
//iteratory QRSonset i QRSend do odpowiednich pol w obiekcie
sig_edr::sig_edr(const QVector<double> &signal_one,
                 const QVector<double>::const_iterator &QRSonsetIterators_one,
                 const QVector<double>::const_iterator &QRSendIterators_one,
                 const QVector<double> &signal_two,
                 const QVector<double>::const_iterator &QRSonsetIterators_two,
                 const QVector<double>::const_iterator &QRSendIterators_two
                ): signal_one(signal_one), signal_two(signal_two)
{
    QVector<double>::const_iterator iterator    = signal_one.begin();
    QVector<double>::const_iterator QRSonset    = QRSonsetIterators_one;
    QVector<double>::const_iterator QRSend      = QRSendIterators_one;
    QVector<double> QRS_Clas;

    EDRsignal_Waves.clear();

    while(iterator != signal_one.end())
    {
        if(iterator == QRSonset)
        {
            while (iterator != QRSend)
            {
                QRS_Clas << *iterator;
                iterator++;
            }
            QRS_Clas << *iterator;
            iterator++;
            QRSonset++;
            QRSend++;

            Integrals_one << integral(QRS_Clas);
            QRS_Clas.clear();
        }
        else
        {
            iterator++;
        }
    }

    iterator    = signal_two.begin();
    QRSonset    = QRSonsetIterators_two;
    QRSend      = QRSendIterators_two;

    while(iterator != signal_two.end())
    {
        if(iterator == QRSonset)
        {
            while (iterator != QRSend)
            {
                QRS_Clas << *iterator;
                iterator++;
            }
            QRS_Clas << *iterator;
            iterator++;
            QRSonset++;
            QRSend++;

            Integrals_two << integral(QRS_Clas);
            QRS_Clas.clear();
        }
        else
        {
            iterator++;
        }
    }
    calculate_signal_from_QRS(Integrals_one, Integrals_two);
}

//funkcja wczytujaca dane i obliczajaca sygnal EDR z wykorzystaniem modulu RPEAKS
//przyjmuje numer sygnalu oraz iteratory kolejnych R-pikow w tym sygnale
void sig_edr::new_RPeaks_signal(int signal_num ,
                                const QVector<double>::const_iterator &RPeaksIterators
                                )
{
    QVector<double>::const_iterator iterator;
    QVector<double>::const_iterator RPeaks  = RPeaksIterators;

    signal_num %=2;//sprawdzamy ktory z sygnalow mamy do dyspozycji
    if(signal_num == 1)//liczenie na podstawie pierwszego
    {
        iterator = signal_one.begin();
        EDRsignal_RPeaks_one.clear();

        while(iterator != signal_one.end())//jezeli natrafimy na R-pik
        {
            if(iterator == RPeaks)
            {
				//to zapisujemy wartosc tego R-piku
                EDRsignal_RPeaks_one << *iterator;
                RPeaks++;
            }
            else
                iterator++;
        }
    }
    else//liczenie na podstawie drugiego - analogicznie jak pierwszego
    {
        iterator = signal_two.begin();
        EDRsignal_RPeaks_two.clear();

        while(iterator != signal_two.end())
        {
            if(iterator == RPeaks)
            {
                EDRsignal_RPeaks_two << *iterator;
                RPeaks++;
            }
            else
                iterator++;
        }

    }
}

//funkcja wczytujaca dane i obliczajaca sygnal EDR z wykorzystaniem modulu WAVES
//przyjmuje iteratory kolejnych punktow QRSonset i QRSend z obu sygnalow
void sig_edr::new_Waves_signal(const QVector<double>::const_iterator &QRSonsetIterators_one,
                               const QVector<double>::const_iterator &QRSendIterators_one,
                               const QVector<double>::const_iterator &QRSonsetIterators_two,
                               const QVector<double>::const_iterator &QRSendIterators_two
                               )
{
    QVector<double>::const_iterator iterator    = signal_one.begin();
    QVector<double>::const_iterator QRSonset    = QRSonsetIterators_one;
    QVector<double>::const_iterator QRSend      = QRSendIterators_one;
    QVector<double> QRS_Clas;
	
	//przetwarzanie pierwszego sygnalu
	
    Integrals_one.clear();//czyszczenie starych sygnalow
    Integrals_two.clear();
    EDRsignal_Waves.clear();

    while(iterator != signal_one.end())//przechodzimy przez caly sygnal EKG
    {
        if(iterator == QRSonset)//jezeli natrafimy na poczatek komlpeksu QRS
        {
            while (iterator != QRSend)//to az do jego konca
            {
                QRS_Clas << *iterator;//przepisujemy wartosci sygnalu do QRS_Clas
                iterator++;
            }
            QRS_Clas << *iterator;
            iterator++;
            QRSonset++;
            QRSend++;

			//liczymy calke z kompleksu QRS i zapisujemy jej wartosc do Integrals_one
            Integrals_one << integral(QRS_Clas);
            QRS_Clas.clear();
        }
        else//jezeli nie natrafilismy na poczatek kompleksu QRS to idziemy dalej
        {
            iterator++;
        }
    }
	
	//przetwarzanie drugiego sygnalu
	//analogiczne jak przetwarzanie pierwszego

    iterator    = signal_two.begin();
    QRSonset    = QRSonsetIterators_two;
    QRSend      = QRSendIterators_two;

    while(iterator != signal_two.end())
    {
        if(iterator == QRSonset)
        {
            while (iterator != QRSend)
            {
                QRS_Clas << *iterator;
                iterator++;
            }
            QRS_Clas << *iterator;
            iterator++;
            QRSonset++;
            QRSend++;

            Integrals_two << integral(QRS_Clas);
            QRS_Clas.clear();
        }
        else
        {
            iterator++;
        }
    }
	//na koncu liczymy wartosci sygnalu EDR na podstawie
	//obliczonych wartosci calek kompleksow QRS z obu sygnalow
    calculate_signal_from_QRS(Integrals_one, Integrals_two);
}

//funkcja zwracajaca sygnal EDR jako QVector
//zarowno z jednej jak i z drugiej metody
QVector<double>* sig_edr::retrieveEDR_QVec(int EDR_type, int signal_num
                                           )
{
    signal_num %= 2;
    EDR_type %= 2;
    if(EDR_type == 1)
    {
        if(signal_num == 1)
            return &EDRsignal_RPeaks_one;
        else
            return &EDRsignal_RPeaks_two;
    }
    else
        return &EDRsignal_Waves;
}

//funkcja zwracajaca sygnal EDR jako std::vector zamiast QVector
//zarowno z jednej jak i z drugiej metody
std::vector<double>* sig_edr::retrieveEDR_StdVec(int EDR_type, int signal_num
                                                 )
{
    signal_num %= 2;
    EDR_type %= 2;
    if(EDR_type == 1)
    {
        if(signal_num == 1)
            EDRsignal = EDRsignal_RPeaks_one.toStdVector();
        else
            EDRsignal = EDRsignal_RPeaks_two.toStdVector();
    }
    else
        EDRsignal = EDRsignal_Waves.toStdVector();

    return &EDRsignal;
}

//funkcja obliczajaca pole powierzchni kompleksu QRS
//czyli srednia wartosc sygnalu miedzy QRS_onset a QRS_end
double sig_edr::integral(QVector<double> &value
                         )
{
    int count   =   value.size();
    double sum  =   0;

    while (!value.isEmpty())
    {
      sum   +=  value.first();
      value.erase(value.begin());
    }

    sum /= count;

    return sum;
}

//funkcja obliczajaca wartosc sygnalu oddechowego na podstawie
//sygnalu EKG z dwoch elektrod - metoda wykorzystujaca modul WAVES
void sig_edr::calculate_signal_from_QRS(const QVector<double> &QRSIntegrals_one,
                                        const QVector<double> &QRSIntegrals_two
                                        )
{
    QVector<double>::const_iterator iterator_one;
    QVector<double>::const_iterator iterator_two;

    EDRsignal_Waves.clear();//czyscimy stary sygnal

    iterator_two = QRSIntegrals_two.begin();//ustawiamy iteratory na poczatek sygnalu

    for(iterator_one = QRSIntegrals_one.begin(); iterator_one != QRSIntegrals_one.end(); iterator_one++)
    {
        EDRsignal_Waves << atan(*iterator_one / *iterator_two);//wartosc sygnalu EDR
        iterator_two++;//inkrementacja obu iteratorow(drugi w petli)
    }
}
