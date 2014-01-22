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
                 const QVector<unsigned int> &RPeaksIterators_one,
                 const QVector<double> &signal_two,
                 const QVector<unsigned int> &RPeaksIterators_two
                 ): signal_one(signal_one), signal_two(signal_two)
{
    QVector<unsigned int>::const_iterator iterator = RPeaksIterators_one.begin();

//  Pierwszy sygnal

    EDRsignal_RPeaks_one.clear();

    if(!RPeaksIterators_one.empty())
    {
        while(iterator != RPeaksIterators_one.end())
        {
            EDRsignal_RPeaks_one << signal_one[*iterator];
            iterator++;
        }
    }

//  Drugi sygnal

    iterator = RPeaksIterators_two.begin();

    EDRsignal_RPeaks_two.clear();

    if(!RPeaksIterators_two.empty())
    {
        while(iterator != RPeaksIterators_two.end())
        {

            EDRsignal_RPeaks_two << signal_two[*iterator];
            iterator++;
        }
    }
}

//konstruktor przypisujacy otrzymane sygnaly EKG z dwoch elektrod,
//iteratory QRSonset i QRSend do odpowiednich pol w obiekcie
sig_edr::sig_edr(const QVector<double> &signal_one,
                 const vector_it &QRSonsetIterators_one,
                 const vector_it &QRSendIterators_one,
                 const QVector<double> &signal_two,
                 const vector_it &QRSonsetIterators_two,
                 const vector_it &QRSendIterators_two
                ): signal_one(signal_one), signal_two(signal_two)
{
    QVector<double>::const_iterator iterator    = signal_one.begin();
    unsigned int QRS_size   = QRSonsetIterators_one.size() - 1;
    unsigned int QRS_iter   = 0;
    QVector<double> QRS_Clas;

    EDRsignal_Waves.clear();

    if(!QRSonsetIterators_one.isEmpty() && !QRSendIterators_one.isEmpty() && !QRSonsetIterators_two.isEmpty() && !QRSendIterators_two.isEmpty())
    {
        while(QRS_iter != QRS_size)
        {
            if(*iterator == *(QRSonsetIterators_one[QRS_iter]))
            {
                while (*iterator != *(QRSendIterators_one[QRS_iter]))
                {
                    QRS_Clas << signal_one[*iterator];
                    iterator++;
                }
                QRS_Clas << *iterator;
                iterator++;
                QRS_iter++;

                Integrals_one << integral(QRS_Clas);
                QRS_Clas.clear();
            }
            else
            {
                iterator++;
            }
        }

        iterator    = signal_two.begin();

        QRS_size   = QRSonsetIterators_two.size() - 1;
        QRS_iter   = 0;

        while(QRS_iter != QRS_size)
        {
            if(*iterator == *(QRSonsetIterators_two[QRS_iter]))
            {
                while (*iterator != *(QRSendIterators_two[QRS_iter]))
                {
                    QRS_Clas << signal_one[*iterator];
                    iterator++;
                }
                QRS_Clas << *iterator;
                iterator++;
                QRS_iter++;

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
}

//funkcja wczytujaca dane i obliczajaca sygnal EDR z wykorzystaniem modulu RPEAKS
//przyjmuje numer sygnalu oraz iteratory kolejnych R-pikow w tym sygnale
void sig_edr::new_RPeaks_signal(int signal_num ,
                                const QVector<unsigned int> &RPeaksIterators
                                )
{
    QVector<unsigned int>::const_iterator iterator = RPeaksIterators.begin();

    if(!RPeaksIterators.empty())
    {
        signal_num %=2;//sprawdzamy ktory z sygnalow mamy do dyspozycji
        if(signal_num == 1)//liczenie na podstawie pierwszego
        {
            EDRsignal_RPeaks_one.clear();

            while(iterator != RPeaksIterators.end())//jezeli natrafimy na R-pik
            {

                //to zapisujemy wartosc tego R-piku
                EDRsignal_RPeaks_one << signal_one[*iterator];
                iterator++;
            }
        }
        else//liczenie na podstawie drugiego - analogicznie jak pierwszego
        {

            EDRsignal_RPeaks_two.clear();

            while(iterator != RPeaksIterators.end())
            {

                EDRsignal_RPeaks_two << signal_one[*iterator];
                iterator++;
            }

        }
    }
}

//funkcja wczytujaca dane i obliczajaca sygnal EDR z wykorzystaniem modulu WAVES
//przyjmuje iteratory kolejnych punktow QRSonset i QRSend z obu sygnalow
void sig_edr::new_Waves_signal(const vector_it &QRSonsetIterators_one,
                               const vector_it &QRSendIterators_one,
                               const vector_it &QRSonsetIterators_two,
                               const vector_it &QRSendIterators_two
                               )
{
    QVector<double>::const_iterator iterator    = signal_one.begin();
    unsigned int QRS_size   = QRSonsetIterators_one.size() - 1;
    unsigned int QRS_iter   = 0;
    QVector<double> QRS_Clas;

    EDRsignal_Waves.clear();

    if(!QRSonsetIterators_one.isEmpty() && !QRSendIterators_one.isEmpty() && !QRSonsetIterators_two.isEmpty() && !QRSendIterators_two.isEmpty())
    {
        while(QRS_iter != QRS_size)
        {
            if(*iterator == *(QRSonsetIterators_one[QRS_iter]))
            {
                while (*iterator != *(QRSendIterators_one[QRS_iter]))
                {
                    QRS_Clas << signal_one[*iterator];
                    iterator++;
                }
                QRS_Clas << *iterator;
                iterator++;
                QRS_iter++;

                Integrals_one << integral(QRS_Clas);
                QRS_Clas.clear();
            }
            else
            {
                iterator++;
            }
        }

        iterator    = signal_two.begin();

        QRS_size   = QRSonsetIterators_two.size() - 1;
        QRS_iter   = 0;

        while(QRS_iter != QRS_size)
        {
            if(*iterator == *(QRSonsetIterators_two[QRS_iter]))
            {
                while (*iterator != *(QRSendIterators_two[QRS_iter]))
                {
                    QRS_Clas << signal_one[*iterator];
                    iterator++;
                }
                QRS_Clas << *iterator;
                iterator++;
                QRS_iter++;

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
