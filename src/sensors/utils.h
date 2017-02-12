#ifndef UTILS_H
#define UTILS_H

#include <arduino.h>

namespace Utils {
    
    /* template bubble sort function */
    template<typename INDEX_TYPE, typename VALUE_TYPE>
    void sort(VALUE_TYPE *array, INDEX_TYPE size);
    
    /* claculate average value */
    template<typename INDEX_TYPE, typename VALUE_TYPE>
    float average(VALUE_TYPE *array, INDEX_TYPE size);
    
    // mapping the value
    template<typename T>
    T map(T x, T in_min, T in_max, T out_min, T out_max);
    
    // Get sample value from the sensor for smooth the value
    float analogReadAverage(byte pin, byte size, unsigned long delay);
    
}

#endif