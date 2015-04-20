/***************************************************************************
*  File: mainwindow.h                                                      *
*  Description: classes of mainwidow                                       *
*  Author: Hassan Saadaoui                                                 *
*  e-mail: saadaoui@triumf.ca                                              *
*  $Id: mainwidow.h 2014-12-20 $                                           *
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QPushButton>
#include <QTextBrowser>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QToolButton>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QToolButton>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QActionGroup>
#include <QStandardItemModel>
#include <QDialog>
#include <QMessageBox>
#include <QStyleFactory>
#include <QtGui>
#include <iostream>
#include <QMainWindow>
#include <iostream>
#include <QIcon>
#include <QList>
#include <QPixmap>
#include <QTextStream>
#include <QHBoxLayout>
#include <QColor>
#include <QSpinBox>
#include <QWidgetAction>

#include "dialog_stream.h"
#include "tab_search.h"
#include "tab_analysis.h"
#include "tab_results.h"
#include "tab_simulations.h"
#include "tab_converter.h"
#include "tab_database.h"

namespace Ui {
class Mainwindow;
}

class Mainwindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit Mainwindow(QWidget *parent = 0);
    ~Mainwindow();
    QString dataPath(QString);
    static bool combinePlots;
    static bool keepPlots;
    static bool keepAscii;

private:
    void readSettings();
    void createImagesGroupBox();
    void writeSettings();
    void createActions();
    void createMenus();
    void checkCurrentStyle();
    void createGraphics();
    void createPages();

    Ui::Mainwindow *mainGUI;
    Stream *streamDialog;
    QTabWidget *pagesTabWidget;

    QMenu        *viewMenu;
    QMenu        *fileMenu;
    QMenu        *editMenu;
    QMenu        *fitMenu;
    QMenu        *helpMenu;
    QMenu        *graphicsMenu;

    QActionGroup *styleActionGroup;
    QActionGroup *colorActionGroup;

    QAction    *show_stream_window;
    QAction    *Action_file_new;
    QAction    *Action_file_old;
    QAction    *Action_file_exit;
    QAction    *Action_file_clean;
    QAction    *Action_edit_paths;
    QAction    *Action_graphics_xmgr;
    QAction    *Action_graphics_qcp;
    QAction    *Action_graphics_keepPlots;
    QAction    *Action_graphics_keepAscii;
    QAction    *Action_graphics_combinePlots;
    QAction    *Action_about;
    QAction    *Action_tutorial;
    QAction    *Action_tips;

public slots:
    void Menu_newgui();
    void Menu_oldgui();
    void Menu_clean();
    void Menu_changeStyle(bool);
    void Menu_changeColor(bool);
    void Menu_editPath();
    void Menu_combinePlots(bool);
    void Menu_keepPlots(bool);
    void Menu_keepAscii(bool);
    void Menu_about();
    void Menu_help();
    void Menu_tutorial();
    void Stream_window();
};

class PathDialog : public QDialog
{
    Q_OBJECT

public:
    PathDialog(QWidget *parent = 0);

private slots:
    void   change_value();
    void   update_value(QString);

private:
    void       createLayout();
    QLabel    *pathValueLabel;
    QLineEdit *pathLineEdit;
    QLabel    *pathNameLabel;
    QComboBox *pathComboBox;
    QDialogButtonBox *buttonBox;
};

#endif // MAINWINDOW_H
