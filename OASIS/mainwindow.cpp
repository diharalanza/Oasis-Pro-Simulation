#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "qcommonstyle.h"
#include "qstyle.h"
#include <QTimer>
#include <QTime>

#include <QElapsedTimer>

//#include "qpalette.h"
#include <iostream>

using namespace std;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    db = new DBManager();

    buttonStyles();
    connect(ui->powerBtn, SIGNAL(pressed()), this, SLOT(holdButton()));
    connect(ui->powerBtn, SIGNAL(released()), this, SLOT(powerBtnFunction()));
    connect(ui->selectBtn, SIGNAL(pressed()), this, SLOT(holdSelectButton()));
    connect(ui->selectBtn, SIGNAL(released()), this, SLOT(selectSession()));
    connect(ui->intUpBtn, SIGNAL(pressed()), this, SLOT(intUp()));
    connect(ui->intDownBtn, SIGNAL(pressed()), this, SLOT(intDown()));
    connect(ui->batteryUp, SIGNAL(pressed()), this, SLOT(batteryUp()));
    connect(ui->batteryDown, SIGNAL(pressed()), this, SLOT(batteryDown()));
    connect(ui->userTimeUp, SIGNAL(pressed()), this, SLOT(userTimeUp()));
    connect(ui->userTimeDown, SIGNAL(pressed()), this, SLOT(userTimeDown()));
    connect(ui->userIntUp, SIGNAL(pressed()), this, SLOT(userIntUp()));
    connect(ui->userIntDown, SIGNAL(pressed()), this, SLOT(userIntDown()));
    connect(ui->saveEditorButton, SIGNAL(pressed()), this, SLOT(saveUserDesignated()));
    connect(ui->viewBtn, SIGNAL(released()), this, SLOT(viewRecordings()));
    connect(ui->viewBtn, SIGNAL(pressed()), this, SLOT(holdViewButton()));
    connect(ui->clearBtn, SIGNAL(pressed()), this, SLOT(clearRecordings()));
    connect(ui->saveBtn, SIGNAL(pressed()), this, SLOT(recordSession()));
    connect(ui->attachEarLeft, SIGNAL(stateChanged(int)), this, SLOT(leftEarConnect(int)));
    connect(ui->attachEarRight, SIGNAL(stateChanged(int)), this, SLOT(rightEarConnect(int)));


    timer = new QTimer(this);
    QTimer::connect(timer, SIGNAL(timeout()), this, SLOT(blink()));

    sessionTimer = new QTimer(this);
    QTimer::connect(sessionTimer, &QTimer::timeout, this, &MainWindow::updateTimer);

    batteryLifeTimer = new QTimer(this);
    QTimer::connect(batteryLifeTimer, &QTimer::timeout, this, &MainWindow::updateBattery);


    autoOffTimer = new QTimer(this);
    QTimer::connect(autoOffTimer, &QTimer::timeout, this, &MainWindow::autoOff);


    battery = new Battery();

    bigTimer = new Timer();

    ui->batteryWarning->setVisible(false);

    // Connection testing QComboBox connections
    connect(ui->connectionDD, QOverload<int>::of(&QComboBox::currentIndexChanged), this, QOverload<int>::of(&MainWindow::connectionTesting));

    // Battery life detail QComboBox connections
    connect(ui->batteryLife, QOverload<int>::of(&QComboBox::currentIndexChanged), this, QOverload<int>::of(&MainWindow::showBatteryLifeDetails));

    // Initialization
    turnedOn = false;
    active = false;
    paused = false;
    showBatteryLife = false;
    currView = false;
    currGroup = -1; //nothing selected
    currType = -1; //nothing selected
    currBattery = 8;
    intensity = 3;
    currUserInt = 0; //before getting any user input from DAVID editor
    currUserTime = 0; //before getting any suer input from DAVID editor


    //not temporary
    labelIsLit = false;
    timerCutOff = 0;
    connection = 2; //no connection


    //time length for 3 groups of sessions
    groupTwenty = 20;
    groupFortyFive = 45;
    groupUser = 60; //default time for user designated group, will be overwritten once user input saved

    //initialize default time and intensity for 4 types of user designated group
    for(int i=0; i< MAX_SESSION_TYPES; i++){
        userDesignedTime[i] = groupUser;
        userDesignedInt[i] = intensity;
    }

    //loading Session Types Array
    sessionTypes[0] = "MET";
    sessionTypes[1] = "Sub-Delta";
    sessionTypes[2] = "Delta";
    sessionTypes[3] = "Theta";

    loadLabelArrays();//loads all the arrays that hold label QLabel pointers

    loadSessionDropDown();//loads session types drop down in admin

    //admin functions
    displayBatteryAdmin();//displays the current battery in the admin text box
    displayIntensityAdmin();//displays the current intensity in the admin text box
    displayIntAdmin();//displays current selected intensity in admin
    displayTimeAdmin();//displays current selected time in admin

    //displayBatteryLevel();//displays the current battery level on the device

    ui->sessionStatus->setVisible(false);
    ui->sessionLbl->setVisible(false);
    ui->lcdTimer->setVisible(false);

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::loadSessionDropDown(){
    for(int i = 0; i < MAX_SESSION_TYPES; i++){
        QString result = QString::fromStdString(sessionTypes[i]);
        ui->sessionTypeDD->addItem(result);
    }
}

