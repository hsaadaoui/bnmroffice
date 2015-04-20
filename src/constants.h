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
TYPE  DATE        AUTHOR  DESCRIBE
***************************************************************************/
#ifndef CONSTANTS_H_
#define CONSTANTS_H_
#include<QString>

//You must hange these according to where your data is stored.
//Each folder must have subfolders for each year; eg 2014/ 2015 ..etc
//The files contained in the year subfolders must be named as 04xxxx.msr
const QString bnmrDataPath   = "/media/hassan/physics/triumf/data/bnmr/";
const QString bnqrDataPath   = "/media/hassan/physics/triumf/data/bnqr/";

//Root folder where the fit fcts are stored. PLEASE DO NOT CHANGE.
//IF YOU DO, Make SURE the Fct libraries are copied to the new fctPath
const QString fctPath        = "/opt/bnmroffice/fct/";

#endif
