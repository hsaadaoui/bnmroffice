/***************************************************************************
*  File: tab_live.cpp                                                      *
*  Description: streams live data                                          *
*  Author: Hassan Saadaoui                                                 *
*  e-mail: saadaoui@triumf.ca                                              *
*  $Id: tab_live.cpp  2014-12-20 $                                         *
*  Copyright (C) 2011 by Hassan Saadaoui                                   *
***************************************************************************/
/***************************************************************************
    This file is part of BnmrOffice.

    BnmrOffice is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    BnmrOffice  is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with BnmrOffice .  If not, see <http://www.gnu.org/licenses/>.
***************************************************************************/

/***************************************************************************
List of Changes:
----------------------------------------------------------------------------
TYPE  DATE        AUTHOR  DESCRIBE
***************************************************************************/

#include "dialog_stream.h"
#include "ui_dialog_stream.h"
#include "plotting.h"
#include "bnmr.h"

Stream::Stream(QWidget *parent) :
    QDialog(parent),
    uiStream(new Ui::Stream)
{
    uiStream->setupUi(this);
    connect(this,SIGNAL(finished(int)),this, SLOT(stop()));
}

Stream::~Stream()
{
    delete uiStream;
}

void Stream::stop()
{
    loop = false;
}

//reads the graphics options: binning, xmin,xmax;
void Stream::read_graphics()
{
    QStringList graphics = uiStream->lineEdit_graphics->text().split(QRegExp(","));
    bin   = 1;
    x_min = -INFINITY;
    x_max =  INFINITY;

    if (graphics.size() >= 1 && !graphics.at(0).isEmpty() )
    {
        bin  = graphics.at(0).toInt();
        if (graphics.size() >= 2 && !graphics.at(1).isEmpty())
        {
            x_min = graphics.at(1).toDouble();
            if (graphics.size() >= 3 && !graphics.at(2).isEmpty())
            {
                x_max = graphics.at(2).toDouble();
            }
        }
    }

    if (x_min >= x_max)
    {
        QMessageBox::warning(this, tr("Error"),
                             tr("The value of 'max' must be higher than 'min'....Doh!"));
    }
}

void Stream::createData()
{
    //read input
    QString runNumber = QString::number(uiStream->spinBox_number->value());
    QString year      = QString::number(uiStream->spinBox_year->value());
    QString input     = year+":"+runNumber;

    //make default settings: all 1f runs are summed, use default mode for 2e
    int     sum = 1, e2 = 0;
    QString kill = "";

    //specify number of graphs
    int nbGrs = 4;
    //specify the indices of each file //read bnmr.cpp to make sense of these
    QStringList subType[nbGrs],runList[nbGrs];
    subType[0]<<"0"<<"1"<<"2";//0:"Asy" 1:"Asy+" 2:"Asy-" 3:"AsyForR" 4:"AsyBorL"
    subType[1]<<"5"<<"6"<<"7";//5:"AsyN" 6:"AsyN+" 7:"AsyN-"
    subType[2]<<"8"<<"9"<<"10"<<"11";//8:"ForR+" 9:"BorL+" 10:"ForR-" 11:"BorL-"
    subType[3]<<"12"<<"13"<<"14"<<"15";//12:"NF+" 13:"NB+"  14:"NF+"   15:"NB-"

    QVector< QVector<QString> > files;
    files.resize(nbGrs);

    QString xlabel, ylabel;
    xlabel = "Bin";
    ylabel = "Histogram";
    QVector<QString> xLabels,yLabels;
    for(int i = 0; i < nbGrs; i++)
    {
        BNMR bnmr(input,subType[i],bin,x_min,x_max,sum,e2,kill);//This is defined in bnmr.cpp
        QStringList headers;
        QVector< QVector <QString> > headerValue;
        runList[i] = bnmr.bNMRfiles(headers,headerValue);//This is defined in bnmr.cpp
        for(int j = 0; j<runList[i].size(); j++)
            files[i].push_back(runList[i][j]);
        xLabels.push_back(xlabel);
        yLabels.push_back(ylabel);
    }
    //send to plot
    Plotting plot;
    plot.stream(uiStream->plotWidget,files,xLabels,yLabels);
}

//stream loop
void Stream::stream_loop()
{
    while(loop)
    {
        int wait_time = uiStream->spinBox_update->value();
        //wait for a wait_time
        int i = 0;
        while(i<wait_time && loop)
        {
            qApp->processEvents();
            uiStream->lcdNumber->setSegmentStyle(QLCDNumber::Flat);

            uiStream->lcdNumber->setStyleSheet("QLCDNumber{color:black;"
                                               "background-color:rgb(19,235,67,255);}");
            uiStream->lcdNumber->display(QString::number(wait_time-i));
            QEventLoop wait;
            QTimer::singleShot(1000, &wait, SLOT(quit()));//wait 1000 ms
            wait.exec();
            i++;
        }
        //execute the user choice
        if(loop)
        {
            read_graphics();
            createData();
        }
    }
    uiStream->lcdNumber->setStyleSheet("QLCDNumber{color:black;"
                                       "background-color:white;}");
    uiStream->lcdNumber->display(QString::number(0));
}

//respond to user clicks
void Stream::on_pushButton_clicked()
{

    QString text = uiStream->pushButton->text();
    if(text == "Start")
    {
        uiStream->pushButton->setText("Stop");
        uiStream->pushButton->setStyleSheet("QPushButton { background-color:"
                                            "rgb(19,235,67,255); }");
        loop = true;
    }
    else
    {
        uiStream->pushButton->setText("Start");
        uiStream->pushButton->setStyleSheet("QPushButton { background-color: red; }");
        loop = false;
    }
    stream_loop();
}
