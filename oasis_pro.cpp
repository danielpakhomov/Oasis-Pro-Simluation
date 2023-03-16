#include <QDebug>
#include <unistd.h>

#include "oasis_pro.h"
#include "ui_oasis_pro.h"
#include "dcs.h"
#include <QTimer>
DCS mainDevice = DCS();
bool session_ongoing = false;
//action count used for deciding when to deduct battery life on top of calculations from sessions
int action_count=0;
//this keeps track of how many recordings we have
int rec_session_count = 0;
oasis_pro::oasis_pro(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::oasis_pro)
{
    ui->setupUi(this);

    connect(ui->power_button, SIGNAL (released()), this, SLOT (PressPowerButton()));
    //This is for the replace battery button
    connect(ui->rep_battery, SIGNAL(released()), this, SLOT(replaceBattery()));

}

oasis_pro::~oasis_pro()
{
    delete ui;
}

void oasis_pro::updateConsole() {
    //QString consoleText = "";
    QString consoleText;

    consoleText += "Power: ON\n";

    if (mainDevice.getConnected()) {
        consoleText += "CONNECTED\n";
    } else {
        consoleText += "NO CONNECTION\n";
    }
    consoleText += "Battery Level: " + QString::number(mainDevice.getBatteryLife()) + "%\n";
    consoleText += "Current Inensity: " + QString::number(mainDevice.getCurrentIntensity()) + "\n";

    ui->console_output->setText(consoleText);
}

void oasis_pro::PressPowerButton(){

    if (session_ongoing == true){

        ui->power_button->setEnabled(false);

    }


    else if (mainDevice.getPower()) {
        ui->start_button->setEnabled(false);
        ui->up_button->setEnabled(false);
        ui->down_button->setEnabled(false);
        ui->record_button->setEnabled(false);
        ui->MET->setEnabled(false);
        ui->Sub_Delta->setEnabled(false);
        ui->Delta->setEnabled(false);
        ui->hundred_hz->setEnabled(false);

        ui->time_20->setEnabled(false);
        ui->time_45->setEnabled(false);
        ui->lineEdit->setEnabled(false);
        ui->console_output->setText("Power: OFF");
        mainDevice.setPower(false);
        turnOffLights();
    }
    else {
        if (mainDevice.getBatteryLife()>0){
            ui->start_button->setEnabled(true);
            ui->up_button->setEnabled(true);
            ui->down_button->setEnabled(true);
            ui->record_button->setEnabled(true);
            ui->MET->setEnabled(true);
            ui->Sub_Delta->setEnabled(true);
            ui->Delta->setEnabled(true);
            ui->hundred_hz->setEnabled(true);

            ui->time_20->setEnabled(true);
            ui->time_45->setEnabled(true);
            ui->lineEdit->setEnabled(true);
            updateConsole();
            mainDevice.setPower(true);
         }
         else{
            ui->console_output->setText("Please replace battery...");

         }

    }

}

void oasis_pro::on_MET_released()
{
    ui->curr_session_selection->setText("MET");
}

void oasis_pro::on_Sub_Delta_released()
{
    ui->curr_session_selection->setText("Sub-Delta");
}

void oasis_pro::on_Delta_released()
{
    ui->curr_session_selection->setText("Delta");
}

void oasis_pro::on_hundred_hz_released()
{
    ui->curr_session_selection->setText("100 Hz");
}

void oasis_pro::on_time_20_released()
{
    ui->lineEdit->setText(QString::number(20));
    ui->curr_session_len->setText(QString::number(20));
}

void oasis_pro::on_time_45_released()
{
    ui->lineEdit->setText(QString::number(45));
    ui->curr_session_len->setText(QString::number(45));
}

void oasis_pro::on_lineEdit_textChanged(const QString &arg1)
{
    ui->curr_session_len->setText(arg1);
}

void oasis_pro::on_up_button_released()
{
    action_count++;
    //this is a check to see if we should decrement battery life yet
    if (action_count>=5){
        mainDevice.setBatteryLife(mainDevice.getBatteryLife()-1);
        action_count=0;
    }
    int curIntensity = ui->curr_session_intensity->text().toInt();
    if (curIntensity < 8){
        curIntensity += 1;
        mainDevice.setCurrentIntensity(curIntensity);
        if (session_ongoing){
            ui->console_output->append("Setting intensity to "+ QString::number(curIntensity));
        }
        else{
          updateConsole();
        }
        ui->curr_session_intensity->setText(QString::number(curIntensity));

    }
}

