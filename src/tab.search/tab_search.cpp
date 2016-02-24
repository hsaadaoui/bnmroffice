/***************************************************************************
*  File: tab_search.cpp                                                    *
*  Description: search engine for looking up specific runs in .msr files.  *
*  Author: Hassan Saadaoui                                                 *
*  e-mail: saadaoui@triumf.ca                                              *
*  $Id: tab_search.cpp 2014-12-20 $                                        *
*  Copyright (C) 2014 by Hassan Saadaoui                                   *
***************************************************************************/
/***************************************************************************
    This file is part of BnmrOffice.

    BnmrOffice is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) all later version.

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
DATE        AUTHOR  DESCRIBE
2014-12-20  HASSAN  Added more button for more experiment variables
2014-12-20  HASSAN  Added Temp, Energy and field lineEdits
2016-02-20  Hassan  Fixed bugs of variables output.
2016-02-20  Hassan  Added checks for units.
2016-02-20  Hassan  Modified the structure of the logs.txt file.
2016-02-21  Hassan  Added labels_variables.
***************************************************************************/

//C++ libraries
#include <cassert>
#include <string>
#include <stddef.h>
#include <vector>
using    namespace std;
//QT libraries
#include <QString>
#include <QProcess>
#include <QHeaderView>
#include <QLatin1String>
#include <QString>
#include <QFileInfo>
#include <QtGui>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QCheckBox>
#include <QSignalMapper>
#include <QWidget>
#include <QDialog>
#include <QStackedWidget>

//User written header
#include "constants.h"
#include "mud.h"
#include "bnmr.h"
#include "tab_search.h"
#include "ui_tab_search.h"
#include "tab_analysis.h"

Search::Search(QWidget *parent) :
    QWidget(parent),
    schGUI(new Ui::Search)
{
    schGUI->setupUi(this);
    experimentCombo();
    modeCombo();
}

Search::~Search()
{
    delete schGUI;
}


void Search::on_pushButton_plot_clicked()
{
    //copy the user selection (checked runs) and paste them into data lineedit in the analysis tab.
    QString runs = checked_runs();
    foreach(QObject *obj, this->parent()->children())
    {
        if(obj->objectName()=="Analysis")
        {
             QLineEdit* button = obj->findChild<QLineEdit*>("lineEdit_data");
            button->setText(runs);
        }
    }
    //find the objectname of the tabwidget
    QObject *obj = this->parent()->parent()->parent()->findChild<QTabWidget*>("pagesTabWidget");
    QTabWidget *tab = qobject_cast<QTabWidget*>(obj);
    tab->setCurrentIndex(1);
}

void Search::valueChanged(int index)
{
    index = 2;
    emit change(index);
}

void Search::experimentCombo()
{
    QStringList expArgs;
    expArgs<<"bnmr"<< "bnqr"<<"all";
    expOptions = new QStandardItemModel(expArgs.size(), 1);
    for (int r = 0; r < expArgs.size(); ++r)
    {
        QStandardItem *item = new QStandardItem(expArgs.at(r));
        expOptions->setItem(r, 0, item);
    }
    schGUI->comboBox_exp->setModel(expOptions);
}

void Search::modeCombo()
{
    QStringList modeArgs;
    modeArgs << "1f" << "2e" << "20" << "1n"<<"all";
    modeOptions = new QStandardItemModel(modeArgs.size(), 1);
    for (int r = 0; r < modeArgs.size(); ++r)
    {
        QStandardItem *item = new QStandardItem(modeArgs.at(r));
        modeOptions->setItem(r, 0, item);
    }
    schGUI->comboBox_mode->setModel(modeOptions);
}


QString Search::msr_archive_path(QString exp,int year)
{
    QString msr_dir = ( exp == "bnmr" )?bnmrDataPath: bnqrDataPath;
    return  msr_dir+QString::number(year);
}

