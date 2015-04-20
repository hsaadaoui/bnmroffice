/***************************************************************************
*  File: energygap.cpp                                                 *
*  Description: simulates the Superconducting energy gap.                  *
*  Author: Hassan Saadaoui                                                 *
*  e-mail: saadaoui@triumf.ca                                              *
*  $Id: energygap.cpp 2014-12-20 $                                     *
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

#ifndef SIM_GAP_H
#define SIM_GAP_H

class ClassGAP
{
public:
  ClassGAP(vector<double> par, string fct, string gap,string sigmab) :
      Par(par), Fct(fct), Gap(gap), Sigmab(sigmab) {}
  ~ClassGAP() {}
  vector<double> par() const { return Par;   }
  string fct()    const      { return Fct;   }
  string gap()    const      { return Gap;   }
  string sigmab() const      { return Sigmab;}

  double operator()(double x) const;
private:
  vector<double> Par;
  string Fct;
  string Gap;
  string Sigmab;
};
double Bc2(double T,double Tc);
double Hc2(double T,double Tc);
double RhoSwave(double T,double D0,double Tc);
double RhoDwave(double T,double D0,double Tc);
double RhoDirty(double T,double D0,double Tc);
double RhoDwaveNonMono(double T,double D0,double Tc);
double rhoGap(double T,string gapChoice, vector<double> par);
double sigmaBML(double b, double lambda);
double LambdaT(double T,string gapChoice,vector<double> par);
double sigmaBGL(double b, double lambda);
double sigmaB(double B,string sigmaBChoice, vector<double> par);
double sigmaT(double T,string gapChoice, string sigmaBChoice,vector<double> par);
double LambdaT(double T,string gapChoice,vector<double> par);

#endif
