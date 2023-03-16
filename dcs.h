#ifndef DCS_H
#define DCS_H


class DCS
{
private:
    int batteryLife;
    bool connected;
    bool power;
    int currentIntensity;

    int curSession; //1: MET, 2: Sub-Delta, 3: Delta, 4: 100 Hz
    int curLen;
    int recording; //0: dont record, 1: record

public:
    DCS();
    bool getConnected(){return connected;};
    bool getPower(){return power;};
    int getBatteryLife(){return batteryLife;};
    int getCurrentIntensity(){return currentIntensity;};
    int getCurSession(){return curSession;};
    int getCurLen(){return curLen;};
    int getRecording(){return recording;};

    void setBatteryLife(int bl){batteryLife = bl;};
    void setConnected(bool c){connected = c;};
    void setPower(bool p){power = p;};
    void setCurrentIntensity(int ci){currentIntensity = ci;};
    void setSession(int s){curSession = s;};
    void setLen(int len){curLen = len;};
    void setRec(int r){recording = r;};

    int therapyRecord[64][7];
};

#endif // DCS_H
