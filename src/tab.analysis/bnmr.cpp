/***************************************************************************
*  File: bnmr.cpp                                                          *
*  Description: handles the bnmr data.                                     *
*  Author: Hassan Saadaoui                                                 *
*  e-mail: saadaoui@triumf.ca                                              *
*  $Id: bnmr.cpp     2014-12-20 $                                          *
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

#include "bnmr.h"
#include "constants.h"
#include "tab_search.h"



//the function sent to the ui
QStringList BNMR::bNMRfiles(QStringList &headers,QVector< QVector <QString> > &headerValue)
{
    QStringList  list_of_plots;
    list_of_plots.clear();

    //lets read the run numbers, year taken and sign of asymmetry
    QVector<int> runs, years,signs;
    int nbRuns = prepareFiles(runs, years,signs,headers,headerValue);
    headers = runHeaders(runs, years,signs,headerValue);

    for(short i = 0; i < nbRuns; i++)
    {
        runNb    = runs.at(i);
        runYear  = years.at(i);
        runSign  = signs.at(i);

        //qDebug()<<runNb<<runYear<<runSign;
        //get the full of data
        QString inFile = msrfile(runNb,runYear);

        int  hi[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
        QString  outFile = "temp.msr.txt";

        if (QFile(inFile).exists())
        {
            int run_opened = 0;
            run_opened =   MUD2COL_convert((char *)inFile.toStdString().c_str(),
                                           (char *)outFile.toStdString().c_str(),0,
                                           hi,&nbHists);
            //qDebug() <<inFile<<run_opened <<nbHists;
            if ( run_opened == 0)
            {
                qDebug()<<inFile<<"File not read";
            }
            else
            {
                if ( run_opened == 1)
                {
                    BNMR_indvar(runNb,runYear);

                    //constructs vector of scans to not include in the asymmetry/mode

                    //mode of 2e asym; only 8 if it is 2e otherwise, it is 0
                    modType  = (runType=="2e"&& mode2eType==0) ? 8 :  0;
                    dwtperfreq = (runType=="2e") ? dwtperfreq:  1;

                    //sum 1f, only for 1f...dohhh
                    sumScans = (sum==1&&runType=="1f")? "Yes": "No";

                    //bin must be bigger or eq 1;
                    bin = (bin>0)? bin : 1;

                    //create the ascii plots here
                    for(short index = 0 ; index < subType.size(); index++)
                    {
                        hisType = subType.at(index).toInt();
                        QString basename = QString::number(runNb)+"."+histList(hisType)+".dat";
                        list_of_plots << GENERATE(outFile,basename);
                    }
                }
            }
        }
        else
        {
            qDebug()<<inFile<<" not found";
        }
    }
    return list_of_plots;
}

QString BNMR::histList(int hist)
{

    switch(hist)
    {
    case 0:  return "Asy";  break;
    case 1:  return "Asy+";  break;
    case 2:  return "Asy-";  break;
    case 3:  return (expType=="bnmr")? "AsyF": "AsyR";  break;
    case 4:  return (expType=="bnmr")? "AsyB": "AsyL";  break;
    case 5:  return "AsyN";  break;
    case 6:  return "AsyN+";  break;
    case 7:  return "AsyN-";  break;
    case 8:  return (expType=="bnmr")? "F+": "R+";  break;
    case 9:  return (expType=="bnmr")? "B+": "L+";  break;
    case 10:  return (expType=="bnmr")? "F-": "R-";  break;
    case 11:  return (expType=="bnmr")? "B-": "L-";  break;
    case 12:  return "NF+";  break;
    case 13:  return "NB+";  break;
    case 14:  return "NF-";  break;
    case 15:  return "NB-";  break;
    }
}
/*read .inf files*/
int  BNMR::read_infFile(QVector<int> &runs, QVector<int> &years, QVector<int> &signs,
                        QStringList &headers,QVector< QVector <QString> > &headerValue)
{
    int nbRuns = 0, run = 0, year = 0, sign = 1;
    runs.clear();years.clear();signs.clear();
    //Create a temporary file which would contains the files to fit with headers
    QFile inFile(input);
    inFile.open(QFile::ReadOnly);
    if(inFile.exists())
    {
        QTextStream inData(&inFile);
        QStringList result= inData.readLine().split(QRegExp("\\s"));
        foreach (const QString &str, result)
        {
            if (!str.isEmpty())
                headers<< str;
        }
        while (true )
        {
            QString lines = inData.readLine();
            //avoid lines starting with  #, ! or //
            if (lines[0] != '#' &&  lines[0] != '!' &&  lines[0] != '/' )
            {
                headerValue.push_back(QVector<QString>());
                QStringList results= lines.split(QRegExp("\\s"));
                foreach (const QString &str, results)
                {
                    if (!str.isEmpty())
                        headerValue[nbRuns].push_back(str);
                }
                run  = headerValue[nbRuns][0].toInt();
                year = headerValue[nbRuns][1].toInt();

                runs.push_back(run);
                years.push_back(year);
                signs.push_back(sign);

                nbRuns++;
            }
            if( inData.atEnd() ) break;
        }
    }
    else
    {
        qDebug() <<"\n\nCannot find/open "<<input<<" file\n\n";
    }
    inFile.close();
    return nbRuns;
}

