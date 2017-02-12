#include "sensor_base.h"

void SensorBase::setDebugStream(Stream *stream) {
    this->debugStream = stream;
}

template<typename T>
void SensorBase::log(T s) {
    if (this->debugStream != NULL) {
        this->debugStream->println(s);
    }
}

template void SensorBase::log<>(StringSumHelper);
template void SensorBase::log<>(String);
template void SensorBase::log<>(const char*);