#ifndef INCLUDES_H
#define INCLUDES_H

#define ICO_HOME 0xf015
#define ICO_BACK 0xf053

#define AUTO_MODEL   1
#define MANUAL_MODEL 2

#define SYSTEMINFO_INI "/SystemInfo"
#define SPEEDINFO_INI  "/SpeedInfo"
#define TIMEINFO_INI   "/TimeInfo"
#define STRETCH_INI    "/StretchInfo"
#define BEND_INI       "/BendInfo"

enum MSG_TYPE
{
    INFO_MSG,
    WARING_MSG,
    ERROR_MSG
};

struct emokit_signal {
    int F3, FC6, P7, T8, F7, F8, T7, P8, AF4, F4, AF3, O2, O1, FC5;
};
struct motorControlCmd {
    char head;
    char cmd;
    char control;
    char check;
};



#endif // INCLUDES_H