void oasis_pro::on_down_button_released()
{
    action_count++;
    if (action_count>=5){
        mainDevice.setBatteryLife(mainDevice.getBatteryLife()-1);
        action_count=0;
    }
    int curIntensity = ui->curr_session_intensity->text().toInt();
    if (curIntensity > 1){
        curIntensity -= 1;
        mainDevice.setCurrentIntensity(curIntensity);
        if (session_ongoing){
            ui->console_output->append("Setting intensity to "+ QString::number(curIntensity));
        }
        else{
          updateConsole();
        }
        ui->curr_session_intensity->setText(QString::number(curIntensity));

    }
}

void oasis_pro::on_record_button_released()
{

    action_count++;
    if (action_count>=5){
        mainDevice.setBatteryLife(mainDevice.getBatteryLife()-1);
        action_count=0;
    }
    if (ui->curr_session_recording->text() == "OFF"){
        if (rec_session_count<63){
            mainDevice.setRec(1);
            ui->curr_session_recording->setText("ON");
        }
        else{
                ui->console_output->append("Maximum allowable number of recordings reached...");
        }
    }
    else {
        mainDevice.setRec(0);
        ui->curr_session_recording->setText("OFF");
    }
}

void oasis_pro::startSession(QString curType, int curLen, int curIntensity, QString curRecording){

    connectionTest();
    checkBattery();
    ui->console_output->append("Session Intensity: " + ui->curr_session_intensity->text());
    ui->console_output->append("Session Type: " + ui->curr_session_selection->text());
    ui->console_output->append("Session Length: " + ui->curr_session_len->text()+ " Minutes");
    ui->console_output->append("Starting Session...");
    session_ongoing = true;
    for (int i=0; i<ui->curr_session_len->text().toInt(); i++){
        QEventLoop loop;
        QTimer::singleShot(1000, &loop, &QEventLoop::quit);
        loop.exec();
        if (i%3==0){
            mainDevice.setBatteryLife(mainDevice.getBatteryLife()-1);
            if(mainDevice.getBatteryLife()<=0){
                ui->console_output->append("Oasis powering down...");
                turnOffLights();
                break;
            }
        }
        if (!ui->power_button->isEnabled()){
            QString string = "";
            for (int i=0;i<3;i++){


                ui->console_output->setText("Soft Shutdown."+string);
                turnOffLights();
                if(i==0){
                    QString highlight = "background-color: red";
                    ui->label_2->setStyleSheet(highlight);
                    ui->label_6->setStyleSheet(highlight);

                }
                if(i==1){
                    QString highlight = "background-color: yellow";
                    ui->label_7->setStyleSheet(highlight);
                    ui->label_8->setStyleSheet(highlight);
                    ui->label_9->setStyleSheet(highlight);
                }
                if(i==2){
                    QString highlight = "background-color: green";
                    ui->label_10->setStyleSheet(highlight);
                    ui->label_11->setStyleSheet(highlight);
                    ui->label_12->setStyleSheet(highlight);
                }
                string=string+".";
                QEventLoop timer;
                QTimer::singleShot(1000, &timer, &QEventLoop::quit);
                timer.exec();
            }
            break;
        }
        //QWidget::repaint();
        ui->console_output->append("Session going...");
        if (ui->MET->isEnabled()){
            ui->console_output->append("Ending Session...");
            turnOffLights();
            break;
        }

    }

    session_ongoing = false;

    if ((mainDevice.getBatteryLife()<=0) || !ui->power_button->isEnabled()){

        turnOffLights();

        ui->power_button->setEnabled(true);
        ui->start_button->setEnabled(false);
        ui->up_button->setEnabled(false);
        ui->down_button->setEnabled(false);
        ui->MET->setEnabled(false);
        ui->Sub_Delta->setEnabled(false);
        ui->Delta->setEnabled(false);
        ui->hundred_hz->setEnabled(false);

        ui->time_20->setEnabled(false);
        ui->time_45->setEnabled(false);
        ui->lineEdit->setEnabled(false);
        ui->console_output->setText("Power: OFF");
        ui->start_button->setText("Start Session");
        mainDevice.setPower(false);

    }
    else{
        ui->console_output->append("Ending Session...");

        turnOffLights();

        ui->record_button->setEnabled(true);
        ui->power_button->setEnabled(true);

        ui->MET->setEnabled(true);
        ui->Sub_Delta->setEnabled(true);
        ui->Delta->setEnabled(true);
        ui->hundred_hz->setEnabled(true);

        ui->time_20->setEnabled(true);
        ui->time_45->setEnabled(true);
        ui->lineEdit->setEnabled(true);
        updateConsole();
        if(mainDevice.getRecording()){
            mainDevice.therapyRecord[rec_session_count][0] = mainDevice.getCurrentIntensity();
            mainDevice.therapyRecord[rec_session_count][1] = mainDevice.getCurLen();
            mainDevice.therapyRecord[rec_session_count][2] = mainDevice.getCurSession();
            ui->console_output->append(QString::number(mainDevice.therapyRecord[rec_session_count][0]) +","+QString::number(mainDevice.therapyRecord[rec_session_count][1])+","+ QString::number(mainDevice.therapyRecord[rec_session_count][2]));
            rec_session_count++;
            mainDevice.setRec(0);
            ui->curr_session_recording->setText("OFF");
        }
        ui->start_button->setEnabled(true);
        ui->start_button->setText("Start Session");

    }

}