/*convert the .list file into a run numbers list*/
int  BNMR::read_listFile(QVector<int> &runs, QVector<int> &years, QVector<int> &signs,
                         QStringList &headers,QVector< QVector <QString> > &headerValue)
{
    int nbRuns = 0;
    //Create a temporary file which would contains the files to fit with headers
    QFile inFile(input);
    inFile.open(QFile::ReadOnly);
    if(inFile.exists())
    {
        QTextStream inData(&inFile);
        input = inData.readLine();
        nbRuns = read_numbers(runs,years,signs,headers,headerValue);
    }
    else
    {
        qDebug() <<"\n\nCannot find/open "<<input<<" file\n\n";
    }
    inFile.close();
    return nbRuns;
}

/*convert the linedit input into a run numbers list*/
int  BNMR::read_numbers(QVector<int> &runs, QVector<int> &years, QVector<int> &signs,
                        QStringList &headers,QVector< QVector <QString> > &headerValue)
{
    int nbRuns = 0;
    runs.clear();
    years.clear();
    signs.clear();
    QStringList runlist = input.split(QRegExp("\\|"));
    foreach(QString yearAruns, runlist )
    {
        QStringList yearsOruns = yearAruns.split(QRegExp(":"));
        int         year       = yearsOruns.at(0).toInt();
        if (yearsOruns.size() >= 2 )
        {
            QStringList RunsList   = yearsOruns.at(1).split(QRegExp(","));
            foreach(QString runName, RunsList )
            {
                int start_run = 0;
                int stop_run  = 0;
                int sign      = 1;
                if (runName.contains("(-)"))
                {
                    sign = -1;
                    runName.remove("(-)");
                    start_run = runName.toInt();
                    stop_run  = runName.toInt();
                }
                else if (runName.contains("(+)"))
                {
                    sign = 1;
                    runName.remove("(+)");
                    start_run = runName.toInt();
                    stop_run  = runName.toInt();
                }
                else if (runName.contains("-"))
                {
                    QStringList RealRuns   = runName.split(QRegExp("-"));
                    start_run = RealRuns.at(0).toInt();
                    if (RealRuns.size() >1 && !RealRuns.at(1).isEmpty())
                        stop_run  = RealRuns.at(1).toInt();
                    else
                        stop_run  =  start_run;
                }
                else
                {
                    sign = 1;
                    start_run = runName.toInt();
                    stop_run  = runName.toInt();
                }
                for ( int run = start_run; run <= stop_run ; run++ )
                {
                    runs.push_back(run);
                    years.push_back(year);
                    signs.push_back(sign);
                    nbRuns++;
                }
            }
        }
    }
    return nbRuns;
}

/*return the run year and sign of the input*/
int BNMR::prepareFiles(QVector<int> &runs, QVector<int> &years, QVector<int> &signs,
                      QStringList &headers,QVector< QVector <QString> > &headerValue)
{
    int nbRuns = 0;
    if(input!="" && !input.isEmpty() && !input.isNull())
    {
        if (QFileInfo(input).suffix()=="inf")
            nbRuns =  read_infFile(runs,years,signs,headers,headerValue);
        else if (QFileInfo(input).suffix()=="list")
            nbRuns = read_listFile(runs,years,signs,headers,headerValue);
        else
            nbRuns = read_numbers(runs,years,signs,headers,headerValue);
    }
    //qDebug()<<runs<<years<<signs<<nbRuns;
    return nbRuns;
}

/*Find the full path of each run*/
QString  BNMR::msrfile(int runnumber,int year)
{
    expType  = ( runnumber < 45000)?  "bnmr": "bnqr" ;
    QString path = ( expType == "bnmr" )?  bnmrDataPath: bnqrDataPath ;
    return  path+QString::number(year)+"/0"+QString::number(runnumber)+".msr";
}

/*initialize .msr parameters*/
void BNMR::initialize_parameters(void)
{
    x_start       = 0;
    x_stop        = 0;
    x_step        = 0;
    Randomize     = 0;
    RF_on_time    = 0;
    HelFlipSleep  = 0;
    Dwell_time    = 0;
    RFon_delay    = 0;
    max_pow       = 0;
    prebeam_dwt   = 0;
    beam_on_period= 0;
    beam_on_dwt   = 0;
    beam_off_dwt  = 0;
    dwtperfreq    = 0;
    nbRFdelays    = 0;
    nbpostRFdw    = 0;
    runTime       = 0;
}