void MainWindow::loadLabelArrays(){
    //load array of labels
    labels[0] = ui->bat1;
    labels[1] = ui->bat2;
    labels[2] = ui->bat3;
    labels[3] = ui->bat4;
    labels[4] = ui->bat5;
    labels[5] = ui->bat6;
    labels[6] = ui->bat7;
    labels[7] = ui->bat8;

    //load array of session group labels
    groups[0] = ui->TwentyLbl;
    groups[1] = ui->FortyFiveLbl;
    groups[2] = ui->UserLbl;

    //load array of session type labels
    types[0] = ui->metLbl;
    types[1] = ui->subDeltaLbl;
    types[2] = ui->deltaLbl;
    types[3] = ui->thetaLbl;

    //load array of misc labels
    miscLabels[0] = ui->intLbl;
    miscLabels[1] = ui->leftEar;
    miscLabels[2] = ui->rightEar;
    miscLabels[3] = ui->shortCES;
    miscLabels[4] = ui->dutyCES;
}

//SLOTS////////////////////////////////

void MainWindow::holdButton(){
    holdTimer.start();
}

void MainWindow::holdSelectButton(){
    autoOffCounter = 0;
    holdSelectBtnTimer.start();
}

void MainWindow::holdViewButton(){
    holdViewBtnTimer.start();
}

void MainWindow::leftEarConnect(int checked){
    if(!turnedOn){
        return;
    }
    if(checked < 2){
        ui->connectionDD->setCurrentIndex(2); //select No Connection
        ui->leftEar->setStyleSheet("background-color: red; color: black");
    }else{
        ui->leftEar->setStyleSheet("background-color: black; color: white");
    }
}

void MainWindow::rightEarConnect(int checked){
    if(!turnedOn){
        return;
    }
    if(checked < 2){
        ui->connectionDD->setCurrentIndex(2); //select No Connection
        ui->rightEar->setStyleSheet("background-color: red; color: black");
    }else{
        ui->rightEar->setStyleSheet("background-color: black; color: white");
    }
}

void MainWindow::autoOff(){
    if(active){
        return;
    }
    autoOffCounter++;
    if(autoOffCounter > AUTO_OFF_CUT_OFF && turnedOn && !active){//device has not been touched for more than 2 minutes, is turned on, and not active
        turnOffDevice();
    }
}

void MainWindow::powerBtnFunction(){//(power button) switches between selected groups
    autoOffCounter = 0;
    if(holdTimer.elapsed() > 2000 && turnedOn == true){//the button was held for 2 seconds, and the device was already on
        qDebug() << holdTimer.elapsed();
        turnOffDevice();
    }else{//not held for 2 seconds
        if (currBattery == 0){//battery drained, can't turn on the device.
            return;
        }
        else if(turnedOn == false){//turn on device
            turnOnDevice();
            batteryLifeTimer->start(1000);
        }else{//already on, switching/selecting group
            if(active){return;}
            currGroup=(currGroup+1)%3;
            ui->lcdTimer->setVisible(true);
            resetSessionGroup();
            groups[currGroup]->setStyleSheet("background-color: yellow; color: black");
            if(currGroup == 0){
                bigTimer->set_time(groupTwenty);
                print_bigtimer();
                ui->intensityLevelTxt->setText(QString::number(intensity));
            }else if(currGroup == 1){
                bigTimer->set_time(groupFortyFive);
                print_bigtimer();
                ui->intensityLevelTxt->setText(QString::number(intensity));
            }else{//60 min selected user designated
                bigTimer->set_time(userDesignedTime[0]);//display the time for first group MET
                print_bigtimer();
            }
            displayIntensityAdmin();
        }
    }
}

