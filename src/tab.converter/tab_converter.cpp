/***************************************************************************
*  File: tab_converter.cpp                                                *
*  Description: convertes between different units                          *
*  Author: Hassan Saadaoui                                                 *
*  e-mail: saadaoui@triumf.ca                                              *
*  $Id: tab_converter.cpp 2014-12-20 $                                    *
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
#include "tab_converter.h"
#include "ui_tab_converter.h"

Converter::Converter(QWidget *parent) : QWidget(parent), conGUI(new Ui::Converter)
{
    conGUI->setupUi(this);
}

Converter::~Converter()
{
   delete conGUI;
}
void Converter::on_lineEdit_field_Li_8_returnPressed()
{
   double GMRatio =  0.63015;//kHz/G_ff
   double input   = conGUI->lineEdit_field_Li_8 -> text().toDouble();
   double result  = input * GMRatio;
   conGUI->lineEdit_frequency_Li_8 -> setText( QString::number( result,'f',2) );
}
void Converter::on_lineEdit_frequency_Li_8_returnPressed()
{
    double GMRatio =  0.63015;//kHz/G_ff
    double input   = conGUI->lineEdit_frequency_Li_8 -> text().toDouble();
    double result  = input / GMRatio;
    conGUI->lineEdit_field_Li_8 -> setText( QString::number( result,'f',2) );
}

void Converter::on_lineEdit_field_helmoltz_returnPressed()
{
    double input   = conGUI->lineEdit_field_helmoltz -> text().toDouble();
    double result  = (input-0.175)/2.2131;
    conGUI->lineEdit_current_helmoltz -> setText( QString::number( result,'f',2) );
}

void Converter::on_lineEdit_current_helmoltz_returnPressed()
{
    double input   = conGUI->lineEdit_current_helmoltz -> text().toDouble();
    double result  = 2.2131*input + 0.175;
    conGUI->lineEdit_field_helmoltz -> setText( QString::number(result,'f',2) );
}

void Converter::on_lineEdit_duration_insech_returnPressed()
{
    double input  = conGUI->lineEdit_duration_insech -> text().toDouble();
    double result =  5*10000/3.14/input;
    conGUI->lineEdit_bandwidth_insech -> setText( QString::number(result,'f',2) );
}

void Converter::on_lineEdit_bandwidth_insech_returnPressed()
{
    double input  = conGUI->lineEdit_bandwidth_insech -> text().toDouble();
    double result =  5*10000/3.14/input;
    conGUI->lineEdit_duration_insech -> setText( QString::number(result,'f',2) );
}

void Converter::on_lineEdit_duration_hermite_returnPressed()
{
    double input   = conGUI->lineEdit_duration_hermite -> text().toDouble();
    double result =  1764.8/input;
    conGUI->lineEdit_bandwidth_hermite -> setText( QString::number(result,'f',2) );
}

void Converter::on_lineEdit_bandwidth_hermite_returnPressed()
{
    double input   = conGUI->lineEdit_bandwidth_hermite -> text().toDouble();
    double result =  1764.8/input;
    conGUI->lineEdit_duration_hermite -> setText( QString::number(result,'f',2) );
}

void Converter::on_lineEdit_frequency_muon_returnPressed()
{
    double GMRatio = 85.16;//kHz/G_ff
    double input   = conGUI->lineEdit_frequency_muon -> text().toDouble();
    double result  = input / GMRatio;
    conGUI->lineEdit_field_muon -> setText( QString::number( result,'f',2) );
}

void Converter::on_lineEdit_field_muon_returnPressed()
{
    double GMRatio =  85.16;//kHz/G_ff
    double input   = conGUI->lineEdit_field_muon -> text().toDouble();
    double result  = input * GMRatio;
    conGUI->lineEdit_frequency_muon -> setText( QString::number( result,'f',2) );
}
