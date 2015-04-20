/***************************************************************************
*  File: viewdata.cpp                                                      *
*  Description: view and fit plots of msr and ascii files.                 * 
*  Author: Hassan Saadaoui                                                 *
*  e-mail: saadaoui@triumf.ca                                              *
*  $Id: mainwindow.h 2014-12-20 $                                          *
*  Copyright (C) 2011 by Hassan Saadaoui                                   *
***************************************************************************/

/***************************************************************************
*Instructions about making changes:
-Make a backup copy of the current working file in the folder ./backup
-Name it file.vX.cpp (or .h). Ex: mainwindow.v9.h, file.v2.cpp...
-X is +1 higher than the number in the last backup copy (see backup folder).
-Make your change to the current version, keeping the same original name.
-Add a list of ALL changes you made to the logs below for future reference. 
-Save it and run make in the parent directory of bnmroffice.
***************************************************************************/

/***************************************************************************
List of Changes:

Changes #: 1 ===============================================================
Date: 2014-12-20
Author: 
List of changes: 
- 

***************************************************************************/
#ifndef BNMR_H
#define BNMR_H



//C++ and C headers
#include<iostream>
#include<fstream>
#include<math.h>
#include<cassert>
#include<string>
#include<sstream>
#include<time.h>
#include<stdio.h>
#include<stddef.h>
#include<stdlib.h>
#include<vector>
#include<unistd.h>
#include<stdarg.h>
#include<iomanip>
#include<iterator>
#include <ctype.h>
#include <stdlib.h>
#include <strings.h>
#include<errno.h>
using namespace std;

#include <QMainWindow>
#include <QPushButton>
#include <QTextBrowser>
#include <QComboBox>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QToolButton>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QTableWidgetItem>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QToolButton>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QActionGroup>
#include <QStandardItemModel>
#include <QDialog>
#include <QMessageBox>
#include <QStyleFactory>
#include <QtGui>
#include <iostream>
#include <QMainWindow>
#include <iostream>
#include <QIcon>
#include <QList>
#include <QPixmap>
#include <QTextStream>
#include <QHBoxLayout>
#include <QColor>
#include <QFileDialog>
#include <QMessageBox>


class BNMR
{
public:
    BNMR(QString inp,QStringList sub,int b,
         double min,double max,int s,int e2,QString f1)
        : input(inp),subType(sub),bin(b),x_min(min), x_max(max),
          sum(s),mode2eType(e2),killscans(f1) {}

    ~BNMR(){};

    QStringList bNMRfiles(QStringList &,QVector< QVector <QString> > &);

    QStringList BNMR_indvar(int,int);

    QVector< QVector<QString> > BNMR_data(int,int,int);

    QStringList  GENERATE(QString,QString);

    void hist_index(void);

    void read_write(QString,QString,QVector<int>&,int&,int&,QVector< QVector<double> >&);

    void bin_data(QString, QString);

    QVector<int> from_lineedit_to_runslist(QString,QStringList &,
                                           QVector< QVector <QString> > &);

    void ModeT1(QString,QString);

    void Mode2e(QString,QString);

    bool Mode1(QString, QString,QVector<int> &);

    QStringList Asym_Type1(QString,int,int,QVector< QVector< double> >,QVector<int>);

    QStringList Asym_Type2(QString,int,int,QVector< QVector< double> >,QVector<int>);

    void initialize_parameters(void);

    QString msrfile(int,int);

    QStringList runHeaders(QVector<int>,QVector<int>,QVector<int>,
                           QVector< QVector <QString> > &);

    int read_numbers(QVector<int>&, QVector<int>&, QVector<int>&,
                     QStringList&,QVector< QVector <QString> >&);

    int read_infFile(QVector<int>&, QVector<int>&, QVector<int>&,
                     QStringList&,QVector< QVector <QString> >&);

    int read_listFile(QVector<int>&, QVector<int>&, QVector<int>&,
                      QStringList&,QVector< QVector <QString> >&);

    int prepareFiles(QVector<int>&, QVector<int>&, QVector<int>&,
                    QStringList&,QVector< QVector <QString> >&);

    QString histList(int);

private:
    int    runNb;
    int    runYear;
    int    runSign;
    QString runType;
    QString runTitle;
    QString runFlag;
    QString expType;
    double x_start;
    double x_stop;
    double x_step;
    double Randomize;
    double RF_on_time;
    double HelFlipSleep;
    double Dwell_time;
    double RFon_delay;
    double max_pow;
    double prebeam_dwt;
    double beam_on_period;
    double beam_on_dwt;
    double beam_off_dwt ;
    int    dwtperfreq;
    int    nbRFdelays;
    int    nbpostRFdw;
    int    nbHists;
    double runTime;
    double x_min;
    double x_max;
    QStringList subType;
    int         bin;
    int         sum;
    int         mode2eType, modType, hisType;
    int         hS, hSN;
    int         M;
    int         h1,h2,h3,h4;
    QString     input,killscans,sumScans;
};

struct Generate_asym
{
    double Mean;
    double StdDev;
};
typedef struct  Generate_asym Generate_asym ;

struct runDescription {
    int      ExptNumber;
    int      RunNumber;
    int      ElapsedSec;
    char	 Title[1000];
    char	 Lab[1000];
    char	 Area[1000];
    char	 Method[1000];
    char	 Apparatus[1000];
    char	 Insert[1000];
    char	 Sample[1000];
    char	 Orient[1000];
    char	 Das[1000];
    char	 Experimenter[1000];
    char	 Temp[1000];
    char	 Field[1000];
    int      Nb_ind_vrbls;
    char     Name[1000][1000];
    double   Mean[1000];
    double   StdDev[1000];
};

extern "C"
{
runDescription rundesc(char *fin_name);
int MUD2COL_convert(char *fin_name, char *fout_name, int nhsel,
                    int   hsel[],int *num_of_hists);
//int MUD_openRead(char*  filename,  unsigned long*   pType );
}

#endif  //BNMR_H
