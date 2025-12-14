#include "UserParameters.h"
#include "config.h"

#if USER_PARAMS_ENABLED
// "USR" + 13 chars remaining for param name
const AP_Param::GroupInfo UserParameters::var_info[] =
{
    AP_GROUPINFO("_RATCLB_P", 0, UserParameters, _ratclb_p, 100),
    AP_GROUPINFO("_CTRL_ALT", 1, UserParameters, _ctrl_alt, 20),
    AP_GROUPINFO("_SEN_ANGL", 2, UserParameters, _sen_angle, -70),
	AP_GROUPINFO("_CTRL_ANGL1", 3, UserParameters, _ctrl_angle1, -10),
	AP_GROUPINFO("_CTRL_ANGL2", 4, UserParameters, _ctrl_angle2, -20),
	AP_GROUPINFO("_ANGL_RATE", 5, UserParameters, _angle_rate, 1),

//	AP_GROUPINFO("CH7_PWM", 6, UserParameters, _ch7_pwm, 1100),
//	AP_GROUPINFO("CH8_PWM", 7, UserParameters, _ch8_pwm, 1100),

    AP_GROUPEND
};

UserParameters::UserParameters()
{
    AP_Param::setup_object_defaults(this, var_info);
}
#endif // USER_PARAMS_ENABLED
