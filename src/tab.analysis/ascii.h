#ifndef ASCII_H
#define ASCII_H

#include <QMainWindow>
#include <QPushButton>
#include <QTextBrowser>
#include <QComboBox>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QToolButton>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QTableWidgetItem>
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
#include <QFileDialog>
#include <QMessageBox>

class ASCII
{
public:
    ASCII(QString input,QStringList subType,int bin,double x_min,double x_max)
        :Input(input),SubType(subType),Bin(bin),Xmin(x_min),Xmax(x_max) {}

    ~ASCII(){}

    QStringList files(QStringList &,QVector< QVector <QString> > &);

    QStringList read_infofiles(QStringList &,QVector< QVector <QString> > &);

    QStringList read_listfiles(QStringList &,QVector< QVector <QString> > &);


private:
    QString  Input;
    QStringList  SubType;
    int  Bin;
    double Xmin,Xmax;
};

#endif // ASCII_H
