/***************************************************************************
*  File: fitting_routine.cpp                                               *
*  Description: fitting fourtine based on minuit.                          * 
*  Author: Hassan Saadaoui                                                 *
*  e-mail: saadaoui@triumf.ca                                              *
*  $Id: fitting_routine.cpp 2014-12-20 $                                   *
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



/********************************CCP headers ***********************************/
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
using namespace std;                            // Declared for output files

#include <QLibrary>
#include <QDebug>
#include "fitting_routine.h"
#include "read_data.h"

//Gets the parameters list for each file. Critical when using a global fit
vector<double>  MODEL::getParList(int f, const std::vector<double> &pars)const
{
    std::vector<double> par;
    short s = 0;
    //loop over each parameters for each file, index increase for each file,
    //eg, pars 0,1,2 for file 0, and then pars 3,4,5 for file 1,...etc.
    for(short p = f*pF; p < (f+1)*pF ; p++)
    {
        //This return s if the variable is shared (share=1), otherwise returns p
        s =  p - ParShare[p]*pF*f;
        par.push_back(pars.at(s));
    }
    return par;
}

//Calculate the chisquared for each file, sums them up for global fit
double MODEL::operator()(const std::vector<double>& pars) const
{
    double chisquared = 0.0;
    for(short f = 0; f <  nbFiles ; f++)
    {
        //Construct pars for each file
        std::vector<double> parf = getParList(f,pars);

        //computes chisquared for each file
        chisquared += Chisquared(xData[f],yData[f],parf);
    }
    return chisquared;
}

//calculates the chisquared
double MODEL::Chisquared(QVector<QVector<double> > X,
                         QVector<QVector<double> > Y,
                         vector<double> par)const
{
    double chiSq= 0.0;
    for(short n = 0; n < Y[0].size(); n++)
    {
        chiSq   +=   pow( (Fct(X[0][n],par) - Y[0][n]) /error(X,Y,par)[n],2.0);
    }
    return chiSq;
}

//calculates the weighting factors for x and y
QVector<double> MODEL::error(QVector<QVector<double> > X,
                             QVector<QVector<double> > Y,
                             vector<double> par)const
{
    QVector<double> errVec;
    double  xerror = 0.0;
    double  yerror = ( X.size() > 1 )? 0.0 : 1.0;
    for(short n = 0; n < Y[0].size(); n++)
    {
        if( Y.size() == 2 )
            yerror = fabs(Y[1][n]);
        else if( Y.size() == 3 )
            yerror = 0.5*(fabs(Y[1][n])+fabs(Y[2][n]));
        if( X.size() == 2 )
            xerror = fabs(X[1][n]);
        else if( X.size() == 3 )
            xerror = 0.5*(fabs(X[1][n])+fabs(X[2][n]));
        errVec.push_back(yerror
                         + 0.25*(Fct(X[0][n] + xerror,par)
                         -Fct(X[0][n] - xerror,par) ) );
    }
    return errVec;
}

//Contstrucs the user parameters for minuit.
//Must follow the same order as in chisquared
MnUserParameters FIT::userParameters(int nbFiles, QVector<QString> parLabel,QVector<int> parShare,
                                     QVector<int> parFix,QVector<double> parValue,
                                     QVector<double> parErrValue,QVector<double> parMin,
                                     QVector<double> parMax)
{
    //nb total of pars
    short pT = parLabel.size();
    //Number of parameters per function.
    short pF = pT/nbFiles;

    MnUserParameters upar;
    for (short p = 0; p <  pT ; p++)
    {
        short f = (short)(p/pF);
        if (parFix[p] == 0)
        {
            upar.Add(parLabel[p].toStdString().c_str(),
                     parValue[p],parErrValue[p],parMin[p],parMax[p]);
        }
        else if (parFix[p] == 1 || ( parShare[p] == 1 && f > 0))
        {
            upar.Add(parLabel[p].toStdString().c_str(),parValue[p]);
        }
    }
    return upar;
}