/*Read the independent variables*/
QStringList BNMR::BNMR_indvar(int runnumber, int year)
{
    QStringList stringlist;
    double field =0, energy=0, temp=0,dtemp=0;
    double naCell=0, plBias=0, iniEnergy=0;

    initialize_parameters();

    QString file2open = msrfile(runnumber,year);
    if(QFileInfo(file2open).exists())
    {
        runDescription  getdesc;
        getdesc     =  rundesc((char *)file2open.toStdString().c_str());

        runTitle    =  QString::fromStdString(getdesc.Title);
        runType     =  QString::fromStdString(getdesc.Insert);
        runTime     =  getdesc.ElapsedSec / 60.;

        if ( runType == "1n")
        {
            runType = "1f";
            runFlag = "1n";
        }
        else
        {
            runFlag = runType;
        }

        for (int i = 0; i< getdesc.Nb_ind_vrbls; i++)
        {
            //Check these defenitions and  path// 15.12.2014
            if (strcmp("/Magnet/mag_field", getdesc.Name[i] ) == 0)
            {
                field =  getdesc.Mean[i]*10000;//10000 for Tesla->Gauss conversion
            }
            if (strcmp("ILE2A1:HH:RDCUR", getdesc.Name[i] ) == 0)
            {
                field =  0.175 + 2.2131*getdesc.Mean[i];//calibration equation
            }
            if (strcmp("BNQR:HVBIAS:RDVOL", getdesc.Name[i] ) == 0 ||
                    strcmp("BNMR:HVBIAS:POS:", getdesc.Name[i] ) == 0 ||
                    strcmp("BNMR:HVBIAS:POS:RDVOL", getdesc.Name[i] ) == 0 )
            {
                plBias	=  getdesc.Mean[i]/1000;//1000 for V to kV conversion
            }
            if (strcmp("ITE:BIAS:RDVOL", getdesc.Name[i] ) == 0  )
            {
                iniEnergy	=  getdesc.Mean[i]/1000;//1000 for V to kV conversion
            }
            if (strcmp("ILE2:BIAS15:RDVOL", getdesc.Name[i] ) == 0  )
            {
                naCell   	=  getdesc.Mean[i]/1000;//1000 for V to kV conversion
            }
            if (strcmp("/Sample/read_B", getdesc.Name[i] ) == 0  ||
                    strcmp("/Sample1/read_B", getdesc.Name[i] ) == 0  )
            {
                temp = getdesc.Mean[i];
                dtemp = getdesc.StdDev[i];
            }

            if(runType == "2e")
            {
                if (strcmp("PPG/PPG2e/Randomize freq values", getdesc.Name[i] ) == 0)
                    Randomize  = getdesc.Mean[i];
                if (strcmp("PPG/PPG2e/RF on time (ms)", getdesc.Name[i] ) == 0)
                    RF_on_time = getdesc.Mean[i];
                if (strcmp("PPG/PPG2e/num RF on delays (dwell times)", getdesc.Name[i] ) == 0)
                    nbRFdelays  = getdesc.Mean[i];
                if (strcmp("PPG/PPG2e/Number dwelltimes per freq", getdesc.Name[i] ) == 0)
                    dwtperfreq   = getdesc.Mean[i];
                if (strcmp("PPG/PPG2e/num post RF beamOn dwelltimes", getdesc.Name[i] ) == 0)
                    nbpostRFdw   = getdesc.Mean[i];
                if (strcmp("PPG/PPG2e/frequency start (Hz)", getdesc.Name[i] ) == 0)
                    x_start = getdesc.Mean[i]/1000;
                if (strcmp("PPG/PPG2e/frequency stop (Hz)", getdesc.Name[i] ) == 0)
                    x_stop = getdesc.Mean[i]/1000;
                if (strcmp("PPG/PPG2e/frequency increment (Hz)", getdesc.Name[i] ) == 0)
                    x_step = getdesc.Mean[i]/1000;
            }
            if(runType == "20")
            {
                if (strcmp("PPG/PPG20/Dwell time (ms)", getdesc.Name[i] ) == 0)
                    Dwell_time = getdesc.Mean[i];
                x_step         = Dwell_time;
                if (strcmp("PPG/PPG20/e20 prebeam dwelltimes", getdesc.Name[i] ) == 0)
                    prebeam_dwt   = getdesc.Mean[i];
                x_start        =-x_step*prebeam_dwt ;
                if (strcmp("PPG/PPG20/e20 beam on dwelltimes", getdesc.Name[i] ) == 0)
                    beam_on_dwt  = getdesc.Mean[i];
                if (strcmp("PPG/PPG20/e20  beam off dwelltimes", getdesc.Name[i] ) == 0)
                    beam_off_dwt = getdesc.Mean[i];
                x_stop         = x_step*(beam_on_dwt+beam_off_dwt  );
            }
        }
        //Energy in kV...maybe! If .msr file has the correct units.
        energy	=  iniEnergy - plBias - naCell;
        beam_on_period = Dwell_time*beam_on_dwt;
    }

    //write the list of ind variables
    //"Run" <<"Year"<<"t(min)"<<"T(K)" <<"E(keV)" <<"B(G)"<<"V(kV)";
    stringlist << QString::number(runnumber)
               << QString::number(year)
               << QString::number(runTime,'f',0)
               << QString::number(temp,'f',2) +","+ QString::number(dtemp,'f',2)
               << QString::number(energy,'f',1)
               << QString::number(field,'f',0);
    return stringlist;
}

