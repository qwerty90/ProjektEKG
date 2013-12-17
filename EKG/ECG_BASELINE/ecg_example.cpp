#include "ecg_example.h"
ecg_example::ecg_example()
{
    this->filtered = new QList<double>;
}

void ecg_example::calculate_mean()
{
    QList<int>::Iterator iter = this->raw_data->begin();
    int tablica[this->window];
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
        this->filtered->append(wynik);
        iter++;
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
        iter++;
        wynik = suma/this->window;
        this->filtered->append(wynik);
    }

    for (int i=0;i<this->window;i++)  //nadpisanie konca (zle)
        iter = iter--;
    for(int i=0;i<this->window;i++)
    {
        tablica[i]=*iter;
        for(int j=0;j<i;j++)
            suma=tablica[j];
        this->filtered->append(suma/(i+1));
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

void ecg_example::get_params(int window_size,QString method)
{
    this->window = window_size;
    this->method = method;
}

void ecg_example::get_data(QList<int>* raw_data)
{
    this->raw_data=raw_data;
}
QList<double> *ecg_example::export_data()
{
    return this->filtered;
}
