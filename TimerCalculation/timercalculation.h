#ifndef TIMERCALCULATION_H
#define TIMERCALCULATION_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class TimerCalculation; }
QT_END_NAMESPACE

class TimerCalculation : public QWidget
{
    Q_OBJECT


public:
    TimerCalculation(QWidget *parent = nullptr);
    ~TimerCalculation();

private slots:
    void on_pushButton_clicked();

    void on_btnARRcalc_clicked();

    void on_btn_DT_Calc_clicked();

private:
    Ui::TimerCalculation *ui;
    int prescaler1 {0};
    int timer_clock1 {0};
    unsigned long long counter_period1 {0};
    double frequency1 {0};

    int tim_cls {0};
    int prescaler2 {0};
    double delay {0};
    double cnt_cls {0};
    double timePeriod {0};
    unsigned long long ARR {0};

    double T_MCU {0};
    double T_PWM {0};
    double T_DT {0};
    unsigned DeadTimeTicks {0};
};
#endif // TIMERCALCULATION_H
