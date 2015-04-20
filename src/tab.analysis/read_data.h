#ifndef READ_DATA_H
#define READ_DATA_H

#include<QTextStream>
#include<QFile>
#include<QFileDevice>
#include<QVector>
#include<QDebug>

class readData
{

public:
    readData(QVector<int> len, QStringList files,double,double,double,double);

    ~readData(){}

    QVector< QVector< QVector<double> > > getxData(void);

    QVector< QVector< QVector<double> > > getyData(void);

    bool readFile(QString,QVector< QVector<double> > &,QVector< QVector<double> >&);

private:
    QVector< QVector< QVector<double> > > xData,yData;

    //Holds the nmbers of arguments of each variable.
    QVector<int> Length;//total nb of variables

    QStringList Files;

    double Xmin;

    double Xmax;

    double Ymin;

    double Ymax;

};
#endif