//Fitting data with fct, and returning the results.
void FIT::PARAMETERS(QVector<double> &parValue,
                     QVector<double> &parErrNeg,QVector<double> &parErrPos,
                     QVector<double> &chisq,
                     QVector<double> parMin,QVector<double> parMax,
                     QVector<int> parFix, QVector<int> parShare)
{
    double chi = 0;
    //Nb of files to fit
    short nbFiles = yData.size();
    //total number of parameters.
    short pT = parValue.size();
    //Number of parameters per function.
    short pF = pT/nbFiles;

    //constructs user parameters
    MnUserParameters upar;
    if (Global==true)
    {
        chisq.clear();
        //construct labels (numbers) for each parameter;
        QVector<QString> label;
        for(short p = 0; p < pT ; p++)
            label<<QString::number(p);
        upar =  userParameters(nbFiles,label,parShare,parFix,parValue,parErrNeg,parMin,parMax);
        START(upar,Fct,parShare,parFix,xData,yData,parValue,parErrNeg,parErrPos,chi);
        short  degFreedom = 0;
        for (short f = 0; f < nbFiles; f++)
            degFreedom += xData[f][0].size();
        for (short f = 0; f < nbFiles; f++)
            chisq.push_back(chi/degFreedom);
    }
    else
    {
        chisq.clear();
        for (short f = 0; f < nbFiles; f++)
        {
            QVector<QString> label;
            QVector<int>     share,fix;
            QVector<double>  value,errn,errp,min,max;
            for(short p = f*pF; p < (f+1)*pF ; p++)
            {
                label<<QString::number(p);//label<<parLabel[p];
                share<<parShare[p];
                fix  <<parFix[p];
                value<<parValue[p];
                errn <<parErrNeg[p];
                errp <<parErrNeg[p];
                min  <<parMin[p];
                max  <<parMax[p];
            }
            //use this to start with values of previous fit
            if(Loop && f > 0 )
            {
                value.clear();
                for(short p = (f-1)*pF; p < f*pF ; p++)
                    value<<parValue[p];
            }
            QVector<QVector<QVector<double> > > x,y;
            x.push_back(xData.at(f));
            y.push_back(yData.at(f));
            upar =  userParameters(1,label,share,fix,value,errn,min,max);
            START(upar,Fct,share,fix,x,y,value,errn,errp,chi);
            for(short p = 0; p < pF ; p++)
            {
                parValue[pF*f+p]  = value[p];
                parErrNeg[pF*f+p] = errn[p];
                parErrPos[pF*f+p] = errp[p];
            }
            chisq.push_back(chi/xData[f][0].size());
        }
    }
}

//Fitting data with fct, and returning the results.
bool FIT::START(MnUserParameters upar,
                function Function,
                QVector<int> Share,
                QVector<int> Fix,
                QVector<QVector<QVector<double> > > X,
                QVector<QVector<QVector<double> > > Y,
                QVector<double> &Value,
                QVector<double> &ErrM,
                QVector<double> &ErrP,
                double &Chisq)
{
    //constructs the model function and calculates Chisquared
    MODEL FCN(Function,X,Y);

    //Nb of files to fit
    int nbFiles = X.size();
    //total number of parameters.
    int pT = Value.size();
    //Number of parameters per function.
    int pF = pT/nbFiles;

    //Setting of the function
    FCN.setErrorDef(1.0); //means sigma errors, check minuit manual for a description
    FCN.setNbFiles(nbFiles);
    FCN.setNbPars(pT);
    FCN.setNbParsFct(pF);
    FCN.setNbSharedPars(Share);

    //migrad
    MnMigrad  migrad(FCN, upar);
    FunctionMinimum   min =  migrad();

    if( !min.IsValid() )
    {
        MnMigrad migrad2(FCN, upar, 2);
        min = migrad2();
        if( !min.IsValid() )
        {
            qDebug()<<"Migrad failed, try simplex method, and then revert back.\n";
            MnSimplex simplex(FCN, upar);
            min = simplex();
        }
    }

    if( min.IsValid() )
    {
        Value.clear();
        ErrM.clear();
        ErrP.clear();
        Chisq  =  min.Fval();

        double parVal = 0.0, parmErr = 0.0, parpErr = 0.0;
        short  f  = 0, s = 0;
        MnMinos minosErrors(FCN,min);
        for (short p = 0; p < pT ; p++)
        {
            //index of the current file
            f = (short)(p/pF);

            //This is used to find the index of the global parameters.
            s = p - Share[p]*pF*f;

            //get the value of the parameters
            parVal = min.UserState().Value(s);
            Value.push_back(parVal);

            if     (   Fix[p] != 1 && Error == "Minos"  )
            {
                parmErr = minosErrors(s).first;
                parpErr = minosErrors(s).second;
            }
            else if (  Fix[p] != 1 && Error == "Migrad")
            {
                parmErr = min.UserState().Error(s);
                parpErr = min.UserState().Error(s);
            }
            else if ( Fix[p] == 1)
            {
                parmErr = 0.0;
                parpErr = 0.0;
            }
            ErrM.push_back(parmErr);
            ErrP.push_back(parpErr);
        }
    }
    else
        qDebug()<<"WARNING: No minimum, minuit gave up!";
    return min.IsValid();
}
