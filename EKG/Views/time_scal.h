#ifndef TIME_SCAL_H
#define TIME_SCAL_H


class TimeScaleDraw: public QwtScaleDraw
{
public:
    TimeScaleDraw( const QTime &base ):
        baseTime( base )
    {
    }
    virtual QwtText label( double v ) const
    {
        QTime upTime = baseTime.addMSecs(( int )v );
        return upTime.toString("mm:ss:zzz");
    }
private:
    QTime baseTime;
};

#endif // TIME_SCAL_H