void oasis_pro::on_start_button_released()
{

    ui->record_button->setEnabled(!ui->record_button->isEnabled());
    ui->start_button->setEnabled(false);

    ui->MET->setEnabled(!ui->MET->isEnabled());
    ui->Sub_Delta->setEnabled(!ui->Sub_Delta->isEnabled());
    ui->Delta->setEnabled(!ui->Delta->isEnabled());
    ui->hundred_hz->setEnabled(!ui->hundred_hz->isEnabled());

    ui->time_20->setEnabled(!ui->time_20->isEnabled());
    ui->time_45->setEnabled(!ui->time_45->isEnabled());
    ui->lineEdit->setEnabled(!ui->lineEdit->isEnabled());

    QString curType = ui->curr_session_selection->text();
    int curLength = ui->curr_session_len->text().toInt();
    int curIntensity = ui->curr_session_intensity->text().toInt();
    QString curRecording = ui->curr_session_recording->text();

    if (ui->start_button->text() == "Start Session") {
        ui->start_button->setText("Stop Session");
        startSession(curType, curLength, curIntensity, curRecording);
    }
    else {
        /*ui->start_button->setText("Start Session");
        ui->record_button->setEnabled(!ui->record_button->isEnabled());
        ui->power_button->setEnabled(!ui->power_button->isEnabled());

        ui->MET->setEnabled(!ui->MET->isEnabled());
        ui->Sub_Delta->setEnabled(!ui->Sub_Delta->isEnabled());
        ui->Delta->setEnabled(!ui->Delta->isEnabled());
        ui->hundred_hz->setEnabled(!ui->hundred_hz->isEnabled());

        ui->time_20->setEnabled(!ui->time_20->isEnabled());
        ui->time_45->setEnabled(!ui->time_45->isEnabled());
        ui->lineEdit->setEnabled(!ui->lineEdit->isEnabled());*/
    }


}


//This function simulates a connection test
void oasis_pro::connectionTest(){
    QString consoleText = "";
    QString highlight = "background-color: green";
    consoleText += "Connection Test... \n";
    ui->console_output->setText(consoleText);
    ui->label_10->setStyleSheet(highlight);
    ui->label_11->setStyleSheet(highlight);
    ui->label_12->setStyleSheet(highlight);
    consoleText += "Connection Test: Connection Excellent \n";
    ui->console_output->setText(consoleText);
    mainDevice.setConnected(true);

    consoleText += "Power: ON\n";
    if (mainDevice.getConnected()) {
        consoleText += "CONNECTED\n";
    } else {
        consoleText += "NO CONNECTION\n";
    }
    ui->console_output->setText(consoleText);
}

//This function simulates replacing the battery
void oasis_pro::replaceBattery(){
    mainDevice.setBatteryLife(100);
    updateConsole();

}


//This function checks the battery levels and alerts user of low/critical battery levels
void oasis_pro::checkBattery(){
    QString consoleText = "";
   if(mainDevice.getBatteryLife() <= 50){
       consoleText += "Low Battery Warning! %\n";
       ui->console_output->append(consoleText);
   }
   if(mainDevice.getBatteryLife() <= 30){
        consoleText += "Critical Battery level! Replace battery. %\n";
        ui->console_output->append(consoleText);
   }
}

//This function turns off the lights on the amd graph
void oasis_pro::turnOffLights(){
    QString highlight = "background-color: white";
    ui->label_2->setStyleSheet(highlight);
    ui->label_6->setStyleSheet(highlight);
    ui->label_7->setStyleSheet(highlight);
    ui->label_8->setStyleSheet(highlight);
    ui->label_9->setStyleSheet(highlight);
    ui->label_10->setStyleSheet(highlight);
    ui->label_11->setStyleSheet(highlight);
    ui->label_12->setStyleSheet(highlight);
}
