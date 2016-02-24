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

#ifndef PAGE_SEARCH_H
#define PAGE_SEARCH_H

#include <QWidget>
#include <QDialog>
#include <QStandardItemModel>

namespace Ui {
class Search;
}

class Search : public QWidget
{
    Q_OBJECT

public:
    explicit Search(QWidget *parent = 0);
    ~Search();
    QString       msr_archive_path(QString,int);

    void          readLogs();

    QStringList   listDefaultHeaders(void);

    QStringList   search_indvar(int, int);

    bool          findLabel(QString,QString);

signals:

    void          change(int);

private slots:
    void          on_pushButton_search_clicked();

    void          on_pushButton_morelogs_clicked();

    void          on_pushButton_checkall_clicked();

    void          on_pushButton_plot_clicked();

    void          checkall();

    QString       checked_runs();

    void          results();

    void          Edit_logs();

    void          experimentCombo();

    void          modeCombo();

    void          valueChanged(int);

    void          read_query(QVector<QString>&);

    QVector< QVector<QString> > engine_bnmr();

private:
    Ui::Search *schGUI;
    QStandardItemModel *expOptions;
    QStandardItemModel *modeOptions;
};

class updatelogs : public QDialog
{
  Q_OBJECT

public:
  updatelogs(QWidget *parent = 0);

  private slots:
  void   change_logSelection(int);
  void   update_logSelection();
  void   edit_logfile();
private:
  void createLayout();
};
#endif // PAGE_SEARCH_H

