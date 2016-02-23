/***************************************************************************
*  File: constants.h                                                       *
*  Description: constants for internal use                                 *
*  Author: Hassan Saadaoui                                                 *
*  e-mail: saadaoui@triumf.ca                                              *
*  $Id: constants.h  2014-12-20 $                                          *
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
DATE        AUTHOR  DESCRIBE
2016.02.21  Hassan  Added the lebels of the independent variable as used in mud.
***************************************************************************/
#ifndef CONSTANTS_H_
#define CONSTANTS_H_
#include<QString>

//You must hange these according to where your data is stored.
//Each folder must have subfolders for each year; eg 2014/ 2015 ..etc
//The files contained in the year subfolders must be named as 04xxxx.msr

const QString bnmrDataPath   = "/media/hassan/physics/triumf/data/bnmr/";
const QString bnqrDataPath   = "/media/hassan/physics/triumf/data/bnqr/";


//Root folder where the fit functions are stored. PLEASE DO NOT CHANGE.
//IF YOU DO, Make SURE the Fct libraries are copied to the new fctPath

const QString fctPath        = "/opt/bnmroffice/fct/";


//Some default values
//Warning: update these for new types of experiments
#define DefaultBeamEnergy 28.0 //28 keV
#define DefaultNaCellBias 0.10 //0.1 kV
#define MaxBeamEnergy     30   //30keV, just as precaution in case the logged energy is too high.
#define MaxHighField      9    //9 Tesla


//Labels of independet variables as logged in mud files.
//Different labels must be separated by a comma, no space.
#define labels_BNMR_Field      "mag_field"
#define labels_BNQR_Helmotz    "ILE2A1:HH"
#define labels_NaCell_Bias     "ILE2:BIAS15:RDVOL"
#define labels_Platforms_Bias  "BNQR:HVBIAS:RDVOL,BNMR:HVBIAS:POS"
#define labels_Beam_Energy     "ITE:BIAS:RDVOL"
#define labels_Cryo_Temp       "read_B"

#endif
