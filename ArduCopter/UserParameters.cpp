#include "UserParameters.h"
#include "config.h"

#if USER_PARAMS_ENABLED
// "USR" + 13 chars remaining for param name
const AP_Param::GroupInfo UserParameters::var_info[] =
{
    AP_GROUPINFO("_CTRL_ALT", 0, UserParameters, _ctrl_alt, 20),
    AP_GROUPINFO("_SEN_ANGL_MAX", 1, UserParameters, _sen_angle_max, -70),
	AP_GROUPINFO("_SEN_ANGL_MIN", 2, UserParameters, _sen_angle_min, -90),
	AP_GROUPINFO("_PITCH_ANGL1", 3, UserParameters, _ctrl_angle1, -10),
	AP_GROUPINFO("_PITCH_ANGL2", 4, UserParameters, _ctrl_angle2, -20),
	AP_GROUPINFO("_PITCH_RATE", 5, UserParameters, _ctrl_angle_rate, 1),
	AP_GROUPINFO("_CLIMB_MAX", 6, UserParameters, _climb_rate_max, 500),
	AP_GROUPINFO("_CLIMB_MIN", 7, UserParameters, _climb_rate_min, 200),
	AP_GROUPINFO("_PITCH_MAX", 8, UserParameters, _pitch_max, 3000),
	AP_GROUPINFO("_PITCH_ANGL0", 9, UserParameters, _ctrl_angle0, -5),
	AP_GROUPINFO("_CLIMB_RATE", 10, UserParameters, _ctrl_climb_rate,2),
    AP_GROUPINFO("_KEEP_CLRATE", 10, UserParameters, _keep_ctrl_climb_rate,2),

	AP_GROUPINFO("2_CTRL_ALT", 11, UserParameters, T_ctrl_alt, 20),
    AP_GROUPINFO("2_SEN_ANGLMAX", 12, UserParameters, T_sen_ang_max, -70),
	AP_GROUPINFO("2_SEN_ANGLMIN", 13, UserParameters, T_sen_ang_min, -90),
	AP_GROUPINFO("2_PITCH_ANGL1", 14, UserParameters, T_ctrl_ang1, -10),
	AP_GROUPINFO("2_PITCH_ANGL2", 15, UserParameters, T_ctrl_ang2, -20),
	AP_GROUPINFO("2_PITCH_RATE", 16, UserParameters, T_ctrl_ang_rate, 1),
	AP_GROUPINFO("2_CLIMB_MAX", 17, UserParameters, T_climb_rate_max, 500),
	AP_GROUPINFO("2_CLIMB_MIN", 18, UserParameters, T_climb_rate_min, 200),
	AP_GROUPINFO("2_PITCH_ANGL0", 19, UserParameters, T_ctrl_angle0, -5),

	//AP_GROUPINFO("CH7_PWM", 10, UserParameters, _ch7_pwm, 1100),
	//AP_GROUPINFO("CH8_PWM", 11, UserParameters, _ch8_pwm, 1100),

    AP_GROUPEND
};

UserParameters::UserParameters()
{
    AP_Param::setup_object_defaults(this, var_info);
}
#endif // USER_PARAMS_ENABLED