/*contruct the headers of each run*/
QStringList BNMR::runHeaders(QVector<int> runs, QVector<int> years, QVector<int> signs,
                             QVector< QVector <QString> > &headerValue)
{
    QStringList headerlist;
    headerlist<<"Run"<<"Year"<<"t(min)"<<"T(K)"<<"E(keV)"<<"B(G)";

    int nbRuns =  runs.size();
    if(nbRuns >0)
    {
        headerValue.resize(nbRuns);

        QFile outFile("temp.inf");
        outFile.open(QFile::WriteOnly);
        QTextStream outData(&outFile);

        for(short l = 0; l < headerlist.size(); l ++)
            outData << headerlist.at(l) <<"\t";
        outData << endl;

        for(short i = 0; i < nbRuns; i ++)
        {
            QStringList list = BNMR_indvar(runs.at(i),years.at(i));
            for(short l = 0; l < headerlist.size(); l++)
            {
                headerValue[i].push_back(list.at(l));
                outData << headerValue[i].at(l)<<"\t";
            }
            outData << endl;
        }
        outFile.close();
    }
    else
    {
        headerlist.clear();
        headerValue.clear();
    }
    return headerlist;
}

//The asymmetry is generated here, where Nb is the number of histograms used to generate the
// asymmetry, hi1-hi4 are the histograms numbers, and cst is the normalization constant.
// 'i' is the index of frequency or time axis in the histogram.
Generate_asym gen_asym(int Nb,QVector< QVector <double> > hist,
                       int i,int hi1,int hi2,int hi3,int hi4,double cst)
{
    Generate_asym asym;
    asym.Mean   = 0.0;
    asym.StdDev = 0.0;
    if ( Nb == 4 &&  (hist[i][hi1] != 0 || hist[i][hi2] != 0 )
         && ( hist[i][hi3]  != 0 || hist[i][hi4]  != 0 ) )
    {
        asym.Mean   =  (hist[i][hi1] - hist[i][hi2])/(hist[i][hi1] + hist[i][hi2])
                -(hist[i][hi3] - hist[i][hi4])/(hist[i][hi3] + hist[i][hi4]);
        asym.StdDev =  sqrt(  pow(sqrt(4.0 * hist[i][hi1] * hist[i][hi2]
                                       /pow(hist[i][hi1] + hist[i][hi2],3.0)),2.0)
                              + pow(sqrt(4.0 * hist[i][hi3] * hist[i][hi4]
                                         /pow(hist[i][hi3] + hist[i][hi4],3.0)),2.0));
    }
    else if ( Nb == 2 &&  (hist[i][hi1] != 0 || hist[i][hi2] != 0 ) )
    {
        asym.Mean   = (hist[i][hi1] - hist[i][hi2])/(hist[i][hi1] + hist[i][hi2]);
        asym.StdDev = sqrt(4.0 * hist[i][hi1] * hist[i][hi2]
                           /pow(hist[i][hi1] + hist[i][hi2],3.0));
    }
    else if ( Nb == 1 && cst != 0 && hist[i][hi1] && hist[i][hi2])
    {
        asym.Mean   =  (hist[i][hi1]/cst) -  ( hist[i][hi2] / cst);;
        asym.StdDev =  sqrt(pow((hist[i][hi1]/cst)*sqrt((1.0/hist[i][hi1]) + (1.0/cst)),2.0)
                            +pow((hist[i][hi2]/cst)*sqrt((1.0/hist[i][hi2]) + (1.0/cst)),2.0));
    }
    else if ( Nb == 0 )
    {
        asym.Mean   = hist[i][hi1];
        asym.StdDev = sqrt(hist[i][hi1]);
    }
    else
    {
        asym.Mean   = 0.0;
        asym.StdDev = 0.0;
    }
    return asym;
}

/*Bin data*/
void BNMR::bin_data(QString in_run, QString out_run)
{
    ifstream inRun(in_run.toStdString().c_str());
    ofstream outRun(out_run.toStdString().c_str());
    int      row = 0;
    double   inhist[10000];
    double   outhist[10000];
    int      j = 1;
    int o = 0, bin2 = bin;
    if (  runType == "1f" )
    {
        o = 1;
        bin2 = 1;
    }
    else
    {
        o = 0;
    }
    while (!inRun.eof())
    {
        for (int col = 1; col <= nbHists ; col ++)
        {
            inRun >> inhist[col] ;
            if ( row <= j*bin )
            {
                outhist[col] += inhist[col];
                outhist[o]    = bin2*inhist[o];
            }
            if ( row == j*bin )
            {
                outRun << outhist[col]  << " ";
                outhist[col] = 0;
                if (  col == nbHists )
                {
                    outRun <<endl;
                    j++;
                }
            }
        }
        row ++;
    }
}

