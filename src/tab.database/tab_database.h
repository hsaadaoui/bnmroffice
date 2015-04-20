#ifndef PAGE_DATABASE_H
#define PAGE_DATABASE_H

#include <QWidget>
#include <QDebug>
#include <QFileInfo>
#include <QMessageBox>
#include <QtSql>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QFileDialog>
#include <QStandardItemModel>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QDesktopServices>

namespace Ui {
class Database;
}

class Database : public QWidget
{

    Q_OBJECT

public:
    explicit Database(QWidget *parent = 0);
    ~Database();

private slots:
    void on_comboBox_tables_currentIndexChanged(const QString &argument);

    void on_pushButton_execute_clicked();

    void on_pushButton_add_clicked();

    void on_pushButton_delete_clicked();

    void on_pushButton_save_clicked();

    void on_pushButton_open_clicked();

    void on_comboBox_browsedb_clicked();

    void on_comboBox_db_currentIndexChanged(const QString &arg1);

private:
    Ui::Database *dbGUI;
    QString lastDir;

public:
    QSqlDatabase mydatabase;
    QStringList listofTables;
    QSqlTableModel *tableModel;
    void fillTable(QSqlTableModel*);
    void populatecomboBoxTables(QStringList);
    void closeConnection();
    bool openConnection(QString);
    bool connectionState=false;
};

#endif // PAGE_DATABASE_H