void Search::on_pushButton_search_clicked()
{
    results();
}

void Search::on_pushButton_morelogs_clicked()
{
    Edit_logs();
}

void Search::on_pushButton_checkall_clicked()
{
    checkall();
}


QStringList logLabel,logSymbol,logPath,logBool,tickedlogLabel,tickedlogSymbol,tickedlogPath;
void Search::readLogs()
{
    //The name of the file that contains the bnmr/bnqr names/path of logged variables.
    QString dataFile="bnmr-bnqr-logs.txt";
    //First check if it exist under the user directory,
    //means the user has made a local copy.
    //If not just load the file that is part
    //of the executable, part of resources.qrc.
    if (!QFile(dataFile).open(QIODevice::ReadOnly | QIODevice::Text) )
    {
        dataFile = ":/files/bnmr-bnqr-logs.txt";
    }
    QFile logData(dataFile);
    logData.open(QIODevice::ReadOnly);
    QTextStream inDataFile(&logData);
    if (!logData.exists())
    {
        QMessageBox::warning(this, tr("Error: "),
                             tr("File of bnmr/bnqr of names/path"
                                "of logged variables is not found.\n"
                                "Please copy/create a new file in"
                                "your working directory\n"
                                "Note: a copy of bnmr-bnqr-logs.txt "
                                "exists under bnmroffice/misc/"));
    }
    else
    {
        //Reads the first line of the file which should contain the labels.
        inDataFile.readLine();
        QStringList Args;
        logLabel.clear();
        logSymbol.clear();
        while(!inDataFile.atEnd())
        {
            Args = inDataFile.readLine().split(QRegExp("\\s+"));
            logLabel <<Args.at(0);
            logSymbol<<Args.at(1);
            logPath  <<Args.at(2);
            //logUnit  <<Args.at(4);
            logBool  <<"no";
        }
    }
}

QStringList Search::listDefaultHeaders(void)
{
    QStringList list;
    list  <<"Run" <<"Year"<<"Type" <<"t(min)"<<"T(K)" <<"E(keV)" <<"B(G)"<<"Title";
    return list;
}

bool Search::findLabel(QString name,QString var)
{
    QStringList labels = var.split( "," );
    for (int m = 0; m < labels.size(); m++)
        if (name.contains(labels[m],Qt::CaseInsensitive))
             return true;
    return false;
}