void MainWindow::intUp(){//when the up intensity button is pressed
    //check first if user wants to change the intensity or session number
    if(!turnedOn || currGroup == -1){
        return;
    }
    autoOffCounter = 0;
    if(active && turnedOn){//increase intensity

        if(currGroup !=2 && intensity < 8){
                intensity++;
        }
        else if (currGroup ==2 && userDesignedInt[currType] < 8){
                userDesignedInt[currType]++;
        }

        displayIntensityAdmin();

    }else{//change session type
        //currType++;
        ui->sessionStatus->setVisible(true);
        ui->sessionLbl->setVisible(true);
        currType=(currType+1)%4;

        if (currGroup == 2){
            displayIntensityAdmin();
            bigTimer->set_time(userDesignedTime[currType]);
            print_bigtimer();
        }
        resetSessionType();

        for(int i = 0; i < 4; i++){
            if(i == currType){
                //cout<<currType<<endl;
                types[currType]->setStyleSheet("background-color: yellow; color: black");
            }
        }

        intensity = db->getPref(QString::fromUtf8(sessionTypes[currType].c_str()));

        if (currGroup != 2){
            ui->intensityLevelTxt->setText(QString::number(intensity));
        }
        else{
            ui->intensityLevelTxt->setText(QString::number(userDesignedInt[currType]));
        }
    }
}

void MainWindow::intDown(){//when the down intensity button is pressed
    //check first if user wants to change the intensity or session number
    if(!turnedOn || currGroup == -1){
        return;
    }
    autoOffCounter = 0;;
    if(active && turnedOn){//decrease intensity

        if(currGroup !=2 && intensity > 0){
                intensity--;
                //cout<<"intensity after decreasing: "<<intensity<<endl;
        }
        else if (currGroup ==2 && userDesignedInt[currType] > 0){
                userDesignedInt[currType]--;
                //cout<<"intensity after decreasing: "<<userDesignedInt[currType]<<endl;
        }
        displayIntensityAdmin();

    }else{//inactive && turnedOn: change session type
        ui->sessionStatus->setVisible(true);
        ui->sessionLbl->setVisible(true);

        if(currType == -1){
            currType = 3;
        }
        else{
           currType=(currType+3)%4;
        }
        if (currGroup == 2){
            displayIntensityAdmin();
            bigTimer->set_time(userDesignedTime[currType]);
            print_bigtimer();
        }
        resetSessionType();

        for(int i = 0; i < 4; i++){
            if(i == currType){
                //cout<<currType<<endl;
                types[currType]->setStyleSheet("background-color: yellow; color: black");
            }
        }

        intensity = db->getPref(QString::fromUtf8(sessionTypes[currType].c_str()));
        if (currGroup != 2){
            ui->intensityLevelTxt->setText(QString::number(intensity));
        }
        else{
            ui->intensityLevelTxt->setText(QString::number(userDesignedInt[currType]));
        }
    }
}

void MainWindow::batteryUp(){
    if(currBattery < 8){
        currBattery++;
        stringstream number;
        number << currBattery;
        string stringResult = number.str();
        ui->batteryLevelTxt->setText(QString::fromStdString(stringResult));
        resetBatteryLevel();
        displayBatteryLevel();
        battery->setBatteryLevel(currBattery);
    }
}

void MainWindow::batteryDown(){
    if(currBattery > 0){
        currBattery--;
        stringstream number;
        number << currBattery;
        string stringResult = number.str();
        ui->batteryLevelTxt->setText(QString::fromStdString(stringResult));
        resetBatteryLevel();
        displayBatteryLevel();
        battery->setBatteryLevel(currBattery);
    }
}

void MainWindow::userIntUp(){
    if(currUserInt < 8){
        currUserInt++;
        stringstream number;
        number << currUserInt;
        string stringResult = number.str();
        ui->userInt->setText(QString::fromStdString(stringResult));
        displayIntAdmin();
    }
}