/*mode 1f and 1n*/
bool BNMR::Mode1(QString inrun, QString outrun,QVector<int> &killscan)
{
    QFile infile(inrun);
    infile.open(QIODevice::ReadOnly|QIODevice::Text);
    QTextStream indata(&infile);
    qint64 size = infile.size();

    if ( size == 0 )
    {
        qDebug()<< QObject::tr("Error: file %1 is empty or not found").arg(inrun);
    }
    else
    {
        //count rows in the data, find min, find max
        int nb_pts = 0, x = 0, min = INFINITY, max = -INFINITY;
        while(!indata.atEnd())
        {
            x = indata.readLine().split(QRegExp("\\s")).at(0).toInt();
            if ( x > max )
                max = x;
            if ( x < min )
                min =  x;
            nb_pts++;
        }
        indata.seek(0);
        int fstep  = - ( indata.readLine().split(QRegExp("\\s")).at(0).toDouble()
                         - indata.readLine().split(QRegExp("\\s")).at(0).toDouble() );
        //qDebug()<< nb_pts<< min << max << fstep;

        indata.seek(0);
        //combine (adding) scans together
        if ( sumScans =="Yes" )
        {
            QVector< QVector<int> > Hist;
            int x = 0,row = 0;

            /*first read the raw data and create temporary histogram columns*/
            while(!indata.atEnd())
            {
                Hist.resize(row+1);
                for (int col = 0; col < nbHists ; col ++)
                {
                    indata >> x;
                    Hist[row].push_back(x);
                }
                row++;
            }

            /*Find number of scans per file*/
            int pts_per_scan = (int)((max -min )/fstep) + 1;
            int Nb_scans = (int)( (row-1)/pts_per_scan) +1;


            killscan.push_back(0);
            QStringList list = killscans.split(QRegExp(","));
            for (int s = 1; s <= Nb_scans ; s ++)
                killscan.push_back(0);
            foreach( QString str, list)
                if (str.toInt()<= Nb_scans)
                    killscan[str.toInt()] = 1;

            int nb = 0, k = 1, j = 0;
            //qDebug() <<pts_per_scan<< "=pts_per_scan   Nb_scans="<< Nb_scans << endl;

            //add scans here
            for (int scan = 1; scan <= (int)(Nb_scans/2); scan ++)
            {
                //zero the first scan if the user wants to kill it
                if ( killscan[1] == 1)
                    for (int row = (scan-1 +nb) * pts_per_scan;
                         row < (scan+nb) * pts_per_scan;row++ )
                        if (row <= nb_pts)
                            for (int col = 1; col < nbHists ; col ++)
                                Hist[row][col] = 0;
                        else
                            break;
                //add non killed scans for up sweep
                if ( k > 1 &&  killscan[k] == 0 )
                    for (int row = (scan-1 +nb) * pts_per_scan;
                         row < (scan+nb) * pts_per_scan;row++ )
                    {
                        if (row <= nb_pts)
                            for (int col = 1; col < nbHists ; col ++)
                                Hist[j][col] += Hist[row][col];
                        else
                            break;
                        j++;
                    }
                k++;
                j=0;
                //add non killed scans for down sweep
                if ( killscan[k]==0)
                    for (int row = (scan+nb) * pts_per_scan;
                         row < (scan+1+nb)*pts_per_scan;row++ )
                    {
                        if (row <= nb_pts)
                            for (int col = 1; col < nbHists ; col ++)
                                Hist[(pts_per_scan-1)-j][col] += Hist[row][col];
                        else
                            break;
                        j++;
                    }
                k++;
                j=0;
                nb++;
            }

            QFile outfile(outrun);
            outfile.open(QIODevice::WriteOnly | QIODevice::Truncate);
            QTextStream outdata(&outfile);
            //create the final file
            for ( int row = 0;row<pts_per_scan;row++)
            {
                outdata << Hist[row][0] << "\t";
                for (int col = 1; col < nbHists ; col ++)
                    outdata << Hist[row][col] << "\t";
                outdata <<endl;
            }
            outfile.close();
        }
        else
        {
            QFile outfile(outrun);
            outfile.open(QIODevice::WriteOnly |QIODevice::Truncate);
            QTextStream outdata(&outfile);
            QVector<int> his;
            // read the file and copy it
            for (int row = 0; row < nb_pts ; row ++)
            {
                his.clear();
                for (int col = 0; col < nbHists ; col ++)
                {
                    indata >> x;
                    his.push_back(x);
                }
                for (int col = 0; col < nbHists ; col ++)
                    outdata << his.at(col)<<"\t";
                outdata <<endl;
            }
            outfile.close();
        }
    }
    infile.close();
    return true;
}

