/***************************************************************************
*  File: plotfigures.cpp                                                   *
*  Description: reads and makes 2D plots out of the fitting results files. *
*  Author: Hassan Saadaoui                                                 *
*  e-mail: saadaoui@triumf.ca                                              *
*  $Id: mainwindow.h 2014-12-20 $                                          *
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

#ifndef PAGE_RESULTS_H
#define PAGE_RESULTS_H

#include <QWidget>
#include <QDialog>
#include <QLabel>
#include <QComboBox>
#include <QDialogButtonBox>

namespace Ui {
class Results;
}

class Results : public QWidget
{
    Q_OBJECT

public:
    explicit Results(QWidget *parent = 0);
    ~Results();

private slots:
    void on_lineEdit_filename_returnPressed();

    void on_pushButton_browse_clicked();

    void on_pushButton_plot_clicked();

    void on_pushButton_clear_clicked();

    void on_tabWidget_tabCloseRequested(int index);

    void on_pushButton_purge_clicked();

    //void on_pushButton_optional_clicked();

    void on_pushButton_next_clicked();

    void on_lineEdit_filename_textChanged(const QString &arg1);

private:
    Ui::Results *resGUI;
    int plotxerr=2,plotyerr=2,plotxlin=0,plotylin=0;

};


/*Needed for options for graphics. Disabled now*/
/*
class optionalDialog : public QDialog
{
    Q_OBJECT

public:
    optionalDialog(int,int,int,int,QWidget *parent = 0);
    int getXerr(void);
    int getYerr(void);
    int getXlin(void);
    int getYlin(void);


private slots:
    void setNewValues(void);


private:
    void createLayout(int,int,int,int);
    QDialogButtonBox *buttonBox;
    QLabel    *xLabel,*xerrLabel,*xlinLabel;
    QLabel    *yLabel,*yerrLabel,*ylinLabel;
    QComboBox *xerrComboBox,*xlinComboBox;
    QComboBox *yerrComboBox,*ylinComboBox;
    int xerr=2,yerr=2,xlin=0,ylin=0;
};
*/

#endif // PAGE_RESULTS_H