void MainWindow::userIntDown(){
    if(currUserInt > 0){
        currUserInt--;
        stringstream number;
        number << currUserInt;
        string stringResult = number.str();
        ui->userInt->setText(QString::fromStdString(stringResult));
        displayIntAdmin();
    }
}

void MainWindow::userTimeUp(){
    if(currUserTime < 180){//time in minutes
        currUserTime += 1; //INCREASES TIME IN 5 MINUTE INTERVALS
        stringstream number;
        number << currUserTime;
        string stringResult = number.str();
        ui->userTime->setText(QString::fromStdString(stringResult));
        displayTimeAdmin();
    }
}

void MainWindow::userTimeDown(){
    if(currUserTime > 0){
        currUserTime -= 1;
        stringstream number;
        number << currUserTime;
        string stringResult = number.str();
        ui->userTime->setText(QString::fromStdString(stringResult));
        displayTimeAdmin();
    }
}

void MainWindow::selectSession(){

    if(holdSelectBtnTimer.elapsed() > 1000 && active == true){//the button was held for 1 seconds, and the device is active
        qDebug() << holdTimer.elapsed();
        savePreference();
        return;
    }
    if(active){return;}//if a session is already active
    if(currGroup == -1 || currType == -1){//No group or type is selected
        return;
    }
    autoOffCounter = 0;;

    int tempType = currType;
    setToBlink = types[--tempType];
    timer->start(500); //blinks chosen session type

    checkConnection();
    if (connection != 2){
        sessionTime.restart();

        sessionToSaveExists = true;
        alreadySaved = false;
        startTime = QDateTime::currentDateTime();

        if (!(currBattery < 4 && currGroup == 2 && userDesignedTime[currType] > 60)){
            cout<<"Session has started"<<endl;

            active = true;
            if(bigTimer->get_start()){
                ui->sessionStatus->setText("On");
                sessionTimer->start();
            }else{
                bigTimer->set_start(true);
                ui->sessionStatus->setText("On");
                sessionTimer->start(1000);
            }
        }
    }
}

void MainWindow::connectionTesting(int selection) {
    if (selection == 2){
        if (active) {
            paused = true;
            ui->sessionStatus->setText("Paused");
            sessionTimer->stop();
        }
    }else {
        if (active) {
            paused = false;
            ui->sessionStatus->setText("On");
            sessionTimer->start(1000);
        }
    }
}

//save user input for the user designated group
void MainWindow::saveUserDesignated(){
    //get the session type which user selected
    int sessionType = ui->sessionTypeDD->currentIndex();
    //update the intensity for the selected type from previous saved user intensity input
    userDesignedInt[sessionType] = currUserInt;
    //update the time for the selected type from previous saved user time input
    userDesignedTime[sessionType] = currUserTime;

    //when saving user input for the current browsed type in the user designated group
    //update the corresponding information: session time, intensity in the admin panel
    //for type not currently browsed, intUP() will update these information
    if(currGroup == 2 && currType == sessionType){
        //sync the intensity displayed in the admin panel with the user input one
        if(!active){
            displayIntensityAdmin();
            bigTimer->set_time(userDesignedTime[currType]);
            print_bigtimer();
        }
    }
}

void MainWindow::blink(){
    if(labelIsLit == true){ //the labels are highlighted -> turn off
        setToBlink->setStyleSheet("background-color: black; color: white");
        labelIsLit = false;
    }else{//highlight labels
        setToBlink->setStyleSheet("background-color: red; color: black");
        labelIsLit = true;
    }
    timerCutOff++;
    if(timerCutOff == TIMER_CUT_OFF){
        timerCutOff = 0;
        timer->stop();
    }
}


