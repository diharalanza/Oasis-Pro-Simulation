#include "record.h"

Record::Record(const QString& group, const QString& sessionType, const int intensity, const QDateTime& startTime, const float duration) {


        this->group = group;
        this->sessionType = sessionType;
        this->intensity = intensity;
        this->startTime = startTime;
        this->duration = duration;

}

void Record::setIntensity(int i) { intensity = i; }

void Record::setDuration(float d) { duration = d; }

QString Record::toString() {



    QString newString =
            startTime.toString("yyyy-MM-dd hh:mm:ss") + "\n"
            + "   Group: " + group + "\n"
            + "   Session Type: " + sessionType + "\n"
            + "   Intensity: " + QString::number(intensity) + "\n"
            + "   Duration: " + QString::number(duration/1000);

    return newString;


    return newString;
}

QString Record::getGroup() { return group; }

QString Record::getSessionType() { return sessionType; }

int Record::getIntensity() { return intensity; }

QDateTime Record::getStartTime() { return startTime; }

float Record::getDuration() { return duration; }
