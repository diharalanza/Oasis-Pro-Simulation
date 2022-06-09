#include "dbmanager.h"


const QString DBManager::DATABASE_PATH = "/database1/oasis.db";

DBManager::DBManager()
{
    oasisDB = QSqlDatabase::addDatabase("QSQLITE");
    oasisDB.setDatabaseName("oasis.db");

    if (!oasisDB.open()) {
        throw "Error: Database could not be opened";
    }

    if (!DBInit()) {
        throw "Error: Database could not be initialized";
    }
}

bool DBManager::DBInit() {

    oasisDB.transaction();

    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS all_recordings (recording_id INTEGER NOT NULL UNIQUE PRIMARY KEY AUTOINCREMENT, group_type text, session_type text, intensity int, start_time text, duration int);");
    query.exec("CREATE TABLE IF NOT EXISTS preferences (pref_id INTEGER NOT NULL UNIQUE PRIMARY KEY, session_type text, intensity int);");

    // initialize device
    query.exec("INSERT INTO preferences VALUES (0, 'MET', 0);");
    query.exec("INSERT INTO preferences VALUES (1, 'Sub-Delta', 0);");
    query.exec("INSERT INTO preferences VALUES (2, 'Delta', 0);");
    query.exec("INSERT INTO preferences VALUES (3, 'Theta', 0);");

    return oasisDB.commit();
}

bool DBManager::addRecord(Record* r) {


    oasisDB.transaction();

    QSqlQuery query;

    query.prepare("INSERT INTO all_recordings (group_type, session_type, intensity, start_time, duration) VALUES (:group_type, :session_type, :intensity, :start_time, :duration);");
    query.bindValue(":group_type",r->getGroup());
    query.bindValue(":session_type", r->getSessionType());
    query.bindValue(":intensity", r->getIntensity());
    query.bindValue(":start_time", r->getStartTime());
    query.bindValue(":duration", r->getDuration());
    query.exec();


    return oasisDB.commit();

}

bool DBManager::updateRecord(Record* r, int rec_id){

    oasisDB.transaction();
    QSqlQuery query;
    query.prepare("UPDATE all_recordings SET intensity=:intensity, duration=:duration  WHERE (recording_id=:rec_id);");
    query.bindValue(":intensity", r->getIntensity());
    query.bindValue(":duration", r->getDuration());
    query.bindValue(":rec_id", rec_id);
    query.exec();

    return oasisDB.commit();
}

int DBManager::getLastId(){
    QSqlQuery query;
    oasisDB.transaction();
    query.exec("select last_insert_rowid()");

    while (query.next()) {
        return query.value(0).toInt();
    }

}

QVector<Record*> DBManager::getRecordings() {

    QSqlQuery query;
    QVector<Record*> qvr;
    oasisDB.transaction();

    query.prepare("SELECT * FROM all_recordings");
    query.exec();

    while (query.next()) {

        QString group = query.value(1).toString();
        QString sessionType = query.value(2).toString();
        int intensity = query.value(3).toString().toInt();


        QDateTime startTime = query.value(4).toDateTime();

        int duration = query.value(5).toString().toInt();

        Record* r = new Record(group, sessionType, intensity, startTime, duration);

        cout << r->toString().toStdString() << endl;

        qvr.append(r);

    }
    return qvr;
}
bool DBManager::updatePref(Record* r, int pref_id){

    oasisDB.transaction();
    QSqlQuery query;
    query.prepare("UPDATE preferences SET intensity=:intensity WHERE (pref_id=:pref_id);");
    query.bindValue(":intensity", r->getIntensity());
    query.bindValue(":pref_id", pref_id);
    query.exec();

    return oasisDB.commit();
}

int DBManager::getPref(const QString& sessionType){
    cout << sessionType.toStdString() << endl;
    oasisDB.transaction();
    QSqlQuery query;
    query.prepare("SELECT intensity FROM preferences where session_type = :session_type;");
    query.bindValue(":session_type", sessionType);
    query.exec();

    while (query.next()) {
        return query.value(0).toInt();
    }
    return -1;
}

QVector<Record*> DBManager::getAllPrefs() {
    QSqlQuery query;
    QVector<Record*> qvr;
    oasisDB.transaction();

    query.prepare("SELECT * FROM preferences");
    query.exec();

    while (query.next()) {

        int pref_id = query.value(0).toInt();
        QString sessionType = query.value(1).toString();
        int intensity = query.value(2).toString().toInt();

        Record* r = new Record("0", sessionType, intensity, QDateTime::currentDateTime(), 0);

        cout << "id: " << pref_id << " sess: " << r->getSessionType().toStdString() << " int: " << r->getIntensity() << endl;

        qvr.append(r);

    }
    return qvr;
}

bool DBManager::deleteAll() {

    QSqlQuery query;
    query.prepare("DELETE FROM all_recordings");

    return query.exec();

}
