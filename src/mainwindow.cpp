/***************************************************************************
*  File: mainwindow.cpp                                                    *
*  Description: invokes the gui                                            *
*  Author: Hassan Saadaoui                                                 *
*  e-mail: saadaoui@triumf.ca                                              *
*  $Id: mainwindow.cpp 2014-12-20 $                                        *
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

#include <iostream>                 //Header file for outputting data
#include <fstream>                  //Header file for outputting data
#include <math.h>
#include <string>
#include <stdio.h>
using namespace std;
#include<QStringList>
#include<QWebView>
#include<QMap>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "constants.h"
#include "plotting.h"


Mainwindow::Mainwindow(QWidget *parent) :
    QMainWindow(parent),
    mainGUI(new Ui::Mainwindow)
{
    mainGUI->setupUi(this);
    createGraphics();
    createActions();
    createMenus();
    checkCurrentStyle();
    createPages();
}

Mainwindow::~Mainwindow()
{
    delete mainGUI;
}

void Mainwindow::createGraphics()
{
    //Set Window icon and size
    //setWindowIcon(QIcon(":/images/logo1.png"));
    setWindowIcon(QIcon(":/images/logo2.png"));
    resize(QSize(720, 920));
    //window background
    setStyleSheet("QLineEdit, QComboBox, QSpinBox "
                  "{background-color:rgba(246,246,246,255);color:black}"
                  "QGroupBox {border: 1px solid gray;border-radius: 5px; margin-top: 1ex;}"
                  "QFrame {background-color:rgba(171,224,137,255);}");
}


void Mainwindow::createPages()
{
    QLabel *owner = new QLabel("BnmrOffice 2015 (c) Hassan Saadaoui");
    statusBar()->addWidget(owner);
    //statusBar()->showMessage(tr("BnmrOffice 2015 (c) Hassan Saadaoui"),0);
    pagesTabWidget  = new QTabWidget;
    pagesTabWidget->setObjectName("pagesTabWidget");
    pagesTabWidget->setIconSize(QSize(40,40));

    Search *tab_search = new Search;
    pagesTabWidget->addTab(tab_search, QIcon(":/images/search.png"), QString("Search  "));

    Analysis *tab_analysis = new Analysis;
    pagesTabWidget->addTab(tab_analysis, QIcon(":/images/analyze.png"), QString("Analysis "));

    Results *tab_results = new Results;
    pagesTabWidget->addTab(tab_results,  QIcon(":/images/results.png"),QString("Results"));

    Database *tab_database = new Database;
    pagesTabWidget->addTab(tab_database,  QIcon(":/images/database.png"),QString("Database"));

    Simulations     *tab_simulations = new Simulations;
    pagesTabWidget->addTab(tab_simulations,  QIcon(":/images/simulation.png"),QString("Simulations"));

    Converter *tab_converter = new Converter;
    pagesTabWidget->addTab(tab_converter,  QIcon(":/images/converter.png"),QString("Converter"));

    this->setCentralWidget(pagesTabWidget);
}

void Mainwindow::Menu_newgui()
{
    popen("./bnmroffice","w");
}

void Mainwindow::Menu_oldgui()
{
    popen("bnmroffice.v1","w");
}

void Mainwindow::Menu_clean()
{
    popen("rm temp.* -f","w");
}

void Mainwindow::Menu_editPath()
{
    PathDialog dialog;
    dialog.exec();
}


void Mainwindow::Stream_window()
{
    streamDialog = new Stream;
    streamDialog->show();
    streamDialog->raise();
    streamDialog->activateWindow();
}

void Mainwindow::Menu_changeStyle(bool checked)
{
    if (!checked)
    {
        return;
    }
    QAction *action = qobject_cast<QAction *>(sender());
    QStyle *style = QStyleFactory::create(action->data().toString());
    Q_ASSERT(style);
    QApplication::setStyle(style);
}

void Mainwindow::checkCurrentStyle()
{
    foreach (QAction *action, styleActionGroup->actions()) {
        QString styleName = action->data().toString();
        QStyle *candidate = QStyleFactory::create(styleName);
        Q_ASSERT(candidate);
        if (candidate->metaObject()->className()
                == QApplication::style()->metaObject()->className()) {
            action->trigger();
            return;
        }
        delete candidate;
    }
}

void Mainwindow::Menu_changeColor(bool checked)
{
    if (!checked)
    {
        return;
    }
    QAction *action = qobject_cast<QAction *>(sender());
    QString color = action->data().toString();

    if (color=="olive")
    {
        setStyleSheet("background-color: rgba(209,210,140,255);");
    }
    else if (color=="green")
    {
        setStyleSheet("background-color: rgba(171,224,137,255);");
    }
    else if (color=="gray+white")
    {
        setStyleSheet("QLineEdit, QComboBox, QSpinBox "
                      "{background-color:rgba(246,246,246,255);color:black}"
                      "QGroupBox {border: 1px solid gray;border-radius: 5px; margin-top: 1ex;}"
                      "QFrame {background-color:darkGray}");
    }
    else if (color=="olive+white")
    {
        setStyleSheet("QLineEdit, QComboBox, QSpinBox "
                      "{background-color:rgba(246,246,246,255);color:black}"
                      "QGroupBox {border: 1px solid gray;border-radius: 5px; margin-top: 1ex;}"
                      "QFrame {background-color:rgba(209,210,140,255)}");
    }
    else if (color=="green+white")
    {
        setStyleSheet("QLineEdit, QComboBox, QSpinBox "
                      "{background-color:rgba(246,246,246,255);color:black}"
                      "QGroupBox {border: 1px solid gray;border-radius: 5px; margin-top: 1ex;}"
                      "QFrame {background-color:rgba(171,224,137,255);}");
    }
    else
        setStyleSheet("QLineEdit, QComboBox, QSpinBox "
                      "{background-color:rgba(246,246,246,255);color:black}"
                      "QGroupBox {border: 1px solid gray;border-radius: 5px; margin-top: 1ex;}"
                      "QFrame {background-color:rgba(171,224,137,255);}");
}

void Mainwindow::createActions()
{
    show_stream_window = new QAction(QIcon(":/images/live.png"), tr("&Show streaming window"), this);
    connect(show_stream_window, SIGNAL(triggered()),this, SLOT(Stream_window()));

    Action_file_old = new QAction(QIcon(":/images/open.png"), tr("&Open gui.v1"), this);
    Action_file_old->setShortcut(tr("Ctrl+O"));
    Action_file_old->setStatusTip(tr("Open the old BnmrOffice"));
    connect(Action_file_old, SIGNAL(triggered()), this, SLOT(Menu_oldgui()));

    Action_file_new = new QAction(QIcon(":/images/open.png"), tr("&Open gui.v2"), this);
    Action_file_new->setShortcut(tr("Ctrl+N"));
    Action_file_new->setStatusTip(tr("Open a new BnmrOffice GUI"));
    connect(Action_file_new, SIGNAL(triggered()), this, SLOT(Menu_newgui()));

    Action_file_exit = new QAction(QIcon(":/images/exit.png"),tr("&Quit"), this);
    Action_file_exit->setShortcut(tr("Ctrl+Q"));
    Action_file_exit->setStatusTip(tr("Close this GUI"));
    connect(Action_file_exit, SIGNAL(triggered()), this, SLOT(close()));

    Action_file_clean = new QAction(QIcon(":/images/clean.png"), tr("&Clean"), this);
    Action_file_clean->setStatusTip(tr("Clean all temp.* files"));
    connect(Action_file_clean, SIGNAL(triggered()), this, SLOT(Menu_clean()));

    styleActionGroup = new QActionGroup(this);
    foreach (QString styleName, QStyleFactory::keys()) {
        QAction *action = new QAction(styleActionGroup);
        action->setText(tr("%1 Style").arg(styleName));
        action->setData(styleName);
        action->setCheckable(true);
        connect(action,SIGNAL(triggered(bool)),this, SLOT(Menu_changeStyle(bool)));
    }

    colorActionGroup = new QActionGroup(this);
    QStringList colors;
    colors <<"default"<<"olive"<<"green"<<"olive+white"<<"gray+white"<<"green+white";
    foreach (QString colorName, colors) {
        QAction *action = new QAction(colorActionGroup);
        action->setText(tr("%1").arg(colorName));
        action->setData(colorName);
        action->setCheckable(true);
        connect(action,SIGNAL(triggered(bool)),this, SLOT(Menu_changeColor(bool)));
    }

    Action_edit_paths = new QAction(tr("&Data path"), this);
    Action_edit_paths->setShortcut(tr("Ctrl+P"));
    connect(Action_edit_paths, SIGNAL(triggered()),
            this, SLOT(Menu_editPath()));

    Action_graphics_xmgr   = new QAction(tr("&Plot with XMGR"), this);
    Action_graphics_xmgr->setCheckable(true);
    //connect(Action_graphics_xmgr,SIGNAL(toggled(bool)),this, SLOT(slotXmgr(bool)));

    Action_graphics_qcp   = new QAction(tr("&Plot with QCP"), this);
    Action_graphics_qcp->setCheckable(true);

    Action_graphics_keepPlots   = new QAction(tr("&Keep Plots"), this);
    Action_graphics_keepPlots->setCheckable(true);
    connect(Action_graphics_keepPlots,SIGNAL(toggled(bool)),this, SLOT(Menu_keepPlots(bool)));

    Action_graphics_combinePlots= new QAction(tr("&Combine Plots"), this);
    Action_graphics_combinePlots->setCheckable(true);
    connect(Action_graphics_combinePlots,SIGNAL(toggled(bool)),this, SLOT(Menu_combinePlots(bool)));

    Action_graphics_keepAscii= new QAction(tr("&Keep Ascii files"), this);
    Action_graphics_keepAscii->setCheckable(true);
    connect(Action_graphics_keepAscii,SIGNAL(toggled(bool)),this, SLOT(Menu_keepAscii(bool)));

    Action_tutorial = new QAction(QIcon(":/images/tuto.png"),tr("&Tutorial"), this);
    Action_tutorial->setShortcut(tr("Ctrl+T"));
    connect(Action_tutorial, SIGNAL(triggered()), this, SLOT(Menu_tutorial()));

    Action_tips = new QAction(QIcon(":/images/tips.png"),tr("&Tips"), this);
    connect(Action_tips, SIGNAL(triggered()), this, SLOT(Menu_help()));

    Action_about = new QAction(QIcon(":/images/logo2.png"),tr("&About"), this);
    connect(Action_about, SIGNAL(triggered()), this, SLOT(Menu_about()));
}


void Mainwindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addSeparator()->setText(tr("Open BnmrOffice"));
    fileMenu->addAction(Action_file_old);
    fileMenu->addAction(Action_file_new);
    fileMenu->addSeparator()->setText(tr("Clean temp*. files"));
    fileMenu->addAction(Action_file_clean);
    fileMenu->addSeparator()->setText(tr("Close GUI"));
    fileMenu->addAction(Action_file_exit);

    menuBar()->addSeparator();

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(Action_edit_paths);

    viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(show_stream_window);
    viewMenu->addSeparator()->setText(tr("Style"));
    foreach (QAction *action, styleActionGroup->actions())
        viewMenu->addAction(action);

    viewMenu->addSeparator()->setText(tr("Color"));
    foreach (QAction *action, colorActionGroup->actions())
        viewMenu->addAction(action);

    viewMenu->addSeparator();

    graphicsMenu = menuBar()->addMenu(tr("&Plotting"));
    // graphicsMenu->addAction(Action_graphics_xmgr);
    // graphicsMenu->addAction(Action_graphics_qcp);
    graphicsMenu->addAction(Action_graphics_keepPlots);
    graphicsMenu->addAction(Action_graphics_combinePlots);
    graphicsMenu->addAction(Action_graphics_keepAscii);

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(Action_about);
    helpMenu->addAction(Action_tips);
    helpMenu->addAction(Action_tutorial);
}

bool  Mainwindow::combinePlots = false;

bool  Mainwindow::keepPlots = false;

bool  Mainwindow::keepAscii = false;

void Mainwindow::Menu_combinePlots(bool state)
{
    combinePlots = state;
}

void Mainwindow::Menu_keepPlots(bool state)
{
    keepPlots = state;
}

void Mainwindow::Menu_keepAscii(bool state)
{
    keepAscii = state;
}



void Mainwindow::Menu_about()
{
    QMessageBox::about(this, tr("About Menu"),
                       tr("<center><font color=red><b>BnmrOffice [version 15.03.2015]</b></font><br>"
                          "Written in C/C++ and Qt 5.<br>"
                          "<br><b><font color=red>Developped by Hassan Saadaoui.</b></font><br>"
                          "For any questions, please contact:<br>"
                          "saadaoui@triumf.ca<br><br>"
                          "Please<b><font color=red> acknowledge the author</b></font> in your<br>"
                          "work, if you have used the program offline."));
}

void Mainwindow::Menu_help()
{
    QMessageBox::about(this, tr("Tips"),
                       tr("Hover the mouse index onto of the labels\n"
                          "to get some tips"));
}

void Mainwindow::Menu_tutorial()
{
    bool openState = QDesktopServices::openUrl(QUrl("/opt/bnmroffice/doc/tutorial.html"));
    bool openState2 = QDesktopServices::openUrl(QUrl("/opt/bnmroffice/doc/tutorial.pdf"));
    if (openState == false)
    {
        openState = QDesktopServices::openUrl(QUrl("qrc:/doc/tutorial.html"));
        if (openState == false)
        {
            qDebug()<<"Error: cannot find the tutorial page";
        }
    }
}

QString dataPath(QString type)
{
    if ( type == "bnmr" ) return bnmrDataPath;
    if ( type == "bnqr" ) return bnqrDataPath;
}

PathDialog::PathDialog(QWidget *parent)
    : QDialog(parent)
{
    createLayout();
    pathComboBox->addItem("bnmr");
    pathComboBox->addItem("bnqr");
}

void PathDialog::update_value(QString path)
{
    pathLineEdit->setText(dataPath(path));
}

void change_path(QString newValue,int index)
{
    QString &tempBpath = const_cast <QString &> (bnmrDataPath);
    QString &tempQpath = const_cast <QString &> (bnqrDataPath);
    switch(index)
    {
    case 0:  tempBpath = newValue;   break;
    case 1:  tempQpath = newValue;  break;
    }
}

void PathDialog::change_value()
{
    int index = pathComboBox->currentIndex();
    QString newValue = pathLineEdit->text();
    change_path(newValue,index);
}

void PathDialog::createLayout()
{
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(change_value()));
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(reject()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    pathLineEdit = new QLineEdit;
    pathLineEdit ->setFixedWidth(400);
    pathComboBox  = new QComboBox;
    pathComboBox->setEditable(false);
    pathComboBox->setFixedWidth(400);
    connect(pathComboBox, SIGNAL(currentTextChanged(QString)),
            this, SLOT(update_value(QString)));
    pathNameLabel  = new QLabel(tr("Type"));
    pathValueLabel = new QLabel(tr("Path"));

    QHBoxLayout *pathNameLayout = new QHBoxLayout;
    pathNameLayout->addWidget(pathNameLabel);
    pathNameLayout->addWidget(pathComboBox);

    QHBoxLayout *pathValueLayout = new QHBoxLayout;
    pathValueLayout->addWidget(pathValueLabel);
    pathValueLayout->addWidget(pathLineEdit);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(pathNameLayout);
    mainLayout->addLayout(pathValueLayout );
    mainLayout->addStretch();
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);
}


