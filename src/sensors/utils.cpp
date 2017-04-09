#include "utils.h"

namespace Utils {

    /* template bubble sort function */
    template<typename INDEX_TYPE, typename VALUE_TYPE>
    void sort(VALUE_TYPE *array, INDEX_TYPE size) {
        VALUE_TYPE temp;
        for (INDEX_TYPE i = 0; i < size - 1; i++) {
            for (INDEX_TYPE j = i + 1; j < size; j++) {
                if (array[i] > array[j]) {
                    temp = array[i];
                    array[i] = array[j];
                    array[j] = temp;
                }
            }
        }
    }

    template void sort<>(int*, byte);

    /* claculate average value */
    template<typename INDEX_TYPE, typename VALUE_TYPE>
    float average(VALUE_TYPE *array, INDEX_TYPE size) {
        float value(0);
        for (INDEX_TYPE i = 0; i < size; i++) {
            value += (float) array[i];
        }
        return value / size;
    }

    template float average<>(int*, byte);

    float analogReadAverage(byte pin, byte size, unsigned long delayTime) {
        int buf[size];
        for (int i = 0; i < size; i++) {
            buf[i] = analogRead(pin);
            delay(delayTime);
        }
        Utils::sort(buf, size);
        return Utils::average(buf + 2, 6);
    }

    template<typename T>
    T map(T x, T in_min, T in_max, T out_min, T out_max) {
        float result = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
        if (result < 0) result = 0;
        return result;
    }

    template float map<>(float, float, float, float, float);
    // if need, uncomment them. Or add yourself
    //template int SensorBase::map<int>(int, int, int, int, int);
    //template long SensorBase::map<long>(long, long, long, long, long);

}