/*mode 2e*/
void BNMR::Mode2e(QString inrun,QString outrun)
{
    QFile infile(inrun);
    infile.open(QIODevice::ReadOnly|QIODevice::Text);
    QTextStream indata(&infile);
    qint64 size = infile.size();

    if ( size == 0 )
    {
        qDebug()<< QObject::tr("Error: file %1 is empty or not found").arg(inrun);
    }
    else
    {
        //count rows in the data
        int nbRows = 0;
        while(!indata.atEnd())
        {
            indata.readLine();
            nbRows++;
        }
        indata.seek(0);

        //exclude nbRFon delays histograms from data
        int K = 0;
        while ( K <nbRFdelays )
        {
            indata.readLine();
            K++;
        }
        K = 0;

        int I[3]={1,         dwtperfreq,    dwtperfreq+1 };
        int F[3]={dwtperfreq,dwtperfreq +1, 2*dwtperfreq };

        QFile outfile(outrun);
        outfile.open(QIODevice::WriteOnly);
        QTextStream outdata(&outfile);
        while (K<(int)( (nbRows - 1) - ( 2*nbRFdelays ) )/(2*dwtperfreq-1) )
        {
            for (int i = 0; i <= 2;i++)
            {
                int   his=0, counter[100]={0};
                for (int j =I[i]; j< F[i] ;j++)
                    for (int col = 1; col <= nbHists ; col ++)
                    {
                        indata >> his ;
                        counter[col] += his;
                    }
                for (int col = 1; col <= nbHists ; col ++)
                {
                    outdata << QString::number(counter[col]) <<" " ;
                    counter[col] = 0;
                }
                outdata << endl;
            }
            K++;
        }
        outfile.close();
    }
    infile.close();
}

/*Mode T1*/
void BNMR::ModeT1(QString inrun,QString outrun)
{
    QFile infile(inrun);
    infile.open(QIODevice::ReadOnly|QIODevice::Text);
    QTextStream indata(&infile);
    qint64 size = infile.size();

    if ( size == 0 )
    {
        qDebug()<< QObject::tr("Error: file %1 is empty or not found").arg(inrun);
    }
    else
    {
        QFile outfile(outrun);
        outfile.open(QIODevice::WriteOnly);
        QTextStream outdata(&outfile);

        QString hist;
        while (true)
        {
            for (int col = 1; col <= nbHists ; col ++)
            {
                indata >> hist;
                if ( hist != "" && !hist.isEmpty())
                    outdata << hist  << " " ;
                hist="";
            }
            outdata << endl;
            if( indata.atEnd() ) break;
        }
        outfile.close();
    }
    infile.close();
}

/*Makes an index of histogram types,...where F+,B+,NB+..etc.
This messy part corrects the radomness of assigning histograms in .msr files,
which are different in each mode (1f,1n,2e,20), and each experiment (bnmr,bnqr).
This would not exist if the original .msr file is written in  uniform structure*/
void BNMR::hist_index()
{
    hS = 0, hSN = 0, M = 0, h1 = 0, h2 = 0,h3 = 0, h4 = 0;
    if (expType == "bnmr" && runType != "1f")
    {
        hS = 1; hSN = 11;
    }
    if (expType == "bnqr" &&  runType!= "1f")
    {
        hS = 3; hSN = 7;
    }
    if (runType == "1f")
    {
        hS = 2; hSN = 8;
    }
    if ( hisType == 1 ) { M = 2; h1 = hS;    h2 = hS+1;  h3 = 0; h4 = 0;}
    if ( hisType == 2 ) { M = 2; h1 = hS+2;  h2 = hS+3;  h3 = 0; h4 = 0;}
    if ( hisType == 6 ) { M = 2; h1 = hSN;   h2 = hSN+1; h3 = 0; h4 = 0;}
    if ( hisType == 7 ) { M = 2; h1 = hSN+2; h2 = hSN+3; h3 = 0; h4 = 0;}
    if ( hisType == 9 ) { M = 0; h1 = hS;}
    if ( hisType == 8 ) { M = 0; h1 = hS+1;}
    if ( hisType == 11 ){ M = 0; h1 = hS+2;}
    if ( hisType == 10 ){ M = 0; h1 = hS+3;}
    if ( hisType == 13 ){ M = 0; h1 = hSN;}
    if ( hisType == 12 ){ M = 0; h1 = hSN+1;}
    if ( hisType == 15 ){ M = 0; h1 = hSN+2;}
    if ( hisType == 14 ){ M = 0; h1 = hSN+3;}
    if (  runType != "1f" || sumScans == "Yes" )
    {
        if ( hisType == 0 ) { M = 4; h1 = hS;    h2 = hS+1;  h3 = hS+2;  h4 = hS+3; }
        if ( hisType == 3 ) { M = 1; h1 = hS+1;  h2 = hS+3;  h3 = 0;     h4 = 0;    }
        if ( hisType == 4 ) { M = 1; h1 = hS+2;  h2 = hS+4;  h3 = 0;     h4 = 0;    }
        if ( hisType == 5 ) { M = 4; h1 = hSN;   h2 = hSN+1; h3 = hSN+2; h4 = hSN+3;}
    }
}

