#ifndef SENSORS_H
#define SENSORS_H

#include <stdint.h>

namespace Sensors {
    void init();
    bool wallFront();
    bool wallLeft();
    bool wallRight();

    // Raw readings (for PID steering correction later)
    uint16_t readFront();
    uint16_t readLeft();
    uint16_t readRight();
}

#endif
