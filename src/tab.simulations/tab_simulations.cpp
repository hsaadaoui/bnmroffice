/***************************************************************************
*  File: tab_simulations.cpp                                              *
*  Description: reads and makes 2D plots out of the fitting results files. *
*  Author: Hassan Saadaoui                                                 *
*  e-mail: saadaoui@triumf.ca                                              *
*  $Id: tab_simulations.cpp 2014-12-20 $                                  *
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
#include "tab_simulations.h"
#include "ui_tab_simulations.h"
#include "vanVleck.h"
#include "energygap.h"
#include "plotting.h"

Simulations::Simulations(QWidget *parent) :
    QWidget(parent),
    simGUI(new Ui::Simulations)
{
    simGUI->setupUi(this);
    energygap_change_window();
}

Simulations::~Simulations()
{
    delete simGUI;
}

void Simulations::on_Vleck_pushButton_start_clicked()
{
    int implanted       = simGUI->Vleck_comboBox_implanted   -> currentIndex();
    int target          = simGUI->Vleck_comboBox_lattice     -> currentIndex();
    int nb_of_sites     = 3;

    QString implantedStr       = simGUI->Vleck_comboBox_implanted -> currentText();
    QString targetStr          = simGUI->Vleck_comboBox_lattice   -> currentText();
    QString qname_lattice      = simGUI->Vleck_comboBox_latticetype   -> currentText();

    //latice crystal or powder;
    string name_lattice ="crystal";
    int type_lattice = 0;
    switch(simGUI->Vleck_comboBox_latticetype -> currentIndex())
    {
    case 0: type_lattice = 0; name_lattice = "crystal";break;
    case 1: type_lattice = 1; name_lattice = "powder"; break;
    }


    QVector< QVector<QString> > sites;
    sites.resize(3);
    sites[0].resize(3);
    sites[0]=simGUI->Vleck_lineEdit_site1->text().split(QRegExp(",")).toVector();
    sites[1].resize(3);
    sites[1]=simGUI->Vleck_lineEdit_site2->text().split(QRegExp(",")).toVector();
    sites[2].resize(3);
    sites[2]=simGUI->Vleck_lineEdit_site3->text().split(QRegExp(",")).toVector();

    QVector< QVector<QString> > files;
    files.resize(1);

    for ( int i = 0; i < nb_of_sites;i++)
    {
        //construct name of each file;
        QString Name = "vvSM_of_"+implantedStr +"_in_"+targetStr +"_"+qname_lattice
                +"_site"+QString::number(i+1)+".dat";

        //read x,y,z from ui
        double X =   sites[i][0].toDouble();
        double Y =   sites[i][1].toDouble();
        double Z =   sites[i][2].toDouble();

        //MAIN SIMULATION IS DONE HERE: calling Vleck Simulation code in ext/
        vector< vector<double> > data = Vleck_sim(implanted,target,type_lattice,X,Y,Z);

        //nb of rows in each data file
        int nbRows = data[0].size();

        //make an ascii file
        ofstream file(Name.toStdString().c_str());
        for (int j = 0; j < nbRows; j++)
        {
            file <<  data[0].at(j)<<"\t"<<  data[1].at(j) <<endl;
        }
        file.close();
        files[0].push_back(Name);
    }
    QVector<QString> xLabels,yLabels;
    xLabels.push_back("Angle(Degree)");
    yLabels.push_back("sigma(kHz)");
    Plotting plot;
    plot.qcp(simGUI->vv_plotWidget,files,xLabels,yLabels);
}

void Simulations::on_Vleck_pushButton_export_clicked()
{
    Plotting plot;
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As pdf, jpg, bmp"),
                                                    QDir::currentPath(),
                                                    tr("image file (*.pdf *.jpg *.bmp)"));
    plot.qcp_exportPlot(simGUI->vv_plotWidget,fileName);
}

void Simulations::on_Vleck_pushButton_clear_clicked()
{
    Plotting plot;
    plot.qcp_clear(simGUI->vv_plotWidget);
}

void Simulations::on_Vleck_pushButton_purge_clicked()
{
    Plotting plot;
    plot.qcp_clear(simGUI->vv_plotWidget);
}



void Simulations::on_energygap_comboBox_function_currentIndexChanged(const QString &arg1)
{
    energygap_change_window();
}

void Simulations::energygap_change_window()
{
    QStringList parVal, parMin, parMax,parName;

    QString  fctChoice    =simGUI->energygap_comboBox_function   -> currentText();
    if( fctChoice == "Sigma_vs_temp"  )
    {
        parVal <<"0.8"     << "5.3"   << "500"     << "10000"  << "200"        << "0.05"         << "0.5";
        parName<<"D0(meV)" << "Tc(K)" << "Bapp(G)" << "Bc2(G)" << "lambda0(nm)"<< "sigmaTc(MHz)" << "a";
        parMin <<"0.0"     << "0.0"   << "0.0"     << "0.0"    << "0.0"        << "0.0"          << "0.0";
        parMax <<"0.0"     << "0.0"   << "0.0"     << "0.0"    << "0.0"        << "0.0"          << "0.0";
    }
    if( fctChoice == "Sigma_vs_field"  )
    {
        parVal  << "10000"    << "200"        ;
        parName << "Bc2(G)"   << "lambda_0(nm)" ;
        parMin  << "0.0"      << "0.0" ;
        parMax  << "0.0"      << "0.0" ;
    }
    if( fctChoice == "Lambda_vs_temp"  )
    {
        parVal  << "0.8"       << "5.3"     << "500"       << "10000"  << "200"  ;
        parName << "D0(meV)"   << "Tc(K)"   << "Bapp(G)"   << "Bc2(G)" << "lambda0(nm)" ;
        parMin  << "0.0"       << "0.0"     << "0.0"       << "0.0"    << "0.0";
        parMax  << "0.0"       << "0.0"     << "0.0"       << "0.0"    << "0.0";
    }

    simGUI->energygap_table_pars  ->  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Ignored);
    simGUI->energygap_table_pars  ->  setSelectionMode(QAbstractItemView::NoSelection);

    int nbrows = parName.size();
    QStringList labels;
    labels << tr("Parameter") << tr("Value");

    int nbcols = labels.size();
    simGUI->energygap_table_pars  ->  setColumnCount(nbcols);
    simGUI->energygap_table_pars  ->  setRowCount(nbrows);
    QHeaderView * tableWidget_headers =simGUI->energygap_table_pars -> horizontalHeader();
    simGUI->energygap_table_pars->setHorizontalHeaderLabels(labels);
    tableWidget_headers->setSectionResizeMode(QHeaderView::Stretch);

    for (int row = 0 ; row <nbrows ; row ++ )
    {
        QTableWidgetItem *par=new QTableWidgetItem(parName.at(row));
        par->setFlags( Qt::ItemIsEnabled);
        simGUI->energygap_table_pars->setItem(row,0,par);

        QTableWidgetItem *value=new QTableWidgetItem(parVal.at(row));
        simGUI->energygap_table_pars->setItem(row,1,value);
    }
}

void Simulations::on_energygap_pushButton_start_clicked()
{
    vector<double> par;
    for (int xrow= 0 ; xrow <simGUI->energygap_table_pars->rowCount() ; xrow ++ )
    {
        QTableWidgetItem *valTable =simGUI->energygap_table_pars->item(xrow,1);
        double value = valTable->text().toDouble();
        par.push_back(value);
        QTableWidgetItem *nameTable =simGUI->energygap_table_pars->item(xrow,0);
    }

    QString fctChoice    =simGUI->energygap_comboBox_function   -> currentText();
    QString gapChoice    =simGUI->energygap_comboBox_gap        -> currentText();
    QString sigmaBChoice =simGUI->energygap_comboBox_sigmaB     -> currentText();

    QString fileName     = fctChoice;
    if( fctChoice == "Sigma_vs_temp"  )
        fileName += "_"+gapChoice+"_"+sigmaBChoice;
    if( fctChoice == "Lambda_vs_temp" )
        fileName += "_"+gapChoice;
    if( fctChoice == "Sigma_vs_field"  )
        fileName += "_"+sigmaBChoice;
    fileName += ".dat";

    ClassGAP fct(par,fctChoice.toStdString(),gapChoice.toStdString(),sigmaBChoice.toStdString());
    double  xmin         = 0.0;
    double  xmax         = 0.0;

    QString xlabel, ylabel;
    if( fctChoice == "Sigma_vs_temp"  )
    {
        xlabel = "T(K)";
        ylabel = "sigma";
        xmax   = par.at(1)+1.0;
    }
    if( fctChoice == "Sigma_vs_field"  )
    {
        xlabel = "B(G)";
        ylabel = "sigma";
        xmax   = par.at(0);
    }
    if( fctChoice == "Lambda_vs_temp"  )
    {
        xlabel = "T(K)";
        ylabel = "Lambda";
        xmax   = par.at(1)-0.2;
    }

    //make an ascii file
    QVector< QVector<QString> > files;
    files.resize(1);
    ofstream file(fileName.toStdString().c_str());
    for (double x = xmin + 0.0001; x < xmax ; x += (xmax-xmin)/1000)
    {
        double val = fct(x);
        file <<  x<<"\t"<<   val <<endl;
    }
    file.close();

    //add filenames to plot
    files[0].push_back(fileName);

    //contruct labesl
    QVector<QString> xLabels,yLabels;
    xLabels.push_back(xlabel);
    yLabels.push_back(ylabel);
    Plotting plot;
    //send to plot
    plot.qcp(simGUI->energygap_widget_customPlot,files,xLabels,yLabels);
    //change tab to graph tab
    simGUI->energygap_tabWidget->setCurrentIndex(simGUI->energygap_tabWidget->count()-1);
}

void Simulations::on_energygap_pushButton_export_clicked()
{
    Plotting plot;
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As pdf, jpg, bmp"),
                                                    QDir::currentPath(),
                                                    tr("image file (*.pdf *.jpg *.bmp)"));
    plot.qcp_exportPlot(simGUI->energygap_widget_customPlot,fileName);
}


void Simulations::on_energygap_pushButton_clear_clicked()
{
    Plotting plot;
    plot.qcp_clear(simGUI->energygap_widget_customPlot);
}

void Simulations::on_energygap_pushButton_purge_clicked()
{
    Plotting plot;
    plot.qcp_clear(simGUI->energygap_widget_customPlot);
}