void MainWindow::recordSession(){

    if(!turnedOn || !sessionToSaveExists){ //does not record session if device is off or on but no session is running
        return;
    }
    autoOffCounter = 0;;

    QString group = groups[currGroup]->text();

    QString sessionType = QString::fromUtf8(sessionTypes[currType].c_str());

    if(active){ // session has not ended, take difference for duration

        duration = (sessionTime.elapsed());
        if (currGroup != 2){
            finalIntensity = intensity;
        }
        else{
            finalIntensity = userDesignedInt[currType];
        }
    }

    //new recording
    if(!alreadySaved){

        Record* newRecord = new Record(group, sessionType, finalIntensity, startTime, duration);

        db->addRecord(newRecord);
        alreadySaved = true;
    }

    else{ //update recording
        Record* newRecord = new Record(group, sessionType, finalIntensity, startTime, duration);
        db->updateRecord(newRecord, db->getLastId());
    }

    //if currently the recording is showing, then added the new record just saved to the display
    if (ui->recordingTxt->toPlainText()!=""){
        displayRecordings();
    }
}

void MainWindow::viewRecordings(){
    if(!turnedOn){
       return;
    }
    autoOffCounter = 0;;
    //turn off the recording display if the view button is holding for more than 1 second and if the recording
    //is showing now
    if(holdViewBtnTimer.elapsed() > 1000 && currView == true){
        qDebug() << holdViewBtnTimer.elapsed();
        currView = false;
        ui->recordingTxt->setText(QString::fromStdString(""));
    }
    //if just click the view button show the recordings
    else{
      displayRecordings();
    }
}

void MainWindow::clearRecordings(){
    autoOffCounter = 0;
    //clear button will only work if the recordings are displayed and the no session is running.
    if (currView && !active){
        db->deleteAll();
        ui->recordingTxt->setText(QString::fromStdString(""));
        currView = false;
    }
}

void MainWindow::showBatteryLifeDetails(int selection){
    if (selection == 1){
        showBatteryLife = true;
    }
    else{
        showBatteryLife = false;
    }
}

//print remain time on screen
void MainWindow::print_bigtimer(){

    if(bigTimer->get_time_sec() < 10){
        if(bigTimer->get_time_min() < 10){
            QString time_number = QString::fromStdString("0" + to_string(bigTimer->get_time_min()) + ":0" + to_string(bigTimer->get_time_sec()));
            ui->lcdTimer->display(time_number);
        }else{
            QString time_number = QString::fromStdString(to_string(bigTimer->get_time_min()) + ":0" + to_string(bigTimer->get_time_sec()));
            ui->lcdTimer->display(time_number);
        }
    }else{
        if(bigTimer->get_time_min() < 10){
            QString time_number = QString::fromStdString("0" + to_string(bigTimer->get_time_min()) + ":" + to_string(bigTimer->get_time_sec()));
            ui->lcdTimer->display(time_number);
        }else{
            QString time_number = QString::fromStdString(to_string(bigTimer->get_time_min()) + ":" + to_string(bigTimer->get_time_sec()));
            ui->lcdTimer->display(time_number);
        }
    }
}

//END OF SLOTS FUNCTIONS////////////////////////

void MainWindow::buttonStyles(){
    QCommonStyle commonStyle;
    //QStyle style;
    //intesity buttons
    ui->intUpBtn->setIcon(commonStyle.standardIcon(QStyle::SP_ArrowUp));
    ui->intDownBtn->setIcon(commonStyle.standardIcon(QStyle::SP_ArrowDown));

    //admin buttons
    ui->userTimeUp->setIcon(commonStyle.standardIcon(QStyle::SP_ArrowUp));
    ui->userTimeDown->setIcon(commonStyle.standardIcon(QStyle::SP_ArrowDown));
    ui->userIntUp->setIcon(commonStyle.standardIcon(QStyle::SP_ArrowUp));
    ui->userIntDown->setIcon(commonStyle.standardIcon(QStyle::SP_ArrowDown));
    ui->batteryUp->setIcon(commonStyle.standardIcon(QStyle::SP_ArrowUp));
    ui->batteryDown->setIcon(commonStyle.standardIcon(QStyle::SP_ArrowDown));
}



