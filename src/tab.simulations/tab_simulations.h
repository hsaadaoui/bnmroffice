/***************************************************************************
*  File: tab_simulations.cpp                                              *
*  Description: reads and makes 2D plots out of the fitting results files. *
*  Author: Hassan Saadaoui                                                 *
*  e-mail: saadaoui@triumf.ca                                              *
*  $Id: tab_simulations.cpp 2014-12-20 $                                  *
*  Copyright (C) 2015 by Hassan Saadaoui                                   *
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
#ifndef PAGE_SIMULATIONS_H
#define PAGE_SIMULATIONS_H

#include <QWidget>
#include <QHeaderView>
#include <QTableWidget>
#include <QFileDialog>
#include <QDir>
#include <QFileInfo>

namespace Ui {
class Simulations;
}

class Simulations : public QWidget
{
    Q_OBJECT

public:
    explicit Simulations(QWidget *parent = 0);
    ~Simulations();

private slots:
    void on_Vleck_pushButton_start_clicked();

    void on_Vleck_pushButton_export_clicked();

    void on_energygap_pushButton_start_clicked();

    void on_energygap_pushButton_export_clicked();

    void on_energygap_comboBox_function_currentIndexChanged(const QString &arg1);

    void energygap_change_window();

    void on_Vleck_pushButton_clear_clicked();

    void on_Vleck_pushButton_purge_clicked();

    void on_energygap_pushButton_clear_clicked();

    void on_energygap_pushButton_purge_clicked();

private:
    Ui::Simulations *simGUI;
};

#endif // PAGE_SIMULATIONS_H
