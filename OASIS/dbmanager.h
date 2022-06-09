#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QString>
#include <QSqlDatabase>
#include <QDateTime>
#include <QDebug>
#include <QSqlQuery>
#include <QList>
#include <QApplication>

#include "record.h"

#include <sstream>
#include <iostream>
using namespace std;

class DBManager
{
public:
    const QString DATE_FORMAT = "yyyy-MM-dd hh:mm";
    static const QString DATABASE_PATH;

    DBManager();
    bool addRecord(Record* r);
    bool updateRecord(Record* r, int rec_id);
    QVector<Record*> getRecordings();
    int getLastId();

    bool updatePref(Record* r, int pref_id);
    int getPref(const QString& sessionType);
    QVector<Record*> getAllPrefs();

    bool deleteAll();

private:
    QSqlDatabase oasisDB;
    bool DBInit();

};

#endif // DBMANAGER_H