void MainWindow::turnOffDevice(){
    turnedOn = false;
    active = false;
    sessionToSaveExists = false;
    alreadySaved = false;
    bigTimer->set_time(0);
    bigTimer->set_start(false);
    updateTimer();
    batteryLifeTimer->stop();
    autoOffTimer->stop();
    autoOffCounter = 0;
    //ui->sessionStatus->setText("On");
    for(int i = 0; i < 4; i++){
        types[i]->setStyleSheet("background-color: black; color: rgb(136, 138, 133)");
    }
    for(int i = 0; i < 3; i++){
        groups[i]->setStyleSheet("background-color: black; color: rgb(136, 138, 133)");
    }
    for(int i = 0; i < 8; i++){
        labels[i]->setStyleSheet("background-color: black; color: rgb(136, 138, 133)");
    }
    for(int i = 0; i < 5; i++){
        miscLabels[i]->setStyleSheet("background-color: black; color: rgb(136, 138, 133)");
    }
    ui->recordingTxt->setText(QString::fromStdString(""));

    ui->sessionStatus->setVisible(false);
    ui->sessionLbl->setVisible(false);
    ui->lcdTimer->setVisible(false);
    ui->batteryWarning->setVisible(false);
    cout<<"Device Has Been Turned Off"<<endl;
}

void MainWindow::turnOnDevice(){
    turnedOn = true;
    sessionToSaveExists = false;
    alreadySaved = false;
    resetBatteryLevel();
    displayBatteryLevel();
    resetSessionType();
    resetSessionGroup();
    currGroup = -1; //nothing selected // device is turned on
    currType = -1; //nothing selected
    autoOffCounter = 0;
    autoOffTimer->start(1000);
    for(int i = 0; i < 5; i++){
        miscLabels[i]->setStyleSheet("background-color: black; color: white");
    }
    cout<<"Device Turned ON"<<endl;
}



void MainWindow::checkConnection(){
    //cout<<"Testing connection"<<endl;
    //MUST CHECK WHICH TYPE OF CONNECTION IS NEEDED
    if(currType == 1){ //50% DUTY CES only for Sub-Delta
        setToBlink = ui->dutyCES;
    }else{
        setToBlink = ui->shortCES;
    }
    timer->start(500); //blinks specific CES signal
    if(!ui->attachEarLeft->isChecked() || !ui->attachEarRight->isChecked()){
        ui->connectionDD->setCurrentIndex(2);
    }
    int selection = ui->connectionDD->currentIndex();
    resetBatteryLevel();
    if(selection == 0){//excellent connection
        connection = 0;
        displayConnection(0,3);
    }else if(selection == 1){//okay connection
        connection = 1;
        displayConnection(3,6);
    }else{//no connection
        connection = 2;
        displayConnection(6,8);
        setToBlink = ui->bat8;
        timer->start(500);
    }
}

//updating and printing the timer
void MainWindow::updateTimer(){
    bigTimer->time_flie();
    if(bigTimer->get_time_min() + bigTimer->get_time_sec() == 0){

        if (currGroup == 0){
            if (sessionTime.elapsed()<= groupTwenty*1000){
                duration = (sessionTime.elapsed());
            }
            else{
                duration = groupTwenty*1000;
            }
        }
        else if (currGroup == 1){
            if (sessionTime.elapsed()<= groupFortyFive*1000){
                duration = (sessionTime.elapsed());
            }
            else{
                duration = groupFortyFive*1000;
            }
        }
        else{
            if (sessionTime.elapsed()<= userDesignedTime[currType]*1000){
                duration = (sessionTime.elapsed());
            }
            else{
                duration = userDesignedTime[currType]*1000;
            }
        }
        //duration = sessionTime.elapsed();
        if (currGroup != 2){
            finalIntensity = intensity;
        }
        else{
            finalIntensity = userDesignedInt[currType];
        };

        sessionTimer->stop();
        ui->sessionStatus->setText("Off");
        if (currGroup == 2){
            bigTimer->set_time(userDesignedTime[currType]);
        }
        else if (currGroup == 1){
            bigTimer->set_time(groupFortyFive);
        }
        else{
            bigTimer->set_time(groupTwenty);
        }
        active = false;
    }
    print_bigtimer();
}