QStringList Search::search_indvar(int run, int year)
{
    QStringList stringlist;
    QString exp = "bnmr" ;
    if(run > 45000)
        exp = "bnqr";

    double time = 0.0, field = 0.0, energy = 0.0, temp = 0.0;
    double dfield = 0.0, denergy = 0.0, dtemp = 0.0;
    double v_na_cell = 0.0, platform_bias = 0.0, initial_energy = 0.0;
    QString title="title",type="30";
    int nbIndVar = 0;

    QStringList tickedvars;
    for (int t = 0; t< tickedlogPath.size(); t++)
        tickedvars <<"0";

    QString  file         =  msr_archive_path(exp,year)+ "/0"+QString::number(run)+".msr";
    if(QFileInfo(file).exists())
    {
        runDescription getdesc =  rundesc((char *)file.toStdString().c_str());
        time          =  floor(getdesc.ElapsedSec / 60.);
        type          =  QString::fromStdString(getdesc.Insert);
        title         =  QString::fromStdString(getdesc.Title);
        nbIndVar      =  getdesc.Nb_ind_vrbls;


        for (int i = 0; i< nbIndVar; i++)
        {
            QString name = QString::fromStdString(getdesc.Name[i]);
            double mean  = getdesc.Mean[i];

            if (findLabel(name,labels_BNMR_Field))
            {
                double unit = 10000.0;//10000 for Tesla->Gauss conversion
                if (mean >MaxHighField)//the field is given in gauss!
                    unit = 1.0;//no conversion needed
                field =  mean*unit;
            }
            else if (findLabel(name,labels_BNQR_Helmotz))
            {
                field =  0.175 + 2.2131*mean;//calibration equation
            }
            else if (findLabel(name,labels_Platforms_Bias))
            {
                double unit = 1.0;//Default unit is kV
                if (mean > MaxBeamEnergy)//Energy must be given in Volts
                        unit = 1000.0;//1000 for V to kV conversion
                platform_bias	=  mean/unit;
            }
            else if (findLabel(name,labels_Beam_Energy))
            {
                double unit = 1.0;//Default unit is kV
                if (mean > MaxBeamEnergy)//Energy must be given in Volts
                        unit = 1000.0;//1000 for V to kV conversion
                initial_energy	=  mean/unit;
            }
            else if (findLabel(name,labels_NaCell_Bias))
            {
                v_na_cell   	=  mean/1000;//1000 for V to kV conversion
            }
            else if (findLabel(name,labels_Cryo_Temp))
            {
                temp = mean;
            }

            for (int t = 0; t< tickedlogPath.size(); t++)
            {
                if (findLabel(name,tickedlogPath[t]))
                   tickedvars[t] = QString::number(mean,'f',2);
            }
        }

        //Energy in kV...maybe! If .msr file has the correct units. Else
        if (initial_energy == 0)
            initial_energy = DefaultBeamEnergy ;//cout << "Warning: the initial energy was not logged in.\n";

        //If na cell bias cannot be found use the default value (0.1kV)
        if ( v_na_cell     == 0)
            v_na_cell = DefaultNaCellBias;
        energy	=  initial_energy - platform_bias - v_na_cell;
    }

    //write the list of ind variables
    stringlist << QString::number(run)
               << QString::number(year)
               << type
               << QString::number(time)
               << QString::number(temp,'f',2)
               << QString::number(energy,'f',1)
               << QString::number(field,'f',0)
               << tickedvars
               << title;
    return stringlist;
}

void Search::read_query(QVector<QString> &Values)
{
    Values.clear();
    Values.push_back(schGUI->lineEdit_key       -> text());
    Values.push_back(schGUI->comboBox_exp->currentText());
    Values.push_back(schGUI->comboBox_mode->currentText());
    Values.push_back(schGUI->spinBox_yearFrom   -> text());
    Values.push_back(schGUI->spinBox_yearTo     -> text());
    Values.push_back(schGUI->spinBox_runFrom    -> text());
    Values.push_back(schGUI->spinBox_runTo      -> text());
    Values.push_back(schGUI->spinBox_timeFrom   -> text());
    Values.push_back(schGUI->spinBox_timeTo     -> text());
    Values.push_back(schGUI->lineEdit_minTemp   -> text());
    Values.push_back(schGUI->lineEdit_maxTemp   -> text());
    Values.push_back(schGUI->lineEdit_minEnergy -> text());
    Values.push_back(schGUI->lineEdit_maxEnergy -> text());
    Values.push_back(schGUI->lineEdit_minField  -> text());
    Values.push_back(schGUI->lineEdit_maxField  -> text());
}

