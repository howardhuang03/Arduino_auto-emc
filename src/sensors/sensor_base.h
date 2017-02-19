#ifndef SENSOR_BASE_H
#define SENSOR_BASE_H

#include "arduino.h"

class SensorBase {
public:

    void setDebugStream(Stream *stream);
    template<typename T> void log(T s);

protected:

    Stream *debugStream;

};

#endif