/*Convert data to a simple hist vectors*/
void BNMR::read_write(QString inFile,QString outFile,QVector<int>&killscan,
                      int&nb_scans, int&Nb_pts_scan,QVector< QVector<double> > &hist)
{
    //qDebug()<<inFile<<outFile<<nbHists<<killscans<<killscan<<sumScans<<nbRFdelays<<dwtperfreq;
    if ( runType == "1f") Mode1(inFile,outFile,killscan);
    if ( runType == "2e") Mode2e(inFile,outFile);
    if ( runType == "20") ModeT1(inFile,outFile);

    //Binning
    QString   binnedFile  = "temp.rawbin.txt";
    if ( bin > 1 )
    {
        bin_data(outFile,binnedFile);
        x_start = x_start + (x_step/dwtperfreq)*bin/2;
    }
    else
        binnedFile = outFile;

    //read file
    QFile infile(binnedFile);
    infile.open(QIODevice::ReadOnly| QIODevice::Text);
    QTextStream indata(&infile);
    double   max = 0.0,x=0.0;
    int      row = 0;
    Nb_pts_scan = 0;
    while (true)
    {
        hist.resize(row+1);
        hist[row].push_back(0);//add empty columns at index 0

        for (int col = 1; col <= nbHists ; col ++)
        {
            indata >> x;
            hist[row].push_back(x);
        }
        if ( hist[row][1] > max )
        {
            max = hist[row][1];
            Nb_pts_scan ++ ;
        }
        if (indata.atEnd())
            break;
        row ++;
    }
    infile.close();

    nb_scans=(Nb_pts_scan!=0)?(int)(row/Nb_pts_scan) +1 : 1;
    if ( runType != "1f" || sumScans=="Yes" )
    {
        nb_scans = 1;
        Nb_pts_scan = (row - 1 );
    }
    x_min = ( x_min < 0.0)? 0.0 : x_min;
}

/*Make asymmetry for type 1f and 1n*/
QStringList BNMR::Asym_Type1(QString basename,int nb_scans,int Nb_pts_scan,
                             QVector< QVector< double> > hist,
                             QVector<int> killscan)
{
    QStringList  namesets;

    Generate_asym asy;
    double   Freq  = 0, asym = 0, dasym = 0,nbsum=0;

    int tPts = hist.size();
    for (int scan = 1; scan < nb_scans+1; scan++)
    {
        int fr = Nb_pts_scan*(scan-1);
        int to = Nb_pts_scan*(scan);
        if ( sumScans=="No" )
        {
            if(hisType==0 &&hist[fr][hS+2] == 0 ){M= 2; h1=hS;    h2= hS+1;  }
            if(hisType==0 &&hist[fr][hS+1] == 0 ){M= 2; h1=hS+2;  h2= hS+3;  }
            if(hisType==3 &&hist[fr][hS+1] != 0 ){M= 1; h1=hS+1;  h2= hS+3;  }
            if(hisType==3 &&hist[fr][hS+1] == 0 ){M= 1; h1=hS+3;  h2= hS+1;  }
            if(hisType==4 &&hist[fr][hS]   != 0 ){M= 1; h1=hS;    h2= hS+2;  }
            if(hisType==4 &&hist[fr][hS]   == 0 ){M= 1; h1=hS+2;  h2= hS;    }
            if(hisType==5 &&hist[fr][hSN+2] == 0){M= 2; h1=hSN;   h2= hSN+1; }
            if(hisType==5 &&hist[fr][hSN+1] == 0){M= 2; h1=hSN+2; h2= hSN+3; }
        }

        if ( (killscan[scan] == 0 && hist[fr][h1] != 0 && sumScans=="No" )
             || ( hist[fr][h1] != 0 && sumScans=="Yes" ) )
        {
            QString file2create;
            if (  nb_scans == 1 )
                file2create = basename;
            if (  nb_scans >  1 )
                file2create = basename.remove(".dat") + ".scan-"
                        + QString::number(scan,'f',0)+".dat";

            namesets <<file2create;

            QFile outfile(file2create);
            outfile.open(QIODevice::WriteOnly|QIODevice::Text);
            QTextStream outdata(&outfile);
            outdata.setRealNumberNotation(QTextStream::FixedNotation);

            for (int i = fr ; i< to -1; i += dwtperfreq)
            {
                if (i  > tPts-2)
                    break;
                if ( sumScans=="Yes" && bin > 1)
                    Freq = (hist[i][1] -   ((hist[1][1] - hist[0][1])/2) ) /1000 ;
                else
                    Freq = hist[i][1] /1000 ;
                nbsum =  hist[i][hSN] + hist[i][hSN+1] + hist[i][hSN+2]  + hist[i][hSN+3];
                asy      =  gen_asym(M,hist,i,h1,h2,h3,h4,nbsum);
                asym    =  asy.Mean;
                dasym    =  asy.StdDev;
                if( Freq >= x_min && Freq <= x_max && !isinf(Freq) && !isnan(Freq)
                        && !isinf(asym) && !isnan(asym) && !isinf(dasym) && !isnan(dasym) )
                    //outFile << Freq <<"\t" <<  runSign*asym <<"\t" << dasym <<endl;
                    outdata << Freq <<"\t" <<  runSign*asym <<"\t" << dasym <<endl;

            }
            outfile.close();
        }
    }
    return namesets;
}

