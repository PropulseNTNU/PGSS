#ifndef GLOBALS_H
#define GLOBALS_H

#include <QString>

namespace globals {
    const int CHART_YRANGE_POSITIVE = 500;
    const int CHART_YRANGE_NEGATIVE = -500;
    const int CHART_XRANGE = 10;
    const int CHART_WIN_MIN_WIDTH = 400;
    const int CHART_WIN_MIN_HEIGHT = 300;
    const int CHART_YRANGE_STEPFACTOR = 20;
    const int ALTITUDE_CHART_YRANGE = 1;
    const int ACCELERATION_CHAR_YRANGE_POSITIVE = 10;
    const int ACCELERATION_CHAR_YRANGE_NEGATIVE = -10;

    const int TIMER_UPDATE = 100;

    const int NUM_FLAGS = 10;
    const int NUM_SENSORS = 5;

    const int NAVBALL_BACKGROUND_COLOR = 0x505050;
    const int NAVBALL_WIDTH = 200;
    const int NAVBALL_HEIGHT = 200;

    const int SERIAL_BUFFER_SIZE = 1000; // Should be at least two times the package size
    const int SERIAL_BAUD_RATE = 115200;

    const int LIGHTWIDGET_MIN_SIZE = 20;


    //Endre på logo størrelse:
    const int STATUSBAR_LOGO_WIDTH = 380;
    const int STATUSBAR_LOGO_HEIGHT = 180;

    const QString DEFAULT_DATA_PATH = "/Users/";
    const QString DEFAULT_DATA_FILENAME = "launch_data.txt";

    enum state {
        IDLE, ARMED, BURNOUT, AIRBRAKES,
        APOGEE, DROGUE, CHUTE, LANDED, NUM_STATES
    };
}

#endif // GLOBALS_H
