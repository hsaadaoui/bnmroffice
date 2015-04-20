#include "read_data.h"

readData::readData(QVector<int> len, QStringList files,
                   double xmin, double xmax,
                   double ymin, double ymax)
{
    Length  = len;
    Files   = files;
    Xmin    = xmin;
    Xmax    = xmax;
    Ymin    = ymin;
    Ymax    = ymax;
    for (short i = 0; i < Files.size(); i++)
    {
        QVector< QVector<double> > X,Y;
        bool fileExists = readFile(Files.at(i),X,Y);
        if (fileExists )
        {
            xData.push_back(X);
            yData.push_back(Y);
        }
    }
}

//read file
bool readData::readFile(QString inFile,QVector< QVector<double> > &X,
                        QVector< QVector<double> > &Y)
{
    QFile file(inFile);
    file.open(QIODevice::ReadOnly);
    if (!file.exists() && file.size()>0)
    {
        qDebug() <<"Error opening input data file.\n"
                   "Maybe it does not exist or holds data\n";
    }
    else
    {
        QTextStream indata(&file);
        while(!indata.atEnd())
        {
            QString in;
            bool ok;
            double x = 0, y = 0;
            int nbVars = Length.size();
            X.resize(Length.at(0));
            Y.resize(Length.at(1));

            for(short v = 0; v < nbVars; v++)
            {
                int nbCols = Length.at(v);
                for(short c = 0; c < nbCols; c++)
                {
                    indata >> in;
                    if (in.toDouble(&ok)&& ok &&  !indata.atEnd() )
                    {
                        if (v == 0          && c == 0)
                            x = in.toDouble();
                        if (v == nbVars - 1 && c == 0)
                            y = in.toDouble();

                        if (v < nbVars  - 1 && x <= Xmax && x>=Xmin && y <= Ymax && y>=Ymin)
                            X[c].push_back(in.toDouble());

                        if (v == nbVars - 1 && x <= Xmax && x>=Xmin && y <= Ymax && y>=Ymin)
                            Y[c].push_back(in.toDouble());
                    }
                }
            }
        }
    }
    file.close();
    return file.exists();
}

//xData
QVector< QVector< QVector<double> > > readData::getxData(void)
{
    return xData;
}

//yData
QVector< QVector< QVector<double> > > readData::getyData(void)
{
    return yData;
}
