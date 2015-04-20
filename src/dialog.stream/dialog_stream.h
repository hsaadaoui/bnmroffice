#ifndef TAB_LIVE_H
#define TAB_LIVE_H

#include <QDialog>

namespace Ui {
class Stream;
}

class Stream : public QDialog
{
    Q_OBJECT

public:
    explicit Stream(QWidget *parent = 0);
    ~Stream();
    void read_graphics();
    void createData();
    void stream_loop();

private slots:
    void on_pushButton_clicked();

private slots:
    void stop();

private:
    Ui::Stream *uiStream;
    double x_min,x_max;
    int bin;
    bool loop;
};

#endif // TAB_LIVE_H