QVector< QVector<QString> > Search::engine_bnmr()
{
    QVector< QVector<QString> > runs_values_of_vars;

    QVector<QString> Values;
    read_query(Values);
    QString intitle  = Values.at(0), inexp   = Values.at(1), inmode = Values.at(2);
    int     yearFrom = Values.at(3).toInt(), yearTo  = Values.at(4).toInt();
    int     minRun   = Values.at(5).toInt(), maxRun  = Values.at(6).toInt();
    int     minTime  = Values.at(7).toInt(), maxTime = Values.at(8).toInt();
    double  minTemp  = Values.at(9).toDouble(), maxTemp = Values.at(10).toDouble();
    double  minEnergy = Values.at(11).toDouble(), maxEnergy = Values.at(12).toDouble();
    double  minField  = Values.at(13).toDouble(), maxField  = Values.at(14).toDouble();

    QStringList keywords;
    keywords= intitle.split(QRegExp("\\s+"));

    QStringList expType,modeType;
    expType  << inexp;
    if(inexp == "all")
    {
        expType.clear();
        for (int i = 0; i < schGUI -> comboBox_exp->count()-1; i++)
            expType << schGUI -> comboBox_exp->itemText(i);
    }
    modeType<<inmode;

    //Read all logs;
    readLogs();
    QStringList dftlist= listDefaultHeaders();
    int nb_total_of_vars = dftlist.size() + tickedlogPath.size();
    int nb_runs          = 0;

    foreach(QString exp, expType )
    {
        foreach(QString type, modeType )
        {
            for ( int year = yearFrom; year <= yearTo; year ++)
            {
                QDir dir(msr_archive_path(exp,year));
                dir.setFilter(QDir::Files|QDir::NoDotAndDotDot|QDir::CaseSensitive);
                QStringList filters;
                if (exp == "bnmr")
                    filters << "040*.msr"<< "041*.msr"<<"042*.msr"<<"043*.msr"<<"044*.msr";
                if (exp == "bnqr")
                    filters << "045*.msr"<< "046*.msr"<<"047*.msr"<<"048*.msr"<<"049*.msr";
                dir.setNameFilters(filters);
                dir.setSorting( QDir::Name);
                QFileInfoList list = dir.entryInfoList();

                QFileInfo fileInfo;
                QString inRun, inType="all", inTitle;
                int  keyfound = keywords.size();

                for (int i = 0;  i < list.size(); i ++)
                {
                    fileInfo  =  list.at(i);
                    inRun     =  fileInfo.baseName().replace(0,1,"");

                    if (inRun.toInt() >= minRun && inRun.toInt() <=maxRun)
                    {
                        //to keep the program responsive
                        qApp->processEvents();

                        QStringList   stringlist = search_indvar(inRun.toInt(),year);
                        if(type != "all" )
                        {
                            inType    =  stringlist.at(2);
                        }
                        double inTime    = stringlist.at(3).toDouble();
                        double inTemp    = stringlist.at(4).toDouble();
                        double inEnergy  = stringlist.at(5).toDouble();
                        double inField   = stringlist.at(6).toDouble();
                        inTitle          = stringlist.at(nb_total_of_vars-1);
                        int titleKey = 0;
                        foreach(QString keys, keywords)
                            if ( inTitle.contains(keys,Qt::CaseInsensitive)==true)
                            {
                                titleKey++;
                            }

                        if(inType == type && inTime >= minTime && inTime <= maxTime && keyfound == titleKey)
                        {
                            if(inTemp   >= minTemp   && inTemp   <= maxTemp
                                    && inEnergy >= minEnergy && inEnergy <= maxEnergy
                                    && inField  >= minField  && inField  <= maxField )
                            {
                                runs_values_of_vars.push_back( QVector<QString> () );
                                for (int t = 0; t < nb_total_of_vars; t++)
                                {
                                    runs_values_of_vars[nb_runs].push_back(stringlist.at(t));
                                }
                                nb_runs++;
                            }
                        }
                    }
                }
            }
        }
    }
    return runs_values_of_vars;
}


