#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qlabel.h>
#include <QSpinBox>
#include <QGraphicsView>
#include "dbmanager.h"
#include "record.h"
#include "battery.h"
#include "timer.h"

#include <sstream>
#include <iostream>
#include <QElapsedTimer>
using namespace std;


const int MAX_SESSION_TYPES = 4;//MAX AMOUNT OF SESSION TYPES
const int TIMER_CUT_OFF = 10; //MAX AMOUNT OF TIMES A TIMER WILL REPEAT
const int MAX_USER_SESSION_TIME = 180; //MAX AMOUNT OF TIME OF AN USER DEGINATED SESSION
const int AUTO_OFF_CUT_OFF = 20; //MAX AMOUNT OF TIME DEVICE STAYS ON AFTER NO BUTTON IS PUSHED


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    //functions
    void checkConnection(); //checks electrical connection
    void buttonStyles();
    void loadSessionDropDown(); //loads admin drop down that displays all the available session types
    void resetSessionGroup(); //resets the currently selected session group
    void resetSessionType(); //resets the currently selected session type // helper function for UI
    void resetBatteryLevel(); //resets battery level // helper function for UI
    void displayBatteryLevel(); //displays current battery level onto device
    void displayConnection(int,int);
    void turnOffDevice();//turns off all features of the device
    void turnOnDevice();//turns on the the device
    void updateTimer();
    //admin functions
    void displayBatteryAdmin(); //displays current battery level onto admin section
    void displayIntensityAdmin(); //displays current intensity level onto admin section
    void displayIntAdmin(); // displays current intensity level onto admin section // USER DESIGNED INFO
    void displayTimeAdmin(); // displays current time onto admin section // USER DESIGNED INFO
    void displayRecordings(); // display the recordings if view button is pressed, not holding
    void loadLabelArrays();
    void updateBattery();
    void savePreference(); //saves the user desired intensity



    //variables
    Ui::MainWindow *ui;
    QLabel* labels[8]; //battery labels
    QLabel* groups[3]; //ex: 20min, 45min, USER DESIGNED
    int groupTwenty, groupFortyFive, groupUser;
    QLabel* types[4]; //ex: MET, Sub-Delta, etc
    QLabel* miscLabels[5]; //ex: L, R, short, long, INT
    QTimer* timer;
    QTimer* sessionTimer; //timer of the session
    QTimer* batteryLifeTimer; //battery depletion
    QTimer* autoOffTimer; //records the amount of time the device is left alone // >2min -> power off
    Timer* bigTimer; //display timer
    Battery* battery;
    QElapsedTimer holdTimer; //counter for how long the power button is held
    QElapsedTimer holdSelectBtnTimer; //counter for how long the select button is held
    QElapsedTimer holdViewBtnTimer; //counter for how long the select button is held

    int autoOffCounter; //counter for autoOffTimer
    bool turnedOn; //true if device is turned on // should be stored in a controller if possible
    int currGroup; //stores the currently selected session group // 0 = NONE/DEVICE IS ON, 1 = 20min, 2 = 45min, 3 = User Designed
    int currType; //stores the currently selected session type // 0 = MET, 1 = Sub-Delta, 2 = Delta, 3 = Theta
    int currBattery;//stores the current battery level (must be adapted to the battery class)
    bool active; //stores whether the device is on AND running
    bool paused; //stores whether the session is paused due to connection lost
    bool showBatteryLife; // whether to print out the battery depletion detail in the console or not
    string sessionTypes[MAX_SESSION_TYPES]; //array of all available session types
    bool currView; //indicate whether records are showing in the display or not
    int currUserInt; //stores the intensity levels when adding a user designed session
    int currUserTime; //stores the times when adding a user designed session
    int userDesignedInt[MAX_SESSION_TYPES]; //stores the intensity levels when adding a user designed session for all 4 types
    int userDesignedTime[MAX_SESSION_TYPES]; //stores the times when adding a user designed session for all 4 types
    bool labelIsLit; //true if labels are currently highlighted, false otherwise // only use to flicker the highlight
    int timerCutOff; // number that timer will stop after x repetitions
    QLabel* setToBlink; //sets a specific label to blink
    int connection; //0 = Excellent, 1 = Okay, 2 = No Connection


    QElapsedTimer sessionTime; //stores duration of session
    int intensity;
    int finalIntensity;
    float duration;
    QDateTime startTime;
    DBManager* db;

    bool sessionToSaveExists;
    bool alreadySaved;

private slots:
    //void switchGroup();
    void holdButton();
    void holdSelectButton();
    void holdViewButton();
    void selectSession();
    void blink();
    void intUp();
    void intDown();
    void batteryUp();
    void batteryDown();
    void userTimeUp();
    void userTimeDown();
    void userIntUp();
    void userIntDown();
    void recordSession();
    void print_bigtimer();
    void connectionTesting(int);
    void saveUserDesignated();
    void showBatteryLifeDetails(int);
    void clearRecordings();
    void viewRecordings();
    void autoOff();
    void rightEarConnect(int);
    void leftEarConnect(int);
    void powerBtnFunction();
    //void showTime();
};
#endif // MAINWINDOW_H

