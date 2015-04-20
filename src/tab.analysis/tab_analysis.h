#ifndef PAGE_ANALYSIS_H
#define PAGE_ANALYSIS_H


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
#include <QThread>
#include <QTextStream>
#include <QHBoxLayout>
#include <QColor>
#include <QFileDialog>
#include "plugin_read.h"
#include "mainwindow.h"

namespace Ui {
class Analysis;
}

class Analysis : public QWidget
{
    Q_OBJECT

public:
    explicit Analysis(QWidget *parent = 0);

    ~Analysis();

    void comboBox_type();

    void comboBox_fct();

    void comboBox_data();

    void comboBox_fitAs();

    void comboBox_fitSettings();

    void comboBox_report();

    void read_graphics();

    bool inputTable_reading();

    void inputTable_set(void);

    void parameters_initialization();

    void outputTable_printResults();

    void report_write();

    void report_header(QString);

    void bnmr_optionsVisibility(bool);

    void xmgr_plots(bool);

    bool fileList();

    void fit_write(void);

    void settings(void);

    bool data_exists(void);

    void loadLibrary(QString);

    void clearVectors(void);

    static QString tableLetter;

    static int tablePrecision;

    static int tableError;

    static int tableName;

    static bool tableGlobal;

    static bool tableSingle;

    static bool tableTemplate;

private slots:
    void on_comboBox_fcn_currentIndexChanged(const QString &);

    void on_toolButton_runs_clicked();

    void on_toolButton_report_clicked();

    void on_pushButton_plot_clicked();

    void on_pushButton_start_clicked();

    void on_pushButton_abort_clicked();

    void on_pushButton_results_clicked();

    void on_comboBox_dataType_currentIndexChanged(const QString &);

    void on_comboBox_fitAs_currentIndexChanged(const QString &);

    void on_comboBox_report_currentIndexChanged(int);

    //void on_tableWidget_input_itemClicked(QTableWidgetItem *item);

    void iniTableWidget_itemClicked(QTableWidgetItem *item);

    void on_comboBox_fitSettings_currentIndexChanged(const QString &);

    void on_tabWidget_tabCloseRequested(int);

    void on_tabWidget_tabBarDoubleClicked(int);

    void iniTabWidget_removeTab(int);

    void ProvideContexMenu(const QPoint&);
    void load_intable(void);
    void save_intable(void);

private:
    Ui::Analysis *anaGUI;

    QStandardItemModel *model;

    QTabWidget *iniTabWidget;

    void tabWidget_initials(void);

    QString lastRunsDir;

    QString lastReportDir;

    function *fct;

    Parameters *fctPars;

    QString fctDesc;

    QStringList fctList;

    bool fctState     = false;

    bool reportAppend = false;

    bool fitAsGlobal  = false;

    QStringList runList,headers,subType;

    int nbFiles;

    int nbParsOfFct;

    double   bin,x_min, x_max, y_min, y_max;

    QString input, minimizer, error, report;

    QVector<QString> parLabel;

    QVector< QVector <QString> > headerValue;

    QVector<int>    parShare,parFix;

    QVector<double> parValue,parMin,parMax,parErrNeg,parErrPos,chisq;

    QVector<int> dataFormat;

    QVector< QVector< QVector<double> > > xData,yData;
};

class tableDialog : public QDialog
{
    Q_OBJECT

public:
    tableDialog(QWidget *parent = 0);

    int getPrecision(void);

    QString getLetter(void);

    bool getGlobal(void);

    bool getSingle(void);

    bool getTemplate(void);

    int getNameType(void);

    int getErrorType(void);

private slots:

    void setNewValues(void);
    void keepValues(void);


private:
    void createLayout();

    QDialogButtonBox *buttonBox;

    QLabel    *nameLabel;

    QComboBox *nameComboBox;

    QLabel    *preLabel;

    QSpinBox  *preSpinBox;

    QLabel    *letLabel;

    QComboBox *letComboBox;

    QLabel    *errLabel;

    QComboBox *errComboBox;

    QCheckBox *globalBox;

    QCheckBox *singleBox;

    int pre;

    int nameType;

    int errorType;

    QString letter;

    bool global;

    bool single;
};
#endif // PAGE_ANALYSIS_H
