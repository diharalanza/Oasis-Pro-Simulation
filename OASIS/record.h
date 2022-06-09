#ifndef RECORD_H
#define RECORD_H

#include <QString>
#include <QDateTime>
#include <QTime>

class Record
{
public:
    Record(const QString& group, const QString& sessionType, const int intensity, const QDateTime& startTime, const float duration);

    //setters
    void setIntensity(int i);
    void setDuration(float d);

    //getters
    QString toString();
    //---
    QString getGroup();
    QString getSessionType();
    int getIntensity();
    QDateTime getStartTime();
    float getDuration();


private: //properties
    QString group;
    QString sessionType;
    int intensity;
    QDateTime startTime;
    float duration;

};

#endif // RECORD_H
