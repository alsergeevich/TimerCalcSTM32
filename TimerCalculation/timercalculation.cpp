#include "timercalculation.h"
#include "ui_timercalculation.h"
#include <QMessageBox>
#include <QRegularExpression>



TimerCalculation::TimerCalculation(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TimerCalculation)
{
    ui->setupUi(this);
    QRegularExpression rx("^([1-9][0-9]*|0)");
    QValidator *l_validator = new QRegularExpressionValidator(rx, this);
    ui->lineEdit_MCU_F->        setValidator (l_validator);
    ui->lineEdit_PWM_F->        setValidator (l_validator);
    ui->lineEdit_clock->        setValidator (l_validator);
    ui->lineEdit_prescaler->    setValidator (l_validator);
    ui->lineEdit_period->       setValidator (l_validator);
    ui->lineEdit_clock_arr->    setValidator (l_validator);
    ui->lineEdit_prescaler_arr->setValidator (l_validator);
}

TimerCalculation::~TimerCalculation()
{
    delete ui;
}


void TimerCalculation::on_pushButton_clicked()
{
    QString typeTimer = ui->cmbTypeTimerSelect->currentText();
    QString result;
    QString prescalerMCU;
    bool ok;
    try {

        timer_clock1 = ui->lineEdit_clock->text().toInt(&ok);
        if(!ok) {
            throw "Ошибка ввода";
        }
        if (timer_clock1 <= 0) {
            throw "Значение частоты таймера не должно быть меньше или равно 0";
        }
        counter_period1 = ui->lineEdit_period->text().toLong(&ok);
        if(!ok) {
            throw "Ошибка ввода";
        }
        prescaler1 = ui->lineEdit_prescaler->text().toInt(&ok);
        if(!ok) {
            throw "Ошибка ввода";
        }
        if(prescaler1 == 0) prescaler1 += 1;
        if (typeTimer == "16" && (counter_period1 > 65535)) {
            counter_period1 = 65535;
            ui->lineEdit_period->setText("65535");
            QMessageBox::information(this, "Внимание", "Значение регистра ARR в 16-ти разрядном таймере не должно превышать значение 65535");
        }
        if (typeTimer == "32" && (counter_period1 > 4294967295)) {
            counter_period1 = 4294967295;
            ui->lineEdit_period->setText("4294967295");
            QMessageBox::information(this, "Внимание", "Значение регистра ARR в 32-х разрядном таймере не должно превышать значение 4294967295");
        }

//        if(counter_period1 < 0 || prescaler1 < 0) {
//            throw "Отрицательные значения недопустимы";
//        }

        frequency1 = (double)timer_clock1 / (double)(prescaler1 * counter_period1);
        result.setNum(frequency1);
        ui->lineEdit_frequency->setText(result);
        prescalerMCU.setNum(prescaler1-1);
        ui->lineEdit_mcu->setText(prescalerMCU);


    } catch (const char* message) {
        QMessageBox::information(this, "Внимание", message);
        return;
    }
}


void TimerCalculation::on_btnARRcalc_clicked()
{
    QString typeTimer = ui->cmbTypeTimerSelect->currentText();
    bool ok;

    try {
        tim_cls = ui->lineEdit_clock_arr->text().toInt(&ok);
        if(!ok) {
            throw "Ошибка ввода";
        }
        if (tim_cls <= 0) {
            throw "Значение частоты таймера не должно быть меньше или равно 0";
        }

        prescaler2 = ui->lineEdit_prescaler_arr->text().toInt(&ok);
        if(!ok) {
            throw "Ошибка ввода";
        }

        delay = ui->lineEdit_delay->text().toDouble(&ok);
        if(!ok) {
            throw "Ошибка ввода";
        }

        if((prescaler2 < 0) || (delay < 0)) {
            throw "Отрицательные значения недопустимы";
        }

        cnt_cls = tim_cls / (1 + prescaler2);
        timePeriod = 1 / cnt_cls;
        ARR = delay / timePeriod;
        if(typeTimer == "16" && ARR > 65535) {
            while (ARR > 65535) {
                ++prescaler2;
                cnt_cls = tim_cls / (1 + prescaler2);
                timePeriod = 1 / cnt_cls;
                ARR = delay / timePeriod;
            }
        }

        if(typeTimer == "32" && ARR > 4294967295) {
            while (ARR > 4294967295) {
                ++prescaler2;
                cnt_cls = tim_cls / (1 + prescaler2);
                timePeriod = 1 / cnt_cls;
                ARR = delay / timePeriod;
            }
        }

        ui->lineEdit_prescaler_arr->setText(QString::number(prescaler2));
        ui->lineEditResult_ARR->setText(QString::number(ARR));

    } catch (const char* message) {
        QMessageBox::information(this, "Внимание", message);
        return;
    }


}


void TimerCalculation::on_btn_DT_Calc_clicked()
{
   if(ui->lineEdit_MCU_F->text ().isEmpty () || ui->lineEdit_PWM_F->text ().isEmpty ()){
      QMessageBox::information(this, "Внимание", "Поля не должны быть пустыми");
      return;
   }
   bool ok;
   QString result_Tmcu;
   QString result_Tpwm;
   QString result_deadTimeTicks;
   double procente;
   double f_mcu = ui->lineEdit_MCU_F->text ().toDouble (&ok);
   double f_pwm = ui->lineEdit_PWM_F->text ().toDouble (&ok);

   T_MCU = (1 / f_mcu)*1000000000;              //определяем время 1 такта мк
   result_Tmcu.setNum (T_MCU);
   ui->lineEdit_T_MCU->setText (result_Tmcu);

   T_PWM = (1 / f_pwm)*1000000;                 //определяем период PWM
   result_Tpwm.setNum (T_PWM);
   ui->lineEdit_T_PWM->setText (result_Tpwm);


   if(ui->radioButton_1p->isChecked ()){        //какой процент от периода ШИМ составляет DT
      procente = 0.01;
   }
   if(ui->radioButton_2p->isChecked ()){
      procente = 0.02;
   }
   if(ui->radioButton_3p->isChecked ()){
      procente = 0.03;
   }
   if(ui->radioButton_4p->isChecked ()){
      procente = 0.04;
   }
   if(ui->radioButton_5p->isChecked ()){
      procente = 0.05;
   }

   T_DT = procente * T_PWM;                  //определяем Dead Time в мс

   DeadTimeTicks = (unsigned)((T_DT / 1000000) / (T_MCU / 1000000000)); //пересчитываем в такты для задания в настройках мк
   if(DeadTimeTicks > 255){
      QMessageBox::information(this, "Внимание", "Невозможно рассчитать Dead Time увеличте частоту PWM");
      return;
   }
   result_deadTimeTicks.setNum (DeadTimeTicks);
   ui->lineEdit_DT->setText (result_deadTimeTicks);
}

