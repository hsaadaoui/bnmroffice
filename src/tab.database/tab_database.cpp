/***************************************************************************
*  File: tab_database.cpp                                                 *
*  Description: interactes with the ui for database                        *
*  Author: Hassan Saadaoui                                                 *
*  e-mail: saadaoui@triumf.ca                                              *
*  $Id: tab_database.cpp   2014-12-20 $                                   *
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
#include "tab_database.h"
#include "ui_tab_database.h"

Database::Database(QWidget *parent) :
    QWidget(parent),
    dbGUI(new Ui::Database)
{
    dbGUI->setupUi(this);
}

Database::~Database()
{
    delete dbGUI;
}

//closes a db connection
void Database::closeConnection()
{
    mydatabase.close();
    mydatabase.removeDatabase(QSqlDatabase::defaultConnection);
}


//establishes a db connection
bool Database::openConnection(QString dataBase)
{
    bool openState = false;

    if(QSqlDatabase::contains("qt_sql_default_connection"))
        mydatabase = QSqlDatabase::database("qt_sql_default_connection");
    else
        mydatabase = QSqlDatabase::addDatabase("QSQLITE");

    mydatabase.setDatabaseName(dataBase);

    openState  = mydatabase.open();

    if (!openState)
    {
        QMessageBox::critical(this, tr("Failure"),
                              tr("Failed to connect to the database."));
    }
    if (openState)
    {
        dbGUI->comboBox_tables->clear();
        listofTables = mydatabase.tables(QSql::Tables);
        listofTables << "create a new table";
        if (listofTables.size() > 0)
        {
            dbGUI->comboBox_tables->insertItems(0,listofTables);
        }
        else
        {
            QMessageBox::warning(this,tr("Warning"),
                                 tr("No tables are found in your database."));
            qDebug()<<"No tables are found in your database:\n"<<dataBase;
        }
    }
    return openState;
}

//fill the table with sqlquerry
void Database::fillTable(QSqlTableModel *model)
{
    model -> setTable (dbGUI->comboBox_tables->currentText());
    model -> setEditStrategy (QSqlTableModel :: OnManualSubmit);
    model -> select();
    dbGUI->tableView_results->setModel(model);
    //dbGUI->tableView_results->resizeColumnsToContents ();
    dbGUI->tableView_results->horizontalHeader()
            ->setSectionResizeMode(QHeaderView::Stretch);
}

//updates the lineedit based on which table the user chooses out of the combobox
void Database::on_comboBox_tables_currentIndexChanged(const QString &argument)
{
    if (dbGUI->comboBox_tables->count() > 0)
    {
        if (connectionState)
        {
            tableModel  = new QSqlTableModel();
            if(argument != "create a new table" && argument != "Select:")
            {
                QString text = "SELECT * FROM "+ argument;
                dbGUI->lineEdit_qry->setText(text);
                fillTable(tableModel);
            }
            if(argument == "create a new table")
            {
                QString text = "CREATE TABLE tableName (id,age,name)";
                dbGUI->lineEdit_qry->setText(text);
                QMessageBox::information(this,tr("Success"),
                                         tr("You must carefully follow the examples"
                                            "to generate a functional db table."
                                            "It is best to use SQLite Manager"
                                            "(add on browsers) to create a new table."));
            }
        }
        else
            if(argument == "create a new table")
                QMessageBox::critical(this,tr("Error"),"Must select a database");

    }
}

//signal executes the query made by the user. Creating tables is tricky.
void Database::on_pushButton_execute_clicked()
{
    QSqlQuery executeQuery;
    QString userInput = dbGUI->lineEdit_qry->text();
    executeQuery.prepare(userInput);
    QSqlQueryModel *model = new QSqlQueryModel();

    if (executeQuery.exec())
    {
        model->setQuery(executeQuery);
        if (dbGUI->comboBox_tables->currentText() !=  "create a new table")
        {
            dbGUI->tableView_results->setModel(model);
            dbGUI->tableView_results->resizeColumnsToContents ();
            dbGUI->tableView_results->horizontalHeader()
                    ->setSectionResizeMode(QHeaderView::Stretch);
        }
        else
        {
            listofTables     = mydatabase.tables(QSql::Tables);
            QString newtable = listofTables.at(listofTables.size()-1);
            dbGUI->comboBox_tables->addItem(newtable);
            dbGUI->comboBox_tables->setCurrentIndex(dbGUI
                                                    ->comboBox_tables->count()-1);
        }
    }
    else
    {
        qDebug()<<"The query could not be executed; is the syntax correct?";
        QMessageBox::critical(this,tr("Error with your Query"),
                              executeQuery.lastError().text());
    }
}

//append an emty row at the end of the table
void Database::on_pushButton_add_clicked()
{
    int rowNum = tableModel->rowCount();  // get the number of rows of the table
    tableModel->insertRow(rowNum);
    tableModel-> submitAll();     // submit row addition.
}

void Database::on_pushButton_save_clicked()
{
    if (tableModel->submitAll())
    {
        tableModel->database().commit();   // submit
    }
    else
    {
        tableModel->database().rollback(); // rollback
        QMessageBox::warning (this, tr ("Critical limit"),
                              tr ("Many connections to SQLITE Database.\n"
                                  "Only one user can make modifications to the db.\n"
                                  "It is a shortcoming of SQLITE software.\n"
                                  "Access to db is allowed but not changes."),
                              tableModel->lastError ().text());
    }
}

//delete entire row and let the user decide if he really wants to do  it.
void Database::on_pushButton_delete_clicked()
{
    int curRow = dbGUI->tableView_results-> currentIndex().row();
    if(curRow<0)
        QMessageBox::critical(this,tr("Error"),"Must select a row.");
    else
    {
        int ok = QMessageBox::warning(this, tr("Delete"),tr("Are you sure to delete row %1?").
                                      arg(QString::number(curRow+1)),
                                      QMessageBox::Yes,QMessageBox::No);
        if (ok == QMessageBox::Yes)
        {
            tableModel-> removeRow(curRow);   // Delete the row
            tableModel-> submitAll();     // otherwise submit to delete the row in the database
        }
    }
}

//open a specific selected cell
void Database::on_pushButton_open_clicked()
{
    //get row and col index of what cell was selected/pressed.
    int curRow = dbGUI->tableView_results-> currentIndex().row();
    int curCol = dbGUI->tableView_results-> currentIndex().column();
    //pass the index to model view to get the item locted at that cell
    QModelIndex index = tableModel->index(curRow, curCol, QModelIndex());
    QString toOpen = dbGUI->tableView_results->model()->data(index).toString();
    if (QFile(toOpen).exists()) //if that thing is a file, and exists do this
    {
        QString fileEnding = QFileInfo(toOpen).suffix().toLower();
        if(fileEnding == "jpg" || fileEnding == "jpeg"
                || fileEnding == "bmp" ||fileEnding == "png"
                || fileEnding =="eps" || fileEnding =="ps")
        {
            QGraphicsScene      *scene = new QGraphicsScene();
            scene->addPixmap(QPixmap(toOpen));
            QGraphicsView       *view  = new QGraphicsView(scene);
            view->fitInView(scene->sceneRect(),Qt::KeepAspectRatio);
            view->show();
        }
        else
        {
            QDesktopServices::openUrl(QUrl(toOpen));
        }
    }
    else //otherwise use a text widget to display the string in that cell.
    {
        QGraphicsScene *scene = new QGraphicsScene();
        scene->addText(toOpen);
        QGraphicsView *view  = new QGraphicsView(scene);
        view->show();
    }
}

void Database::on_comboBox_browsedb_clicked()
{

    if(lastDir.isEmpty())
        lastDir = QDir::currentPath();
    QString filename = QFileDialog::getOpenFileName(this,tr("Open Database"),
                                                    lastDir,tr("Database (*.sqlite)"));
    lastDir =  QFileInfo(filename).absolutePath();
    QDir dir(QDir::currentPath());
    QString relName = dir.relativeFilePath(filename);
    bool dbExists = false;
    int i=0;
    while(i < dbGUI->comboBox_db->count()&& !dbExists)
    {
        if(relName == dbGUI->comboBox_db->itemText(i))
            dbExists = true;
        i++;
    }
    if(!dbExists)
        dbGUI->comboBox_db->addItem(relName);
    connectionState = openConnection(filename);
}

void Database::on_comboBox_db_currentIndexChanged(const QString &filename)
{
    connectionState = openConnection(filename);
}