void Search::results()
{
    //Read the results of query
    QVector< QVector<QString> >runs_values_of_vars  = engine_bnmr();
    int nb_runs          = runs_values_of_vars.size();
    int nb_total_of_vars = listDefaultHeaders().size() + tickedlogPath.size();

    //Headers of the results table
    QStringList horLabels;
    //1st Column for checkbox
    horLabels	<<"x";
    //Columns of the default headers (usually run,year,mode,time,T,E,B,Title)
    //defined in the function listDefaultHeaders
    for (short t = 0; t< listDefaultHeaders().size()-1; t++)
    {
        horLabels	<<  listDefaultHeaders().at(t);
    }
    //More custom variables
    for (short t = 0; t< tickedlogPath.size(); t++)
    {
        horLabels <<tickedlogSymbol.at(t);
    }
    //title is always last
    horLabels<<listDefaultHeaders().at(listDefaultHeaders().size()-1);
    //total headers
    int nb_headers = horLabels.size();

    //Define the results table here;
    QTableWidget *resTable = schGUI->tableWidget_results;
    resTable  -> clearContents();
    resTable  ->  setColumnCount(nb_headers);
    resTable  ->  setRowCount(nb_runs);
    resTable  ->  setHorizontalHeaderLabels(horLabels);
    QHeaderView * headers = resTable -> horizontalHeader();
    headers->resizeSection (0, 20 );

    for (int m = 1 ; m < nb_headers -1; m ++ )
    {
        headers->resizeSection (m, 60);
    }
    headers->setStretchLastSection(1);

    for (int m= 0 ; m < nb_runs ; m ++ )
    {
        QTableWidgetItem *checkbox = new QTableWidgetItem("");
        checkbox->setFlags( Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
        checkbox->setCheckState(Qt::Unchecked);
        resTable->  setItem (m,0,checkbox);
        for (int t = 0; t < nb_total_of_vars; t++)
        {
            resTable-> setItem (m,t+1, new QTableWidgetItem(runs_values_of_vars[m].at(t)));
        }
    }
    schGUI->pushButton_checkall->setText("Select");
}

//Selects checked runs and makes a list in with year/number, eg: 2009:45344,40122|2013:43211
QString Search::checked_runs()
{
    //Define the results table here;
    QTableWidget *resTable = schGUI->tableWidget_results;
    //Read the number of runs in the table.
    int    nb_runs     = resTable -> rowCount();
    QString list(","), year0 = "0";
    for (int run_index= 0 ; run_index < nb_runs ; run_index ++ )
    {
        //Count the number of runs in the table which are checked by the user
        if (resTable -> item(run_index,0)->checkState () == 2 )
        {
            QString year  = resTable-> item(run_index,2)->text();
            //Separates runs taken during different years by "|year:"
            if ( year != year0 )
            {
                list.truncate(list.length() -1 );
                list +="|"+ year+":";
                year0 = year;
            }
            //separates between runs by comma ","
            list += resTable-> item(run_index,1)->text()+",";
        }
    }
    //removes the first "|" from the string runs
    list.remove(0,1);
    //removes the last comma ","
    list.truncate(list.length() -1 );
    //The end result will be this string, something like "2009:45344,40122|2013:43211,40123,40900"
    return list;
}

//select or deselect all runs
void Search::checkall()
{
    QTableWidget *table = schGUI->tableWidget_results;
    int    nb_runs = table->rowCount();
    if(schGUI->pushButton_checkall->text() == "Select")
    {
        for (int row= 0 ; row < nb_runs ; row ++ )
        {
            QTableWidgetItem *checkbox = table->item(row, 0);
            checkbox->setCheckState(Qt::Checked);
        }
        schGUI->pushButton_checkall->setText("Deselect");
    }
    else
    {
        for (int row= 0 ; row < nb_runs ; row ++ )
        {
            QTableWidgetItem *checkbox = table->item(row, 0);
            checkbox->setCheckState(Qt::Unchecked);
        }
        schGUI->pushButton_checkall->setText("Select");
    }
}

//This section is needed to change the log variable showen on the table.
//This is invoked when one clicks on More pushbutton.
void Search::Edit_logs()
{
    readLogs();
    updatelogs dialog;
    dialog.exec();
    results();
}

void updatelogs::change_logSelection(int arg)
{
    if(logBool.at(arg)=="yes")
    {
        logBool.replace(arg,"no");
    }
    else if(logBool.at(arg)=="no")
    {
        logBool.replace(arg,"yes");
    }
}


void updatelogs::update_logSelection()
{
    tickedlogLabel.clear(), tickedlogSymbol.clear(), tickedlogPath.clear();
    for (int l = 0; l < logLabel.size();l++)
    {
        if (logBool.at(l) == "yes" )
        {
            tickedlogLabel<< logLabel.at(l);
            tickedlogSymbol<< logSymbol.at(l);
            tickedlogPath << logPath.at(l);
        }
    }
}

void updatelogs::edit_logfile()
{
    if (!QFile("bnmr-bnqr-logs.txt").open(QIODevice::ReadOnly | QIODevice::Text) )
    {
        QFile::copy(":/files/bnmr-bnqr-logs.txt","bnmr-bnqr-logs.txt");
        QFile newfile("bnmr-bnqr-logs.txt");
    }
    bool openState = QDesktopServices::openUrl(QUrl("bnmr-bnqr-logs.txt"));
    if ( openState == false)
    {
        QMessageBox::warning(this, tr("Editing logged variables: "),
                             tr("Please edit the file \"bnmr-bnqr-logs.txt\"\n"
                                " (located in your working directory)\n\n"
                                "Note: to make it permanent, edit \n"
                                "bnmroffice/misc/files/bnmr-bnqr-logs.txt and re-compile"));
    }
}

updatelogs::updatelogs(QWidget *parent)
    : QDialog(parent)
{
    createLayout();
}

void updatelogs::createLayout()
{
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setSizeConstraint(QLayout::SetNoConstraint);

    int nbofLogs = logLabel.size();
    int nb = 0, l = 0, nbcols = 3;

    while (nb < nbofLogs )
    {
        for (int m = 0 ; m < nbcols ; m ++ )
        {
            if (nbcols*l+m < nbofLogs)
            {
                int arg = nbcols*l+m;
                QCheckBox *cBox = new QCheckBox();
                cBox   -> setText(logLabel.at(nbcols*l+m));
                if (logBool.at(arg)== "yes" )
                {
                    cBox   -> setChecked(true);
                }
                if (logBool.at(arg)== "no"  )
                {
                    cBox   -> setChecked(false);
                }
                mainLayout->addWidget(cBox,l,m);
                cBox -> setObjectName(logLabel.at(arg));
                QSignalMapper * mapper = new QSignalMapper(this);
                connect(mapper, SIGNAL(mapped(int)), this, SLOT(change_logSelection(int)));
                mapper->setMapping(cBox,arg);
                connect(cBox, SIGNAL(clicked()), mapper, SLOT(map()));
            }
        }
        l++;
        nb = nbcols*l;
    }

    QDialogButtonBox *okbuttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
    connect(okbuttonBox, SIGNAL(accepted()), this, SLOT(update_logSelection()));
    connect(okbuttonBox, SIGNAL(accepted()), this, SLOT(reject()));

    QDialogButtonBox *cancelbuttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel);
    connect(cancelbuttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QPushButton *editFile = new QPushButton(tr("&Edit"));
    editFile->setIcon(QIcon(":/images/editfile.png"));
    editFile->setIconSize(QSize(25,18));
    editFile -> setDefault(true);

    QDialogButtonBox *editFileButton = new QDialogButtonBox;
    editFileButton -> addButton(editFile, QDialogButtonBox::YesRole);

    connect(editFileButton, SIGNAL(accepted()), this, SLOT(edit_logfile()));
    connect(editFileButton, SIGNAL(accepted()), this, SLOT(reject()));

    mainLayout->addWidget(okbuttonBox, int(nbofLogs/3) + 1, 0,Qt::AlignHCenter);
    mainLayout->addWidget(editFileButton,int(nbofLogs/3) + 1,1,Qt::AlignHCenter);
    mainLayout->addWidget(cancelbuttonBox, int(nbofLogs/3) + 1, 2,Qt::AlignHCenter);

    setLayout(mainLayout);
}