/*Make asymmetry for type 2e and 20*/
QStringList BNMR::Asym_Type2(QString basename, int nb_scans,int Nb_pts_scan,
                             QVector< QVector< double> > hist,
                             QVector<int> killscan)
{
    QStringList  namesets;
    Generate_asym asy;
    double Freq=0,asym=0,dasym=0,nbsum[dwtperfreq],Y[dwtperfreq],DY[dwtperfreq];

    QString file2create =  basename;
    namesets <<file2create;
    ofstream outFile(file2create.toStdString().c_str());
    int tPts = hist.size();
    for (int i = 0 ; i< Nb_pts_scan; i += dwtperfreq)
    {
        if (i  > tPts-2)
            break;
        if ( runType == "20" ) Freq = (x_start + (x_step/dwtperfreq)*i*bin)/1000.0;
        if ( runType == "2e" ) Freq = x_start  + (x_step/dwtperfreq)*i*bin;

        for ( int w = 0; w< dwtperfreq ; w ++ )
        {
            nbsum[w] =  hist[i+w][hSN] + hist[i+w][hSN+1] + hist[i+w][hSN+2]  + hist[i+w][hSN+3];
            asy      =  gen_asym(M,hist,i+w,h1,h2,h3,h4,nbsum[w]);
            Y[w]     =  asy.Mean;
            DY[w]    =  asy.StdDev;
        }
        if ( modType == 8 )
        {
            asym  = 2.0*(Y[2] - Y[0]);
            dasym = 2.0*sqrt(pow(DY[2],2.0) + pow(DY[2],2.0) );
        }
        else
        {
            asym = Y[0];
            dasym = DY[0];
        }
        if( Freq >= x_min && Freq <= x_max && !isinf(Freq) && !isnan(Freq)
                && !isinf(asym) && !isnan(asym) && !isinf(dasym) && !isnan(dasym) )
        {
            outFile << Freq <<"\t" <<  runSign*asym <<"\t" << dasym <<endl;
        }
    }
    outFile.close();
    return namesets;
}

/*Generate asymmetry all*/
QStringList BNMR::GENERATE(QString run,QString basename)
{
    QStringList  namesets;
    QFile file2open(run);
    file2open.open(QIODevice::ReadOnly|QIODevice::Text);
    qint64 size = file2open.size();
    if (size != 0 )
    {
        //Prepare data for different mode
        QVector<int> killscan;
        int nb_scans=0, Nb_pts_scan=0;
        QVector< QVector< double> > hist;

        //read the indices of type of hist, data, runtype...etc.
        hist_index();

        //read the .msr data
        read_write(run,"temp.raw.txt",killscan,nb_scans,Nb_pts_scan,hist);

        //nb of bins per file
        int tPts = hist.size();

        //        qDebug() <<"Insert="<<runType<<"Title="<<runTitle<<"cellflag="<<runFlag;
        //        qDebug() <<"x_start="<<  x_start <<" x_stop="<< x_stop <<" x_step="<< x_step;
        //        qDebug() <<"Randomize ="<<  Randomize   <<" RF_on_time="
        //                <<  RF_on_time  <<" HelFlipSleep =" << HelFlipSleep ;
        //        qDebug() <<"Dwell_time ="<<  Dwell_time  <<" RFon_delay="
        //                <<  RFon_delay  <<"  max_pow ="<<  max_pow      ;
        //        qDebug() <<"prebeam_dwt="<<  prebeam_dwt  <<" beam_on_dwt="
        //                <<  beam_on_dwt <<" beam_off_dwt=" << beam_off_dwt ;
        //        qDebug() <<"dwtperfreq="<<  dwtperfreq   <<"  nbRFdelays ="
        //                <<   nbRFdelays  <<"  nbpostRFdw ="<<   nbpostRFdw  ;
        //        qDebug() <<"Time="<<  runTime     <<"  Fmin =" <<   x_min      <<"  Fmax=" <<  x_max   ;
        //        qDebug() <<"hS="<<  hS <<"  hSN ="<<  hSN <<" flip_asym="<< runSign;
        //        qDebug() <<"bin="<<  bin  <<"  nbRFdelays="<<  nbRFdelays
        //                <<" dwtperfreq="<< dwtperfreq <<" nbHists ="<< nbHists <<" hisType="<<hisType;
        //        qDebug() <<"run_type="<<  runType
        //                <<"  mode_type="<<  modType;
        //        qDebug()<< " x_start=" << x_start<< " Nb_pts_scan=" << Nb_pts_scan
        //                << " M=" << M << "h1="<< h1<< " h2=" << h2
        //                << " h3="<< h3<< " h4="<< h4
        //                << " nb_scan="<< nb_scans
        //                << " Nb_pts_scan="<< Nb_pts_scan;
        //create data
        if (runType=="1f")//for 1f and 1n
            namesets = Asym_Type1(basename,nb_scans,Nb_pts_scan,hist,killscan);
        else//for 20 and 2e
            namesets = Asym_Type2(basename,nb_scans,Nb_pts_scan,hist,killscan);
    }
    return namesets;
}