//update the battery level
void MainWindow::updateBattery(){
    if(battery->getBatteryLevel() < 2){
            sessionTimer->stop();
            // TODO:  issues a warning
            //qDebug()<<"warning";
            if(active){
                active = false;
                ui->sessionStatus->setText("Off");
                ui->batteryWarning->setText("Battery is low, session stopped.");
            }
            else{
                ui->batteryWarning->setText("Battery is low, please charge.");
            }
            ui->batteryWarning->setVisible(true);
            ui->batteryWarning->setStyleSheet("color: orange;");
            // When the battery is less than 2%
            if(battery->getBatteryLevel() < 0.24){//2 minutes of battery life
                // TODO: shutdown
                //qDebug()<<"shutdown";
                ui->batteryWarning->setText("Shutting down in 2 min. Save record now.");
                ui->batteryWarning->setVisible(true);
                ui->batteryWarning->setStyleSheet("color: red;");
                batteryLifeTimer->stop();
                turnOffDevice();
            }
    }
    else if (currBattery < 4 && currGroup == 2 && userDesignedTime[currType] > 60 && !active){
        ui->batteryWarning->setText("Battery is too low for session selected.");
        ui->batteryWarning->setVisible(true);
        ui->batteryWarning->setStyleSheet("color: red;");
    }
    else{
        ui->batteryWarning->setVisible(false);
    }

    if (currGroup != 2){
        battery->consume(active, paused, intensity);
    }
    else{
        battery->consume(active, paused, userDesignedInt[currType]);
    }
    if (showBatteryLife){
        cout<<"battery level: "<<battery->getBatteryLevel()<<endl;
    }
    //set the current battery level from the battery level updated continuously
    currBattery = battery->getBatteryLevel();

    resetBatteryLevel();
    displayBatteryLevel();

    //outout the current battery level to the admin panel
    stringstream number;
    number << currBattery;
    string stringResult = number.str();
    ui->batteryLevelTxt->setText(QString::fromStdString(stringResult));
}

void MainWindow::resetSessionGroup(){
    for(int i = 0; i < 3; i++){
        groups[i]->setStyleSheet("background-color: black; color: white");
    }
}

void MainWindow::resetSessionType(){
    for(int i = 0; i < 4; i++){
        types[i]->setStyleSheet("background-color: black; color: white");
    }
}
void MainWindow::resetBatteryLevel(){
    if(turnedOn){
        for(int i = 0; i < 8; i++){
           labels[i]->setStyleSheet("background-color: black; color: white");
        }
    }
}

void MainWindow::savePreference(){
    QString sessionType = QString::fromUtf8(sessionTypes[currType].c_str());
    Record* r = new Record("-", sessionType, intensity, QDateTime::currentDateTime(), -1);
    db->updatePref(r, currType);
}

void MainWindow::displayRecordings(){
    QVector<Record*> r = db->getRecordings();
    ui->recordingTxt->setText(QString::fromStdString("Recording Display On"));
    for(int i = 0; i < r.size(); i++){
        stringstream number;
        int temp = i + 1;
        number << "--------"<<"Recording #"<< temp << "--------";
        string stringResult = number.str();
        ui->recordingTxt->append(QString::fromStdString(stringResult));
        ui->recordingTxt->append(r[i]->toString());
    }
    currView = true;
}

void MainWindow::displayBatteryAdmin(){
    stringstream number;
    number << currBattery;
    string stringResult = number.str();
    ui->batteryLevelTxt->setText(QString::fromStdString(stringResult));
}

void MainWindow::displayBatteryLevel(){
    if (turnedOn){
        for(int i = 0; i <= currBattery; i++){
            labels[i]->setStyleSheet("background-color: green; color: black");
        }
    }
}
void MainWindow::displayConnection(int start, int end){
    if(connection == 0){//excellent
        //cout<<start<<endl;
        for(int i = start; i < end; i++){
            labels[i]->setStyleSheet("background-color: green; color: black");
        }
    }else if(connection == 1){//okay
        for(int i = start; i < end; i++){
            labels[i]->setStyleSheet("background-color: yellow; color: black");
        }
    }else{//no
        for(int i = start; i < end; i++){
            labels[i]->setStyleSheet("background-color: red; color: black");
        }
    }
}

void MainWindow::displayIntensityAdmin(){
    stringstream number;

    if (currGroup != 2){
        number << intensity;
    }
    else{
        number << userDesignedInt[currType];
    }
    string stringResult = number.str();
    ui->intensityLevelTxt->setText(QString::fromStdString(stringResult));
}

void MainWindow::displayIntAdmin(){
    stringstream number;
    number << currUserInt;
    string stringResult = number.str();
    ui->userInt->setText(QString::fromStdString(stringResult));
}

void MainWindow::displayTimeAdmin(){
    stringstream number;
    number << currUserTime;
    string stringResult = number.str();
    ui->userTime->setText(QString::fromStdString(stringResult));
}
