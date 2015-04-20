/***************************************************************************
*  File: tab_analysis.cpp                                                 *
*  Description: makes contact to the page analysis                         * 
*  Author: Hassan Saadaoui                                                 *
*  e-mail: saadaoui@triumf.ca                                              *
*  $Id: tab_analysis.cpp  2014-12-20 $                                    *
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

#include <iostream>                             //Header file for outputting data
#include <fstream>                              //Header file for outputting data
#include <math.h>                               //Header file for math operations
#include <cassert>
#include <string>
#include <time.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <vector>
#include <sstream>                              // Required for stringstreams
#include <iostream>

#include "constants.h"
#include "tab_analysis.h"
#include "ui_tab_analysis.h"
#include "tab_search.h"
#include "fitting_routine.h"
#include "read_data.h"
#include "plugin_read.h"
#include "plotting.h"
#include "bnmr.h"
#include "ascii.h"

//constructor
Analysis::Analysis(QWidget *parent) :
    QWidget(parent),anaGUI(new Ui::Analysis)
{
    anaGUI->setupUi(this);
    tabWidget_initials();
    comboBox_data();
    comboBox_type();
    comboBox_fct();
    comboBox_fitAs();
    comboBox_fitSettings();
    comboBox_report();
}

//destructor
Analysis::~Analysis()
{
    delete anaGUI;
}

//construct tab widget for initial parameters
void Analysis::tabWidget_initials(void)
{
    iniTabWidget = new QTabWidget(this);
    iniTabWidget->setObjectName("iniTabWidget");
    iniTabWidget->setTabsClosable(true);
    anaGUI->tabWidget->addTab(iniTabWidget,"Initial parameters");
    connect(iniTabWidget,SIGNAL(tabCloseRequested(int)),
            this,SLOT(iniTabWidget_removeTab(int)));
    connect(iniTabWidget,SIGNAL(tabCloseRequested(int)),
            this,SLOT(iniTabWidget_removeTab(int)));
}

//kill tabs
void Analysis::iniTabWidget_removeTab(int index)
{
    iniTabWidget->removeTab(index);
    anaGUI->comboBox_fcn->setCurrentIndex(0);
}

//data as bnmr, ascii...prototype for adding more types
void Analysis::comboBox_data()
{
    QStringList Args;
    Args<<"BNMR"<<"ASCII";
    anaGUI->comboBox_dataType->clear();
    anaGUI->comboBox_dataType->addItems(Args);
}

//if the user changes index of datatype combobox, do this
void Analysis::on_comboBox_dataType_currentIndexChanged(const QString &arg1)
{
    comboBox_type();
}

//Create combo of asymmetry options for bnmr type
void Analysis::comboBox_type()
{
    QStringList Args;
    if (anaGUI->comboBox_dataType->currentText()=="BNMR")
    {
        bnmr_optionsVisibility(true);
        Args<<"Asy"  << "Asy+"  << "Asy-"  << "AsyForR" << "AsyBorL"
           <<"AsyN" << "AsyN+"  << "AsyN-" << "ForR+"   << "BorL+"
          <<"ForR-"<< "BorL-"<< "NF+"<< "NB+" << "NF-" <<"NB-";
    }
    if (anaGUI->comboBox_dataType->currentText()=="ASCII")
    {
        bnmr_optionsVisibility(false);
        Args<<"x y" <<"x y dy"<<"x dx y dy"<<"x dx y dy1 dy2"
           <<"x dx1 dx2 y dy"<<"x dx1 dx2 y dy1 dy2";
    }
    anaGUI->comboBox_plot->clear();
    anaGUI->comboBox_plot->addItems(Args);
}

//show or hide specific items related to bnmr data
void Analysis::bnmr_optionsVisibility(bool choice)
{
    anaGUI->comboBox_1fscans->setVisible(choice);
    anaGUI->lineEdit_killscans->setVisible(choice);
    anaGUI->label_1f->setVisible(choice);
    anaGUI->label_kill1f->setVisible(choice);
    anaGUI->label_2e->setVisible(choice);
    anaGUI->comboBox_2eoptions->setVisible(choice);
}

//fit as single or global
void Analysis::comboBox_fitAs()
{
    anaGUI->comboBox_fitAs->clear();
    QStringList Args;
    Args <<"Single"<<"Global";
    anaGUI->comboBox_fitAs->addItems(Args);
}

//functions loaded from the plugings
void Analysis::comboBox_fct()
{
    anaGUI->comboBox_fcn->clear();
    function library(fctPath);//("/opt/bnmroffice/fct/");
    QStringList fctList;
    fctList<<"Select:";
    fctList<< library.getLibrariesInDir();
    fctList.replaceInStrings(".so","");
    fctList.replaceInStrings("lib","");
    fctList<<">>>>>>Create new<<<<<<";
    fctList<<">>>>>>Update<<<<<<<<<";
    anaGUI->comboBox_fcn->addItems(fctList);
    anaGUI->comboBox_fcn->setCurrentIndex(0);
}

//data as bnmr, ascii...prototype for adding more types
void Analysis::comboBox_report()
{
    QStringList Args;
    Args<<"Rewrite"<<"Append";
    anaGUI->comboBox_report->clear();
    anaGUI->comboBox_report->addItems(Args);
}

//Error routine
void Analysis::comboBox_fitSettings()
{
    anaGUI->comboBox_fitSettings->clear();
    QStringList Args;
    Args <<"Migrad"<<"Minos";
    anaGUI->comboBox_fitSettings->addItems(Args);
}

//output table ouptions, invoked with double click on headers.
void Analysis::on_tabWidget_tabBarDoubleClicked(int index)
{
    settings();
}

//if the fit option changes
void Analysis::on_comboBox_fitAs_currentIndexChanged(const QString &arg)
{
    fitAsGlobal = anaGUI->comboBox_fitAs->currentIndex();
    //if (anaGUI->comboBox_fcn->count()>3)
    //clearVectors();
    inputTable_set();
    anaGUI->tabWidget->setCurrentIndex(0);
}

//reads the graphics options: binning, xmin,xmax,ymin,ymax;
void Analysis::read_graphics()
{
    QStringList graphics = anaGUI->lineEdit_graphics->text().split(QRegExp(","));
    bin   = 1;
    x_min = -INFINITY;
    x_max =  INFINITY;
    y_min = -INFINITY;
    y_max =  INFINITY;

    if (graphics.size() >= 1 && !graphics.at(0).isEmpty() )
    {
        bin  = graphics.at(0).toInt();
        if (graphics.size() >= 2 && !graphics.at(1).isEmpty())
        {
            x_min = graphics.at(1).toDouble();
            if (graphics.size() >= 3 && !graphics.at(2).isEmpty())
            {
                x_max = graphics.at(2).toDouble();
                if (graphics.size() >= 4 && !graphics.at(3).isEmpty())
                {
                    y_min = graphics.at(3).toDouble();
                    if (graphics.size() >= 5 && !graphics.at(4).isEmpty())
                        y_max = graphics.at(4).toDouble();
                }
            }
        }
    }

    if (x_min >= x_max || y_min >= y_max )
    {
        QMessageBox::warning(this, tr("Error"),
                             tr("The value of 'max' must be higher than 'min'....Doh!"));
    }
    //qDebug() <<bin<<x_min<<x_max<<y_min<<y_max;
}

//signal emitted when the user clicks on browsing runs button.
void Analysis::on_toolButton_runs_clicked()
{
    QDir dir(QDir::currentPath());
    if(lastRunsDir.isEmpty())
        lastRunsDir = QDir::currentPath();
    QStringList files = QFileDialog::getOpenFileNames(this,"Open Files to fit",lastRunsDir,
                                                      "Data file(*.db *.txt *.fit *dat *.inf *list)");
    QString output="";
    if (files.size() >0)
        lastRunsDir =   QFileInfo(files.at(0)).absolutePath();
    else
        lastRunsDir = QDir::currentPath();
    for(short i=0; i<files.size();i++)
    {
        output += dir.relativeFilePath(files.at(i))+",";
    }
    if(!output.isEmpty())
    {
        output.truncate(output.size()-1);
        anaGUI->lineEdit_data  -> setText(output);
    }
}

//signal emitted when the user clicks on browsing report button.
void Analysis::on_toolButton_report_clicked()
{
    QDir dir(QDir::currentPath());
    if(lastReportDir.isEmpty())
        lastReportDir = QDir::currentPath();
    QString output = QFileDialog::getOpenFileName(this,"Open Files to fit",lastReportDir,
                                                  "Data file(*.db *.txt *.fit)");
    lastReportDir =   QFileInfo(output).absolutePath();
    if(!output.isEmpty())
        anaGUI->lineEdit_report  -> setText(dir.relativeFilePath(output));
}

//To read the ui data options. Here you can add functions to handle any
//type of data. The function has to return the list of files (which
//exist as ascii) and headers(strings) and headervalues(double) of each run.
bool Analysis::fileList()
{
    runList.clear();
    read_graphics();
    headers.clear();
    headerValue.clear();
    dataFormat.clear();
    subType.clear();
    subType << anaGUI->comboBox_plot->currentText();

    QString dataType = anaGUI->comboBox_dataType->currentText();
    int  formatIndex = anaGUI->comboBox_plot->currentIndex();
    input = anaGUI->lineEdit_data-> text();
    //read the ascii files
    if ( dataType == "ASCII" )
    {
        //This is defined in ascii.cpp
        if (input.contains(":") ||input.contains("|")
                || input.contains("@") ||input.contains("#")
                || input.contains(";") ||input.contains("(")
                || input.contains(";") ||input.contains(")"))
        {
            QMessageBox::warning(this,tr("Error"),
                                 tr("None of these special characters are allowed \":;|<>?*&^%$#@!()\".\n"
                                    "Please update the filenames accordingly.\n"));
        }
        else
        {
            switch (formatIndex)
            {
            case 0: dataFormat<<1<<1; break;//xy
            case 1: dataFormat<<1<<2; break;//xydy
            case 2: dataFormat<<2<<2; break;//x dx y dy
            case 3: dataFormat<<2<<3; break;//x dx y dy1 dy2
            case 4: dataFormat<<3<<2; break;//x dx1 dx2 y dy
            case 5: dataFormat<<3<<3; break;//x dx1 dx2 y dy1 dy2
            }
            //this is defined in ascii.cpp
            ASCII  asci(input,subType,bin,x_min,x_max);
            runList = asci.files(headers,headerValue);
        }
    }
    //read the bnmr files
    else if ( dataType == "BNMR" )
    {
        dataFormat.push_back(1);
        dataFormat.push_back(2);
        int     sum = anaGUI->comboBox_1fscans->currentIndex();
        int     e2 = anaGUI->comboBox_2eoptions->currentIndex();
        QString kill = anaGUI->lineEdit_killscans->text();
        subType.clear();
        subType << QString::number(formatIndex);
        //This is defined in bnmr.cpp
        BNMR bnmr(input,subType,bin,x_min,x_max,sum,e2,kill);
        runList = bnmr.bNMRfiles(headers,headerValue);
    }
    else
        runList.clear();

    bool allFilesExist  = data_exists();

    nbFiles     = runList.size();

    return allFilesExist;
}

//check all files exist
bool Analysis::data_exists(void)
{
    bool state = false;
    for (short f = 0; f < runList.size(); f++)
    {
        QFile file(runList.at(f));
        if (!file.exists() || file.size() <= 0)
        {
            state = false;
            QMessageBox::warning(this,tr("Error"),
                                 tr("The file \"%1\" is not found").arg(runList.at(f)));
            break;
        }
        else
            state = true;
    }
    return state;
}
//signal emitted when the user changes the fct
void Analysis::on_comboBox_fcn_currentIndexChanged(const QString & library)
{
    fctState = false;

    if ( anaGUI->comboBox_fcn->count()>3)
    {
        if (library == ">>>>>>Create new<<<<<<")
        {
            QFile::copy(":/files/newFunction.cpp","newFunction.cpp");
            bool openState = QDesktopServices::openUrl(QUrl("newFunction.cpp"));
            if (openState == false)
            {
                QMessageBox::warning(this,tr("Error"),
                                     tr("Cannot load the template newFunction.cpp\n"));
            }
        }
        else if (library == ">>>>>>Update<<<<<<<<<")
        {
            comboBox_fct();
        }
        else if (library != "Select:")
        {
            loadLibrary(library);
            if (fctState)
            {
                bool filesExist = fileList();
                if(filesExist)
                {
                    parameters_initialization();
                    inputTable_set();
                    anaGUI -> comboBox_fcn
                            ->setItemData(anaGUI->comboBox_fcn->currentIndex(),
                                          fctDesc, Qt::ToolTipRole);
                    anaGUI -> tabWidget -> setCurrentIndex(0);
                }
                else
                {
                    QMessageBox::warning(this,tr("Error"),
                                         tr("Data is not loaded/found.\n"
                                            "Must fix data selection first.\n"));
                }
            }
            else
            {
                QMessageBox::warning(this,tr("Error"),
                                     tr("The library function %1"
                                        " is not loaded.\n").arg(library));
            }
        }
    }
}

//read the initial parameters
void Analysis::parameters_initialization()
{
    clearVectors();
    for(int i=0;i<fctPars->parNames().size();i++)
        parLabel.push_back(QString::fromStdString(fctPars->parNames().at(i)));
    parValue    = QVector<double>::fromStdVector(fctPars->parValues());
    parErrNeg   = QVector<double>::fromStdVector(fctPars->parErrors());
    parErrPos   = QVector<double>::fromStdVector(fctPars->parErrors());
    parMin      = QVector<double>::fromStdVector(fctPars->parMins());
    parMax      = QVector<double>::fromStdVector(fctPars->parMaxs());
}

//read the initial parameters
void Analysis::clearVectors(void)
{
    parLabel.clear();
    parValue.clear();
    parErrNeg.clear();
    parErrPos.clear();
    parMin.clear();
    parMax.clear();
    parShare.clear();
    parFix.clear();
}
//template for passing arguments to the table
void Analysis::inputTable_set(void)
{
    bool allFilesExist = fileList();
    if (allFilesExist && fctState && nbParsOfFct <= parLabel.size())
    {
        QTableWidget *inputTable = new QTableWidget(this);
        inputTable->setColumnCount(6);

        QStringList labels;
        labels << "Parameters"<<"Value"<<"Lower limit"<<"Upper limit"<<"Fix"<<"Share";
        inputTable->setHorizontalHeaderLabels(labels);

        QHeaderView * headers = inputTable -> horizontalHeader();
        headers  -> setSectionResizeMode(QHeaderView::Stretch);

        nbFiles     =   runList.size();

        int nbArgs  = (fitAsGlobal && tableGlobal) ? nbFiles : 1;

        inputTable  -> setColumnHidden(5,!fitAsGlobal);

        inputTable  ->  setRowCount(nbParsOfFct*nbArgs);

        QHeaderView *tblHeaders = inputTable -> horizontalHeader();
        tblHeaders ->setSectionResizeMode(QHeaderView::Stretch);

        for (int  f = 0; f < nbArgs  ; f ++)
            for (int  p = 0; p < nbParsOfFct  ; p ++)
            {
                int row = nbParsOfFct *f + p;
                QString label = (f==0)? parLabel.at(p):parLabel.at(p)+QString::number(f+1);
                QTableWidgetItem *tableLabel= new QTableWidgetItem(label);
                tableLabel-> setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
                inputTable -> setItem (row,0, tableLabel);

                QTableWidgetItem *tableValue= new QTableWidgetItem
                        (QString::number(parValue.at(p),'f',4));
                tableValue-> setFlags(Qt::ItemIsEnabled| Qt::ItemIsEditable);
                inputTable -> setItem (row,1, tableValue);

                QTableWidgetItem *tableMin  = new QTableWidgetItem
                        (QString::number(parMin.at(p),'f',4));
                tableMin-> setFlags(Qt::ItemIsEnabled| Qt::ItemIsEditable);
                inputTable -> setItem (row,2, tableMin);

                QTableWidgetItem *tableMax  = new QTableWidgetItem
                        (QString::number(parMax.at(p),'f',4));
                tableMax-> setFlags(Qt::ItemIsEnabled| Qt::ItemIsEditable);
                inputTable -> setItem (row,3, tableMax);

                QTableWidgetItem *fixchBox  = new QTableWidgetItem("");
                fixchBox-> setFlags(Qt::ItemIsEnabled|Qt::ItemIsUserCheckable);
                fixchBox->setCheckState(Qt::Unchecked);
                inputTable->setItem(row,4,fixchBox);

                QTableWidgetItem *sharechBox = new QTableWidgetItem("");
                sharechBox-> setFlags(Qt::ItemIsEnabled|Qt::ItemIsUserCheckable);
                sharechBox->setCheckState(Qt::Unchecked);
                inputTable->setItem(row,5,sharechBox);
            }

        int nbTabs   = iniTabWidget->count();
        QString fct  = anaGUI->comboBox_fcn->currentText();
        QString name = (fitAsGlobal)? fct+".G" : fct;
        iniTabWidget->addTab(inputTable,name);
        iniTabWidget->setTabToolTip(nbTabs,fct);
        iniTabWidget->setCurrentIndex(nbTabs);

        connect(inputTable,SIGNAL(itemClicked(QTableWidgetItem *)),
                this,SLOT(iniTableWidget_itemClicked(QTableWidgetItem *)));
        inputTable->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(inputTable, SIGNAL(customContextMenuRequested(const QPoint&)),
                SLOT(ProvideContexMenu(const QPoint&)));
    }

}
void Analysis::ProvideContexMenu(const QPoint&)
{
    QDialog *dialog = new QDialog;
    dialog->setAttribute(Qt::WA_DeleteOnClose);

    QToolButton *save       = new QToolButton;
    save->setText("Save as a template");
    save->setEnabled(true);
    connect(save,SIGNAL(clicked()),this, SLOT(save_intable()));
    connect(save, SIGNAL(clicked()), dialog, SLOT(close()));

    QToolButton *browse       = new QToolButton;//("Load template");
    browse->setText("Load a template");
    browse->setEnabled(true);
    connect(browse,SIGNAL(clicked()),this, SLOT(load_intable()));
    connect(browse, SIGNAL(clicked()), dialog, SLOT(close()));


    QDialogButtonBox *buttonBox = new QDialogButtonBox(//QDialogButtonBox::Ok|
                                                       QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(reject()));
    connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

    QHBoxLayout *layout = new QHBoxLayout;
    QHBoxLayout *tmplate = new QHBoxLayout;
    tmplate->addWidget(save);
    tmplate->addWidget(browse);
    tmplate->addWidget(buttonBox);
    layout->addLayout(tmplate);
    //setLayout(layout);
    dialog->setLayout(layout);
    dialog->exec();
}

void Analysis::save_intable(void)
{
    if(lastReportDir.isEmpty())
        lastReportDir = QDir::currentPath();
    QString output = QFileDialog::getSaveFileName(this,"Save as a template",lastReportDir,
                                                  "Data file(*.tab)");
    lastReportDir =   QFileInfo(output).absolutePath();

    if(!output.isEmpty())
    {
        QFile file(output);
        file.open(QIODevice::WriteOnly| QIODevice::Truncate);
        QTextStream templ(&file);
        int tab = iniTabWidget->currentIndex();
        QTableWidget *inputTable = static_cast<QTableWidget *>(iniTabWidget->widget(tab));

        templ<<"#Template of initial parameters for "<<iniTabWidget->tabToolTip(tab)<<" \n";
        templ<<"#Created on "<<QDate::currentDate().toString()
            <<" at "<<QTime::currentTime().toString()<< "\n";
        templ<<"#\n";
        templ<<"#\n";
        int tblRows  = inputTable->rowCount(), tblCols = inputTable->columnCount();
        for (int row = 0 ; row < tblRows ; row ++ )
        {
            for (int col = 0 ; col < 4 ; col ++ )
                templ <<  inputTable->item(row,col)->text()<<"\t";
            for (int col = 4 ; col < 6 ; col ++ )
                templ <<  (bool)inputTable->item(row,col)->checkState()<<"\t";
            templ<<endl;
        }
        file.close();
    }
}

void Analysis::load_intable(void)
{
    if(lastReportDir.isEmpty())
        lastReportDir = QDir::currentPath();
    QString input = QFileDialog::getOpenFileName(this,"Save as a template",lastReportDir,
                                                 "Data file(*.tab)");
    lastReportDir =   QFileInfo(input).absolutePath();

    if(!input.isEmpty())
    {
        int tab = iniTabWidget->currentIndex();
        QTableWidget *inputTable = static_cast<QTableWidget *>(iniTabWidget->widget(tab));
        QFile file(input);
        file.open(QIODevice::ReadOnly);
        QTextStream templ(&file);
        int row = 0;

        Qt::CheckState chState;

        while(!templ.atEnd() && row < inputTable->rowCount())
        {
            QVector<QString> inValues = templ.readLine().split(QRegExp("\\s")).toVector();
            if(!inValues.at(0).contains("#") && inValues.size()>=5 && !inValues.at(1).isEmpty())
            {
                for (int col = 0 ; col < 4 ; col ++ )
                    inputTable -> item (row,col)->setText(inValues.at(col));
                for (int col = 4 ; col < 6 ; col ++ )
                {
                    chState =  (inValues.at(col) == "1")? Qt::Checked : Qt::Unchecked;
                    inputTable->item(row,col)->setCheckState(chState);
                }
                row++;
            }
        }
    }
}

//read from the input table
bool Analysis::inputTable_reading()
{
    bool state = false;
    clearVectors();

    if (iniTabWidget->count()>0)
    {
        int tab = iniTabWidget->currentIndex();
        QString library  = iniTabWidget->tabToolTip(tab);
        QTableWidget *inputTable = static_cast<QTableWidget *>(iniTabWidget->widget(tab));

        fitAsGlobal  = (inputTable->isColumnHidden(5))? false : true;
        anaGUI->comboBox_fitAs->setCurrentIndex(fitAsGlobal);

        int fctRows  = ( fitAsGlobal && tableGlobal)? nbFiles : 1;

        int tblRows  = inputTable->rowCount();
        if (tblRows > 0)
        {
            loadLibrary(library);//nbParsOfFct is set here
            if (fctState && tblRows == nbParsOfFct*fctRows)
            {
                int nbReads  = (fitAsGlobal&&tableGlobal)? 1 : nbFiles;

                double minValue,maxValue;
                for (int  rd = 0; rd < nbReads; rd ++)
                {
                    for (int row = 0 ; row < tblRows ; row ++ )
                    {
                        QString label = (rd==0)? inputTable->item(row,0)->text():
                                                 inputTable->item(row,0)->text()
                                                 +QString::number(rd+1);
                        parLabel.push_back(label);
                        parValue.push_back(inputTable->item(row,1)->text().toDouble());
                        parErrNeg.push_back(0.001);
                        parErrPos.push_back(0.001);
                        minValue = inputTable->item(row,2)->text().toDouble();
                        maxValue = inputTable->item(row,3)->text().toDouble();
                        parMin.push_back(minValue);
                        parMax.push_back(maxValue);
                        if(minValue >= maxValue )
                        {
                            QMessageBox::warning(this,tr("Error"),tr("min is >= than max"));
                            goto endloop;
                        }
                        parFix.push_back((bool)inputTable->item(row,4)->checkState());
                        if( fitAsGlobal )
                            parShare.push_back((bool)inputTable->item(row,5)->checkState());
                        else
                            parShare.push_back(0);
                    }
                }
                state = true;
            }
        }
    }
    else
        QMessageBox::warning(this,tr("Error"),tr("No input table is found/loaded"));
endloop:;
    return state;
}

void Analysis::loadLibrary(QString library)
{
    fct = new function(fctPath);
    fctPars = new Parameters;

    fctList.clear();
    fctList<< fct->getLibrariesInDir();
    fctState = fct->getLibraryState("lib"+library);
    if (fctState)
    {
        fctDesc = fct->getLibraryDescription();
        fct->getParameters(*fctPars);
        nbParsOfFct = fctPars->parNames().size();
    }
}

//change the input table depending on single or global options
//void Analysis::on_tableWidget_input_itemClicked(QTableWidgetItem *item)
void Analysis::iniTableWidget_itemClicked(QTableWidgetItem *item)
{
    int clickedCol = item->column();
    int clickedRow = item->row();

    if (clickedCol == 5 && tableGlobal)
    {
        QTableWidget *inputTable = item->tableWidget();
        //this is the clicked index for cases where the user
        //clicks on index higher than the nb of pars.
        int C = clickedRow;

        //find the real index
        if (clickedRow >= nbParsOfFct)
        {
            for (int  f = 0; f < nbFiles; f ++)
                for (int  p = 0; p < nbParsOfFct; p ++)
                    if(clickedRow == nbParsOfFct *f + p)
                        C = p;

        }
        //qDebug() <<clickedCol<<C<<clickedRow;
        Qt::CheckState ok = Qt::Checked;
        Qt::ItemFlags flags =  Qt::ItemIsEnabled| Qt::ItemIsEnabled;

        if (inputTable->item(clickedRow,clickedCol)->checkState()==2)
        {
            flags = Qt::NoItemFlags;
            ok = Qt::Checked;
        }
        else if (inputTable->item(clickedRow,clickedCol)->checkState()==0)
        {
            flags = Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
            ok = Qt::Unchecked;
        }

        inputTable->item(C,5)->setCheckState(ok);
        for (int  f = 1; f < nbFiles; f ++)
        {
            for (int  p = 0; p < nbParsOfFct; p ++)
            {
                int row = nbParsOfFct *f + p;
                inputTable->item(row,4)->setCheckState(Qt::Unchecked);
                if (row == f*nbParsOfFct + C )
                {
                    inputTable->item(row,5)->setCheckState(ok);
                    for (int  col = 0; col < 5; col ++)
                        inputTable->item(row,col)->setFlags(flags);
                }
            }
        }
    }
}

//this function starts when the user clicks on the fitting button.
void Analysis::on_pushButton_start_clicked()
{
    input       = anaGUI->lineEdit_data-> text();
    minimizer   = "Migrad";
    error       = anaGUI->comboBox_fitSettings->currentText();
    report      = anaGUI->lineEdit_report->text();
    fitAsGlobal = anaGUI->comboBox_fitAs->currentIndex();
    bool  filesExist  = fileList();
    bool  inputRead   = inputTable_reading();

    if (fctState && filesExist && inputRead)
    {
        report_header(report);

        readData data(dataFormat,runList,x_min,x_max,y_min,y_max);

        xData = data.getxData();

        yData = data.getyData();

        FIT fit(*fct,minimizer,error,fitAsGlobal,tableSingle,xData,yData);

        fit.PARAMETERS(parValue,parErrNeg,parErrPos,chisq,parMin,parMax,parFix,parShare);

        outputTable_printResults();

        report_write();

        fit_write();

        xmgr_plots(true);
    }
    else
    {
        QMessageBox::warning(this,tr("Error"),
                             tr("Files to fit and/or function \"%1\" are not loaded")
                             .arg(anaGUI->comboBox_fcn->currentText()));
    }
}

//write the header of the report file, contains ind variables and fitting pars.
void Analysis::report_header(QString fileName)
{
    QFile file(fileName);

    if ( !reportAppend || !file.exists() )
    {
        file.open(QIODevice::WriteOnly| QIODevice::Truncate);
        QTextStream report(&file);
        for (short h = 0 ; h < headers.size()  ; h ++ )
        {
            report  << headers.at(h)<<" ";
        }
        for (short p = 0 ; p < nbParsOfFct ; p ++ )
        {
            if (p < parLabel.size())
                report<<parLabel.at(p)<<" ";
        }
        report << "chisq"<<endl;
        file.close();
    }
}

//write the results to the report file, contains ind variables and fitting pars.
void Analysis::report_write()
{
    QFile file(report);
    file.open(QIODevice::WriteOnly| QIODevice::Append);

    //Prepare the file to write
    report_header(report);

    //Now add pars to the report
    QTextStream output(&file);
    for (short r = 0 ; r < nbFiles; r ++ )
    {
        for (short h = 0 ; h < headers.size()  ; h ++ )
        {
            output  << headerValue[r].at(h)<<"\t";//add headers
        }
        for (short p = r*nbParsOfFct ; p < (r+1)*nbParsOfFct; p ++ )
        {
            //Add the parameters of each run to the report;
            output<<parValue.at(p)<<","<< parErrNeg.at(p)<<","<<parErrPos.at(p)<<"\t";
        }
        output 	<< chisq[r]<<endl;//Add chisq.
    }
    file.close();
}

//print the fit results to the table
void Analysis::outputTable_printResults()
{
    QTableWidget *resTable  = new QTableWidget(this);
    int nbCols = nbParsOfFct+2;//2 for colmns for label and chisq
    resTable -> setColumnCount(nbCols);

    QStringList labels;
    labels << "Files ";
    for (int p = 0 ; p < nbParsOfFct ; p ++ )
        labels << parLabel.at(p);
    labels <<"Chisq ";
    resTable->setHorizontalHeaderLabels(labels);

    QHeaderView * headers = resTable -> horizontalHeader();
    headers  -> setSectionResizeMode(QHeaderView::Stretch);
    resTable -> setRowCount(nbFiles);//we make a new table with nb of rows equal to nb of runs

    char l = tableLetter.toLocal8Bit().at(0);
    for (int  run = 0; run <nbFiles; run ++)
    {
        QString runLabel;
        if (tableName == 0)
            runLabel = QFileInfo(runList.at(run)).baseName();
        if (tableName == 1)
            runLabel = QFileInfo(runList.at(run)).fileName();
        if (tableName == 2)
            runLabel = QFileInfo(runList.at(run)).canonicalFilePath();

        QTableWidgetItem *colName = new QTableWidgetItem(runLabel);
        resTable->setItem(run,0, colName);
        //cols of parameters
        for (int  col = 0; col <nbParsOfFct  ; col ++)
        {
            int p = nbParsOfFct*run + col;
            QString input;
            if( anaGUI->comboBox_fitSettings->currentText() == "Minos" && tableError == 2)
                input = QString::number(parValue.at(p),l,tablePrecision)+"["+
                        QString::number(parErrNeg.at(p),l,tablePrecision)+",+"+
                        QString::number(parErrPos.at(p),l,tablePrecision)+"]";
            else
            {
                if (tableError == 1 || tableError == 2)
                    input = QString::number(parValue.at(p),l,tablePrecision)
                            +QString::fromUtf8("\u00B1")+
                            QString::number(parErrPos.at(p),l,tablePrecision);
                else
                    input = QString::number(parValue.at(p),l,tablePrecision);
            }
            QTableWidgetItem *parCols = new QTableWidgetItem(input);
            resTable -> setItem(run,col+1,parCols);
        }
        //chisq column
        QTableWidgetItem *chisqCol =
                new QTableWidgetItem(QString::number(chisq.at(run),l,tablePrecision));
        if (chisq.at(run)==0.0)
            chisqCol ->setBackground(QBrush(QColor(Qt::red)));//set cell red if fit didn't converge
        resTable -> setItem(run,nbCols-1, chisqCol);
    }
    int nbTabs =anaGUI -> tabWidget->count();
    anaGUI -> tabWidget->setTabsClosable(true);
    anaGUI -> tabWidget
            ->addTab(resTable,QString(tr("Results %1").arg(QString::number(nbTabs))) );
    anaGUI -> tabWidget
            ->setTabToolTip(nbTabs-1,QString(tr("Results %1").arg(QString::number(nbTabs))));
    anaGUI -> tabWidget
            ->setCurrentIndex(nbTabs);
}

void Analysis::on_tabWidget_tabCloseRequested(int index)
{
    //kill only results tables, not inpput table.
    if (index >0)
        anaGUI -> tabWidget->removeTab(index);
}

//write the results in a report
void Analysis::fit_write(void)
{
    std::vector<double> par;

    for (short f= 0 ; f < nbFiles; f ++ )
    {
        QString fileName = QFileInfo(runList.at(f)).baseName()+".fit";

        QFile file(fileName);
        file.open(QIODevice::WriteOnly| QIODevice::Truncate);
        QTextStream fit(&file);

        par.clear();
        for(short p = f*nbParsOfFct; p < (f+1)*nbParsOfFct ; p++)
        {
            par.push_back(parValue.at(p));
        }

        int nbRows =  xData[f][0].size();
        double min =  (min <= x_min)? x_min : xData[f][0][0];
        double max =  (max >= x_max)? x_max : xData[f][0][nbRows-1];

        for(double x = min; x <= max; x += (max - min)/(10*nbRows))
        {
            fit << x << "\t" <<  (*fct)(x,par)<<"\n";
        }
        file.close();
    }
}

//xmgr plotting
void Analysis::xmgr_plots(bool fit)
{
    //needed for plotting the graphs
    QVector<QString> fileNames,xLabels,yLabels,legends,titles;
    QVector<int>     xformat,yformat,nbSetsPerGraph;
    QString          basename,rawData,fitData;
    //specifies the arguments of xmgr graphs
    for ( int i = 0; i < nbFiles ; i++ )
    {
        nbSetsPerGraph << 1;
        rawData  =  runList.at(i);
        basename =  QFileInfo(rawData).baseName();
        xformat  << dataFormat.at(0);
        yformat  << dataFormat.at(1);
        legends  << basename;
        xLabels  << "";
        yLabels  << basename;
        titles   << basename;
        fileNames<< rawData ;
        if (fit)
        {
            xformat  << 1;
            yformat  << 1;
            nbSetsPerGraph[i]=2;
            fitData  =  QFileInfo(rawData).baseName()+".fit";
            fileNames<< fitData;
            legends  << basename;
        }
    }
    //send signals to xmgr
    Plotting plot;
    plot.xmgr(nbFiles,nbSetsPerGraph,xformat,yformat,xLabels,yLabels,fileNames,legends,titles);
}

//plot data
void Analysis::on_pushButton_plot_clicked()
{
    bool filesExist  = fileList();
    if (filesExist)
    {
        xmgr_plots(false);
    }
    else
    {
        QMessageBox::warning(this,tr("Error"),tr("No files are found"));
    }
}

//write report options depending on the UI selection
void Analysis::on_comboBox_report_currentIndexChanged(int index)
{
    reportAppend = index;
}

//do the fit, and stop whatever is going on
void Analysis::on_pushButton_abort_clicked()
{
    qDebug()<<"Abort not implemented yet";
    cout<<"Abort not implemented yet";
}

//send report to the next page
void Analysis::on_pushButton_results_clicked()
{
    QString fileName = anaGUI->lineEdit_report->text();
    //copy the user selection (checked runs) and paste them into data lineedit in the analysis tab.
    foreach(QObject *obj, this->parent()->children())
    {
        if(obj->objectName()=="Results")
        {
            QLineEdit* button = obj->findChild<QLineEdit*>("lineEdit_filename");
            button->setText(fileName);
        }
    }
    //find the objectname of the tabwidget
    QObject *obj = this->parent()->parent()->parent()->findChild<QTabWidget*>("pagesTabWidget");
    if (obj != NULL)
    {
        QTabWidget *tab = qobject_cast<QTabWidget*>(obj);
        if (tab->count() > 2)
            tab->setCurrentIndex(2);
    }
}

void Analysis::on_comboBox_fitSettings_currentIndexChanged(const QString &arg)
{
    if (arg == "Settings" )
    {
        settings();
    }
}

QString Analysis::tableLetter = "f";

int Analysis::tablePrecision  = 4;

int Analysis::tableError      = 2;

int Analysis::tableName       = 0;

bool Analysis::tableGlobal  = true;

bool Analysis::tableSingle  = false;

bool Analysis::tableTemplate  = false;

void Analysis::settings(void)
{
    tableDialog dialog;
    dialog.exec();
    tableLetter    = dialog.getLetter();
    tablePrecision = dialog.getPrecision();
    tableName      = dialog.getNameType();
    tableError     = dialog.getErrorType();
    tableGlobal    = dialog.getGlobal();
    tableSingle    = dialog.getSingle();
}

//output table options
tableDialog::tableDialog(QWidget *parent): QDialog(parent)
{
    createLayout();
}


void tableDialog::createLayout()
{
    int p = Analysis::tablePrecision;

    int n = Analysis::tableName;

    int r = Analysis::tableError;

    QString l = Analysis::tableLetter;

    bool glb  = Analysis::tableGlobal;

    bool sgl  = Analysis::tableSingle;

    QLabel *tableTitle  = new QLabel(tr("Output table:"));
    QHBoxLayout *titleLayout = new QHBoxLayout;
    titleLayout->addWidget(tableTitle);

    nameLabel  = new QLabel(tr("Filenames label"));
    nameComboBox  = new QComboBox;
    QStringList names;
    names <<"Base name"<<"Short name"<<"Fullpath name";
    nameComboBox->addItems(names);
    nameComboBox->setCurrentIndex(n);
    nameComboBox->setSizePolicy(QSizePolicy::Expanding,
                                QSizePolicy::Preferred);

    QHBoxLayout *nameLayout = new QHBoxLayout;
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(nameComboBox);

    preLabel  = new QLabel(tr("Parameters precision"));
    preSpinBox  = new QSpinBox;
    preSpinBox->setValue(p);
    preSpinBox->setSizePolicy(QSizePolicy::Expanding,
                              QSizePolicy::Preferred);

    letComboBox  = new QComboBox;
    QStringList letters;
    letters <<"f"<<"e"<<"E";
    letComboBox->addItems(letters);
    letComboBox->setCurrentText(l);
    letComboBox->setSizePolicy(QSizePolicy::Expanding,
                               QSizePolicy::Preferred);
    QHBoxLayout *preLayout = new QHBoxLayout;
    preLayout->addWidget(preLabel);
    preLayout->addWidget(preSpinBox);
    preLayout->addWidget(letComboBox);

    errLabel = new QLabel(tr("Number of errors"));
    errComboBox  = new QComboBox;
    QStringList errors;
    errors <<"None"<<"One"<<"Both";
    errComboBox->addItems(errors);
    errComboBox->setCurrentIndex(r);
    errComboBox->setSizePolicy(QSizePolicy::Expanding,
                               QSizePolicy::Preferred);

    QHBoxLayout *errLayout = new QHBoxLayout;
    errLayout->addWidget(errLabel);
    errLayout->addWidget(errComboBox);

    QVBoxLayout *title = new QVBoxLayout;
    title->addLayout(titleLayout);
    title->addStretch();
    QVBoxLayout *options = new QVBoxLayout;
    options->addLayout(nameLayout);
    options->addLayout(preLayout);
    options->addLayout(errLayout );
    QHBoxLayout *outlayout = new QHBoxLayout;
    outlayout->addLayout(title);
    outlayout->addLayout(options);

    QLabel *intableTitle  = new QLabel(tr("Input table:    "));
    QHBoxLayout *intitleLayout = new QHBoxLayout;
    intitleLayout->addWidget(intableTitle);

    globalBox = new QCheckBox("Global: show parameters for all runs");
    globalBox->setCheckable(true);
    globalBox->setChecked(glb);
    globalBox->setEnabled(true);
    connect(globalBox,SIGNAL(toggled(bool)),this, SLOT(setNewValues()));

    singleBox = new QCheckBox("Single: start with fit parameters of preceding run");
    singleBox->setCheckable(true);
    singleBox->setChecked(sgl);
    singleBox->setEnabled(true);
    connect(singleBox,SIGNAL(toggled(bool)),this, SLOT(setNewValues()));

    QVBoxLayout *intitle = new QVBoxLayout;
    intitle->addLayout(intitleLayout);
    intitle->addStretch();

    QVBoxLayout *inoptions = new QVBoxLayout;
    inoptions->addStretch();
    inoptions->addWidget(globalBox);
    inoptions->addWidget(singleBox);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(setNewValues()));
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(reject()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(keepValues()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QHBoxLayout *inlayout = new QHBoxLayout;
    inlayout->addLayout(intitle);
    inlayout->addLayout(inoptions);

    QHBoxLayout *oklayout = new QHBoxLayout;
    oklayout->addWidget(buttonBox);

    QVBoxLayout *mainlayout = new QVBoxLayout;
    mainlayout->addLayout(inlayout);
    mainlayout->addSpacing(50);
    mainlayout->addLayout(outlayout);
    mainlayout->addSpacing(50);
    mainlayout->addLayout(oklayout);
    setLayout(mainlayout);
}

void tableDialog::keepValues(void)
{
    pre       = Analysis::tablePrecision;
    nameType  = Analysis::tableName;
    letter    = Analysis::tableLetter;
    errorType = Analysis::tableError;
    global    = Analysis::tableGlobal;
    single    = Analysis::tableSingle;
}

void tableDialog::setNewValues(void)
{
    pre       = preSpinBox->value();
    nameType  = nameComboBox->currentIndex();
    letter    = letComboBox->currentText();
    errorType = errComboBox->currentIndex();
    global    = globalBox->checkState();
    single    = singleBox->checkState();
}
int tableDialog::getPrecision(void)
{
    return pre;
}

int tableDialog::getNameType(void)
{
    return nameType;
}

int tableDialog::getErrorType(void)
{
    return errorType;
}

QString tableDialog::getLetter(void)
{
    return letter;
}

bool tableDialog::getGlobal(void)
{
    return global;
}

bool tableDialog::getSingle(void)
{
    return single;
}


