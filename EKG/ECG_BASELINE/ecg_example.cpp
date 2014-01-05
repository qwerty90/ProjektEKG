#include "ecg_example.h"
ecg_example::ecg_example()
{
    this->filtered = new QVector<double>();
}

void ecg_example::calculate_mean()
{
    QList<int>::Iterator iter = this->raw_data->begin();
    int tablica[this->window];
    QVector<double>::iterator iterator_v = this->filtered->begin();
    int suma;
    double wynik;
    QLOG_INFO() << "Window:" <<QString::number(this->window);

    for(int i=0;i<this->window;i++)  //nadpisanie poczatku
    {
        suma = 0;
        tablica[i]=*iter;
        for(int j=0;j<=i;j++)
        {
            suma+=tablica[j];
        }
        wynik = suma/(i+1);
        *iterator_v = wynik/this->gain;
        iter++;
        iterator_v++;
    }

    while(iter != this->raw_data->end())  //filtracja
    {
        suma = 0;
        for(int i=0;i<this->window-1;i++)
        {
            tablica[i]=tablica[i+1];
            suma +=tablica[i];
        }
        tablica[this->window] = *iter;        
        suma += tablica[this->window];
        wynik = suma/this->window;
        *iterator_v = wynik/this->gain;
        iter++;
        iterator_v++;
    }
    for (int i=0;i<this->window;i++)  //nadpisanie konca (zle)
         iter--;
    for(int i=0;i<this->window;i++)
    {
        tablica[i]=*iter;
        for(int j=0;j<i;j++)
            suma=tablica[j];
        *iterator_v=suma/(i+1)/this->gain;
        iterator_v++;
        iter++;
    }
}

void ecg_example::run()
{
    if (method=="mean")
    {
        this->calculate_mean();
    }
    else
    {
        QLOG_ERROR() << "Blad wczytania metody";
    }
    QLOG_INFO() << "Baseline done.";
}

void ecg_example::get_params(int window_size, QString method, int gain)
{
    this->window = window_size;
    this->method = method;
    this->gain   = gain;
}

void ecg_example::get_data(QList<int>* raw_data)
{
    this->raw_data=raw_data;
    QLOG_INFO() << "Wczytano " <<QString::number(raw_data->size()) << "probek";
    this->filtered->resize(this->raw_data->size()+1);
}
QVector<double> *ecg_example::export_data()
{
    return this->filtered;
}
