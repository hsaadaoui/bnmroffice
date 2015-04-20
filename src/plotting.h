#ifndef PLOTTING_H
#define PLOTTING_H

#include <QWidget>
#include <QHeaderView>
#include <QTableWidget>
#include <QFileDialog>
#include <QObject>
#include <QDir>
#include <QFileInfo>
#include "qcustomplot.h"
#include "mainwindow.h"


class Plotting
{

public:
    Plotting();

    ~Plotting();

    void qcp(QCustomPlot*, QVector< QVector<QString> >,QVector<QString>,QVector<QString>);

    void stream(QCustomPlot*, QVector< QVector<QString> >,QVector<QString>,QVector<QString>);

    void xmgr(int,QVector<int>,QVector<int>,QVector<int>,QVector<QString>,QVector<QString>,
                   QVector<QString>,QVector<QString>,QVector<QString>);

    bool qcp_readFile(QVector<double>&, QVector<double>&,QString);

    bool qcp_readFile(QVector<double>&, QVector<double>&,QVector<double>&,QString);

    void qcp_exportPlot(QCustomPlot*,QString);

    void qcp_clear(QCustomPlot*);

    QColor colorName(int i);

    Qt::PenStyle lineStyle(int i);

private:

};

#endif // CLASSPLOTING_H
