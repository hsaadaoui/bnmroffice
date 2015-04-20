/***************************************************************************
*  File: mainwindow.cpp                                                    *
*  Description: invokes the gui                                            *
*  Author: Hassan Saadaoui                                                 *
*  e-mail: saadaoui@triumf.ca                                              *
*  $Id: mainwindow.cpp 2014-12-20 $                                        *
*  Copyright (C) 2014 by Hassan Saadaoui                                   *
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

#include "plotting.h"


Plotting::Plotting()
{

}

Plotting::~Plotting()
{

}

void Plotting::xmgr(int nbgraphs,QVector<int>nbsets,
                    QVector<int> xformats,QVector<int> yformats,
                    QVector<QString> xaxislabel,QVector<QString> yaxislabel,
                    QVector<QString> names,QVector<QString> legends,
                    QVector<QString> titles)
{
    bool combinePlots = Mainwindow::combinePlots;
    bool keepPlots = Mainwindow::keepPlots;
    if ( keepPlots == false )
        system("pkill xmgr");

    QString char_size  = QString::number(1.0 - nbgraphs *0.025,'f',2);
    QString label_size = QString::number(1.0 - nbgraphs *0.02);

    int nh           = (int)sqrt(nbgraphs);
    int nv           = (int)sqrt(nbgraphs);
    if( nh*nv <nbgraphs )
        nv++;
    if( nh*nv <nbgraphs )
        nv++;
    if( nh*nv <nbgraphs )
        nh++;
    if ( nbgraphs > 32 )
    {
        qDebug()<<"xmgr cannot plot more than 32 graphs...reduce and replot!\n";
    }
    else
    {
        QVector<QVector <QString> > Name,Legend,Title;
        Name.resize(nbgraphs),Legend.resize(nbgraphs),Title.resize(nbgraphs);

        QVector<QVector <int> > xFormat,yFormat;
        xFormat.resize(nbgraphs),yFormat.resize(nbgraphs);

        QString yaxis="";
        int nbtotsets = 0;

        for (int g = 0; g< nbgraphs; g++)
        {
            for (int s = g*nbsets[g]; s <(g+1)*nbsets[g];s++)
            {
                if ( combinePlots == 0 || nbgraphs == 1)
                {
                    Name[g].push_back(names[s]);
                    xFormat[g].push_back(xformats[s]);
                    yFormat[g].push_back(yformats[s]);
                    Legend[g].push_back(legends[s]);
                    Title[g].push_back(titles[g]);
                }
                if ( combinePlots == 1 && nbgraphs != 1)
                {
                    Name[0].push_back(names[s]);
                    xFormat[0].push_back(xformats[s]);
                    yFormat[0].push_back(yformats[s]);
                    Legend[0].push_back(legends[s]);
                    Title[0].push_back("");
                }
                nbtotsets     ++;
            }
        }
        if ( combinePlots == 1 && nbgraphs != 1)
        {
            nbgraphs = 1;
            nbsets[0] = nbtotsets;
            nv = 1;
            nh = 1;
            yaxislabel[0] = yaxis;
        }
        QString plot = "xmgr -rvideo  -pexec \"legend on\" -maxgraph 32 -maxsets 100 -arrange "
                +QString::number(nv)+" "+QString::number(nh)+" ";

        for (int g = 0; g < nbgraphs; g++)
        {
            plot +=" -graph " + QString::number(g)
                    +" -pexec \"subtitle \\\" "+Title[g][0]+" \\\"  \"  "
                    +" -pexec \"subtitle color 2\" "
                    +" -pexec \"subtitle size \""+char_size+"\" \" "
                    +" -pexec \"xaxis ticklabel char size \""+char_size+"\" \" "
                    +" -pexec \"yaxis ticklabel char size \""+char_size+"\" \" "
                    +" -pexec \"xaxis label \\\" "+xaxislabel[g]+"\\\" \" "
                    +" -pexec \"yaxis label \\\" "+yaxislabel[g]+"\\\" \" "
                    +" -pexec \"xaxis label char size \""+label_size+"\" \" "
                    +" -pexec \"yaxis label char size \""+label_size+"\" \" "
                    +" -pexec \"xaxis tick size \""+char_size+"\" \" "
                    +" -pexec \"yaxis tick size \""+char_size+"\" \" ";

            for (int s = 0; s < nbsets[g]; s++)
            {
                int xF = xFormat[g][s];
                int yF = yFormat[g][s];
                QString S = QString::number(s);
                QFile inFile(Name[g][s]);
                inFile.open(QIODevice::ReadOnly|QIODevice::Text);
                QTextStream indata(&inFile);
                int nbLines=0;
                while(!indata.atEnd())
                {
                    indata.readLine();
                    nbLines++;
                }
                inFile.close();
                QString symbol_size = (nbLines<30)? char_size: "0.3";
                int ssymb = ( s > 9 )? 3: s + 2 ;

                if      ( xF == 1 &&  yF == 1)
                    plot  +=" -xy -pexec \"s"+S+" linestyle 1\" -pexec \"s"+S+" linewidth 3\" ";
                else  if ( xF <= 3 &&  yF <= 3 )
                {
                    if ( xF == 1 &&  yF == 2)
                        plot +=" -xydy ";
                    else if ( xF == 1 &&  yF == 3)
                        plot +=" -xydydy ";
                    else if ( xF == 2 &&  yF == 1)
                        plot +=" -xydx ";
                    else if ( xF == 3 &&  yF == 1)
                        plot +=" -xydxdx ";
                    else if ( xF == 2 &&  yF == 2)
                        plot += " -xydxdy ";
                    plot  +=" -pexec  \"s" +S+ " linestyle 0\"  "
                            +" -pexec  \"s" +S+ " errorbar length 0.200000\" "
                            +" -pexec  \"s" +S+ " symbol " +QString::number(ssymb)+"\" "
                            +" -pexec  \"s" +S+ " symbol size \""+symbol_size+"\" \" "
                            +" -pexec  \"s" +S+ " symbol fill 1\" ";
                }
                else
                    plot += " -nxy -pexec \"s" +S+ " linewidth 3\" " ;

                if ( nbsets[g] > 1 && nbgraphs == 1)
                {
                    plot += " -pexec \"legend  x1 0.01\" "
                            " -pexec \"legend  y1 0.99\" "
                            " -pexec \"legend  char size \""+char_size+"\" \" "
                            " -pexec \"legend  string "+S+" \\\" ";

                    if (   nbsets[g] <= 5 )
                        plot += Legend[g][s]+" \\\" \" " ;
                    else
                        plot += Name[g][s]+" \\\" \" " ;

                }
                plot +=   Name[g][s] ;
            }
        }
        popen(plot.toStdString().c_str(),"w");
    }

    if ( Mainwindow::keepAscii == false )
    {
        QString deleteFiles="rm -rf ";
        for (int g = 0; g < nbgraphs; g++)
            for (int s = g*nbsets[g]; s <(g+1)*nbsets[g];s++)
                deleteFiles += names[s]+" ";
        //wait 1s to make sure xmgr has loaded the files
        QEventLoop wait;
        QTimer::singleShot(1000, &wait, SLOT(quit()));
        wait.exec();
        //issue delete
       // popen(deleteFiles.toStdString().c_str(),"w");
    }
}

Qt::PenStyle Plotting::lineStyle(int i)
{
    if (i==0)
        return Qt::SolidLine;
    else if (i==1)
        return Qt::DashLine;
    else if (i==2)
        return Qt::DotLine;
    else if (i==3)
        return Qt::DashDotLine;
    else if (i==4)
        return Qt::DashDotDotLine;
}

QColor Plotting::colorName(int i)
{
    if (i==0)
        return Qt::red;
    else if (i==1)
        return Qt::blue;
    else if (i==2)
        return Qt::green;
    else if (i==3)
        return Qt::yellow;
    else if (i==4)
        return Qt::magenta;
    else if (i==5)
        return Qt::cyan;
    else if (i==6)
        return Qt::gray;
    else if (i==7)
        return Qt::black;
    else if (i==8)
        return Qt::white;
    else if (i==9)
        return Qt::darkRed;
    else if (i==10)
        return Qt::darkBlue;
    else if (i==11)
        return Qt::darkGreen;
    else if (i==12)
        return Qt::darkYellow;
    else if (i==14)
        return Qt::darkMagenta;
    else if (i==14)
        return Qt::darkCyan;
    else if (i==15)
        return Qt::darkGray;
    return QColor(i).light(255);
}

void Plotting::qcp_clear(QCustomPlot* plot)
{
    //clear all graphs
    plot->clearGraphs();
    //send the signal to plot
    plot->replot();
}
void Plotting::qcp(QCustomPlot *plot, QVector< QVector<QString> > files,
                   QVector<QString> xLabels,QVector<QString> yLabels)
{
    //clear all graphs
    plot->clearPlottables();

    //number of new graphs
    int nbGrs = files.size();

    //loop over each graph
    for ( int g = 0; g < nbGrs;g++)
    {
        //loop over sets of each graph
        int nbSets = files[g].size();
        //qDebug()+nbGrs+nbSets ;
        for ( int s = 0; s < nbSets;s++)
        {
            plot->addGraph();
            QPen customPen;
            customPen.setColor(colorName(s));
            customPen.setStyle(lineStyle(s));
            customPen.setWidthF(3);
            plot->graph(s)->setPen(customPen);

            QVector<double> x, y;
            bool state = qcp_readFile(x,y,files[g][s]);
            if (state == true)
            {
                plot->graph(s)->setData(x,y);
                plot->graph(s)->rescaleAxes(true);
                plot->graph(s)->setName(files[g][s]);
            }
        }
        plot->rescaleAxes();
        //give the axes some labels:
        plot->setInteraction(QCP::iRangeDrag, true);
        plot->setInteraction(QCP::iRangeZoom, true);
        plot->setInteraction(QCP::iSelectPlottables, true);
        plot->legend->setVisible(true);

        // start out with MainWindow's font..
        QFont legendFont;
        legendFont.setPointSize(9);
        plot->legend->setFont(legendFont);
        plot->legend->setBrush(QBrush(Qt::transparent));
        //QColor(198,199,154,255)));//Qt::white
        plot->xAxis->setLabel(xLabels[g]);
        plot->yAxis->setLabel(yLabels[g]);
    }
    plot->setBackground(Qt::darkGray);
    plot->axisRect()->setBackground(Qt::darkGray);
    //send the signal to plot
    plot->replot();
}



void Plotting::stream(QCustomPlot *plot, QVector< QVector<QString> > files,
                      QVector<QString> xLabels,QVector<QString> yLabels)
{
    plot->plotLayout()->clear();
    QPixmap pic;
    pic.fill(Qt::black);
    plot->setBackground(pic);
    plot->clearPlottables();

    QCPLayoutGrid *upLayout = new QCPLayoutGrid;
    QCPLayoutGrid *downLayout = new QCPLayoutGrid;
    plot->plotLayout()->addElement(0, 0, upLayout);
    plot->plotLayout()->addElement(1, 0, downLayout);
    plot->plotLayout()->setRowStretchFactor(1, 1);

    QCPAxisRect *upRectLeft = new QCPAxisRect(plot);
    //give the axes some labels:
    upRectLeft->axis(QCPAxis::atBottom)->setLabel(xLabels[0]);
    upRectLeft->axis(QCPAxis::atLeft)->setLabel(yLabels[0]);
    QCPAxisRect *upRectRight = new QCPAxisRect(plot);
    upRectRight->axis(QCPAxis::atBottom)->setLabel(xLabels[1]);
    upRectRight->axis(QCPAxis::atLeft)->setLabel(yLabels[1]);
    QCPAxisRect *downRectLeft = new QCPAxisRect(plot);
    downRectLeft->axis(QCPAxis::atBottom)->setLabel(xLabels[2]);
    downRectLeft->axis(QCPAxis::atLeft)->setLabel(yLabels[2]);
    QCPAxisRect *downRectRight = new QCPAxisRect(plot);
    downRectRight->axis(QCPAxis::atBottom)->setLabel(xLabels[3]);
    downRectRight->axis(QCPAxis::atLeft)->setLabel(yLabels[3]);
    upLayout->addElement(0, 0, upRectLeft);
    upLayout->addElement(0, 1, upRectRight);
    downLayout->addElement(0, 0, downRectLeft);
    downLayout->addElement(0, 1, downRectRight);

    plot->setAutoAddPlottableToLegend(false); // would add to the main legend (in the primary axis rect)
    // setup an extra legend for that axis rect:
    QCPLegend *ulLegend = new QCPLegend;
    upRectLeft->insetLayout()->addElement(ulLegend, Qt::AlignTop|Qt::AlignRight);
    ulLegend->setLayer("legend");

    QCPLegend *urLegend = new QCPLegend;
    upRectRight->insetLayout()->addElement(urLegend, Qt::AlignTop|Qt::AlignRight);
    urLegend->setLayer("legend");


    QCPLegend *dlLegend = new QCPLegend;
    downRectLeft->insetLayout()->addElement(dlLegend, Qt::AlignTop|Qt::AlignRight);
    dlLegend->setLayer("legend");


    QCPLegend *drLegend = new QCPLegend;
    downRectRight->insetLayout()->addElement(drLegend, Qt::AlignTop|Qt::AlignRight);
    drLegend->setLayer("legend");


    QString deleteFiles="rm -rf ";
    for (int g = 0; g < files.size(); g++)
        for (int s = 0; s <files.at(g).size();s++)
            deleteFiles += files[g][s]+" ";

    QVector < QVector< QVector <double> > > X(4), Y(4),dY(4);
    int nbGrs = files.size();
    int G=0;
    for ( int g = 0; g < nbGrs; g++)
    {
        int nbSets = files.at(g).size();
        X[g].resize(nbSets);
        Y[g].resize(nbSets);
        dY[g].resize(nbSets);

        for ( int s = 0; s < nbSets;s++)
        {
            QVector <double> x,y,dy;
            qcp_readFile(x,y,dy,files[g][s]);
            X[g][s] = x;
            Y[g][s] = y;
            dY[g][s] = dy;
            QPen customPen;
            customPen.setColor(colorName(s));
            customPen.setWidthF(2);
            if (g==0)
            {
                plot->addGraph(upRectLeft->axis(QCPAxis::atBottom),upRectLeft->axis(QCPAxis::atLeft));
                ulLegend->addItem(new QCPPlottableLegendItem(ulLegend, plot->graph(G)));
            }
            if (g==1)
            {
                plot->addGraph(upRectRight->axis(QCPAxis::atBottom),upRectRight->axis(QCPAxis::atLeft));
                urLegend->addItem(new QCPPlottableLegendItem(urLegend, plot->graph(G)));
            }
            if (g==2)
            {
                plot->addGraph(downRectLeft->axis(QCPAxis::atBottom),downRectLeft->axis(QCPAxis::atLeft));
                dlLegend->addItem(new QCPPlottableLegendItem(dlLegend, plot->graph(G)));
            }
            if (g==3)
            {
                plot->addGraph(downRectRight->axis(QCPAxis::atBottom),downRectRight->axis(QCPAxis::atLeft));
                drLegend->addItem(new QCPPlottableLegendItem(drLegend, plot->graph(G)));

            }
            plot->graph(G)->setLineStyle(QCPGraph::lsNone);
            plot->graph(G)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle,
                                                            colorName(s), colorName(s), 2));
            plot->graph(G)->setErrorType(QCPGraph::etValue);
            plot->graph(G)->setErrorPen(customPen);
            plot->graph(G)->setDataValueError(X[g][s], Y[g][s],dY[g][s]);
            plot->graph(G)->setErrorBarSize(0);
            plot->graph(G)->setName(files[g][s].remove(QRegExp(".dat")).remove(0,6));
            plot->graph(G)->setErrorType(QCPGraph::etValue);
            if(s==0)
                plot->graph(G)->rescaleAxes();
            else
                plot->graph(G)->rescaleAxes(true);
            G++;
        }
    }
    plot->setInteraction(QCP::iRangeDrag, true);
    plot->setInteraction(QCP::iRangeZoom, true);
    plot->setInteraction(QCP::iSelectPlottables, true);
    plot->setBackground(Qt::darkGray);
    plot->axisRect()->setBackground(Qt::darkGray);
    plot->replot();

    if ( Mainwindow::keepAscii == false )
        popen(deleteFiles.toStdString().c_str(),"w");
}
void Plotting::qcp_exportPlot(QCustomPlot *plot,QString fileName)
{
    if (QFileInfo(fileName).suffix() == "pdf")
    {
        plot->savePdf(fileName,false, 0, 0,"A","B");
    }
    else if (QFileInfo(fileName).suffix() == "jpg")
    {
        plot->saveJpg(fileName,  0, 0,1.0,-1.0);
    }
    else if (QFileInfo(fileName).suffix() == "bmp")
    {
        plot->saveBmp(fileName,  0, 0,1.0);
    }
}


bool Plotting::qcp_readFile(QVector<double> &x, QVector<double> &y, QString inFile)
{
    QFile file(inFile);
    file.open(QIODevice::ReadOnly);
    if (!file.exists())
    {
        qDebug() <<"Error opening input data file\n"
                <<inFile
               <<" , maybe it does not exist or holds data\n";
        return false;
    }
    else
    {
        QTextStream indata(&file);
        QString inX, inY;
        bool ok;
        while (true)
        {
            indata >> inX >> inY;
            if (   inX.toDouble(&ok)&& ok  == true
                   && inY.toDouble(&ok)&& ok  == true)
            {
                x.push_back(inX.toDouble());
                y.push_back(inY.toDouble());
            }
            if( indata.atEnd() ) break;
        }
        return true;
    }
}



bool Plotting::qcp_readFile(QVector<double> &x, QVector<double> &y,QVector<double> &dy, QString inFile)
{
    QFile file(inFile);
    file.open(QIODevice::ReadOnly);
    if (!file.exists())
    {
        qDebug() <<"Error opening input data file\n"
                <<inFile
               <<" , maybe it does not exist or holds data\n";
        return false;
    }
    else
    {
        QTextStream indata(&file);
        QString inX, inY,indY;
        bool ok;
        while (true)
        {
            indata >> inX >> inY>>indY;
            if (   inX.toDouble(&ok)&& ok  == true
                   && inY.toDouble(&ok)&& ok  == true
                   && indY.toDouble(&ok)&& ok  == true)
            {
                x.push_back(inX.toDouble());
                y.push_back(inY.toDouble());
                dy.push_back(indY.toDouble());
            }
            if( indata.atEnd() ) break;
        }
        return true;
    }
}

