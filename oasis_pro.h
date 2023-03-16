#ifndef OASIS_PRO_H
#define OASIS_PRO_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class oasis_pro; }
QT_END_NAMESPACE

class oasis_pro : public QMainWindow
{
    Q_OBJECT

public:
    oasis_pro(QWidget *parent = nullptr);
    void updateConsole();
    void startSession(QString curType, int curLen, int curIntensity, QString curRecording);
    void connectionTest();
    void checkBattery();
    void turnOffLights();
    ~oasis_pro();

private:
    Ui::oasis_pro *ui;

private slots:
    void PressPowerButton();
    void on_MET_released();
    void on_Sub_Delta_released();
    void on_hundred_hz_released();
    void on_Delta_released();
    void on_time_20_released();
    void on_time_45_released();
    void on_lineEdit_textChanged(const QString &arg1);
    void on_up_button_released();
    void on_down_button_released();
    void on_record_button_released();
    void on_start_button_released();
    void replaceBattery();
};
#endif // OASIS_PRO_H
