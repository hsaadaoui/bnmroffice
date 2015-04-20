#ifndef PAGE_CONVERTER_H
#define PAGE_CONVERTER_H

#include <QWidget>

namespace Ui { class Converter; }

class Converter : public QWidget
{
    Q_OBJECT

public:
    explicit Converter(QWidget *parent = 0);
    ~Converter();

private slots:

    void on_lineEdit_field_Li_8_returnPressed();

    void on_lineEdit_frequency_Li_8_returnPressed();

    void on_lineEdit_field_helmoltz_returnPressed();

    void on_lineEdit_current_helmoltz_returnPressed();

    void on_lineEdit_duration_insech_returnPressed();

    void on_lineEdit_bandwidth_insech_returnPressed();

    void on_lineEdit_duration_hermite_returnPressed();

    void on_lineEdit_bandwidth_hermite_returnPressed();

    void on_lineEdit_field_muon_returnPressed();

    void on_lineEdit_frequency_muon_returnPressed();

private:
    Ui::Converter *conGUI;
};

#endif // PAGE_CONVERTER_H
