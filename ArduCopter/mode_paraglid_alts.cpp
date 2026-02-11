#include "Copter.h"

bool ModeParaglidAlts::init(bool ignore_checks)													// loiter_init - initialise loiter controller
{
    if (!pos_control->is_active_z())															// initialise the vertical position controller
    {
        pos_control->init_z_controller();
    }

    pos_control->set_max_speed_accel_z(-get_pilot_speed_dn(), g.pilot_speed_up, g.pilot_accel_z);// set vertical speed and acceleration limits
    pos_control->set_correction_speed_accel_z(-get_pilot_speed_dn(), g.pilot_speed_up, g.pilot_accel_z);

	_ctrl_alt = copter.g2.user_parameters.get2_ctrl_alt();
	_sen_angle_min = copter.g2.user_parameters.get2_sen_angle_min();
	_sen_angle_max = copter.g2.user_parameters.get2_sen_angle_max();
	_ctrl_angle1 = copter.g2.user_parameters.get2_ctrl_angle1();
	_ctrl_angle2 = copter.g2.user_parameters.get2_ctrl_angle2();
	_angle_rate = copter.g2.user_parameters.get2_ctrl_rate();
	_climb_rate_max = copter.g2.user_parameters.get2_climb_rate_max();
	_climb_rate_min = copter.g2.user_parameters.get2_climb_rate_min();
	_pitch_max = copter.g2.user_parameters.get_pitch_max();
	_climb_rate_factor = (_climb_rate_max - _climb_rate_min) / (_sen_angle_max - _sen_angle_min);
	float posD;
    if (copter.ahrs_view->get_relative_position_D_origin(posD))								//С���趨�߶Ⱦ�����ͨ����ģʽ
    {
    	if(-posD < _ctrl_alt)															//��ǰ�߶ȴ����趨�߶�
    	{
			gcs().send_text(MAV_SEVERITY_NOTICE, "ALT:%f < %d", -posD, _ctrl_alt);
			return false;
    	}
    }

	gcs().send_text(MAV_SEVERITY_NOTICE, "Ctrl Angle1=%f,Ctrl Angle2=%f\r\n", _ctrl_angle1, _ctrl_angle2);

	_channel_ctrl_state = 0x00;																	//��ʼ��״̬
	_target_pitch_angle = copter.g2.user_parameters.get2_ctrl_angle0();
	_target_climb_rate = 0.0f;

	if (!copter.failsafe.radio)																	//δʧ�ر���
	{
		for(uint8_t i=6; i<8; i++)																//7��8ͨ��pwm
		{
			RC_Channel *rc_channel = rc().channel(i);
			_pwm_value[i-6] = rc_channel->get_radio_in();
		}

		//_pwm_value[0] = copter.g2.user_parameters.get_ch7_pwm();
		//_pwm_value[1] = copter.g2.user_parameters.get_ch8_pwm();

		if((_pwm_value[0] <1500) && (_pwm_value[1] <1500))										//��ʼ����ģʽ��Ҫ7��8ͨ�����ڵ�λ����Ч
		{
			_channel_ctrl_state = 0x01;
			gcs().send_text(MAV_SEVERITY_NOTICE, "Angle CTRL Active");
		}else
		{
			gcs().send_text(MAV_SEVERITY_NOTICE, "Angle CTRL Inactive");
			return false;
		}
	}else
	{
		gcs().send_text(MAV_SEVERITY_NOTICE, "Radio FailSafe\r\n");
		return false;
	}

	gcs().send_text(MAV_SEVERITY_NOTICE, "Enter Mode 52");

    return true;
}

void ModeParaglidAlts::run()																		// loiter_run - runs the loiter controller, should be called at 100hz or more
{
    float target_roll=0.0f, target_pitch=0.0f;
    float target_yaw_rate=0.0f;

    pos_control->set_max_speed_accel_z(-get_pilot_speed_dn(), g.pilot_speed_up, g.pilot_accel_z);// set vertical speed and acceleration limits

    if (!copter.failsafe.radio)																	// process pilot inputs unless we are in radio failsafe
    {
        get_pilot_desired_lean_angles(target_roll, target_pitch, loiter_nav->get_angle_max_cd(), attitude_control->get_althold_lean_angle_max_cd());// convert pilot input to lean angles

        target_yaw_rate = get_pilot_desired_yaw_rate();											// get pilot's desired yaw rate

        float posD;
        if (copter.ahrs_view->get_relative_position_D_origin(posD))								//С���趨�߶Ⱦ�����ͨ����ģʽ
        {
        	if(-posD > _ctrl_alt)																//��ǰ�߶ȴ����趨�߶�
        	{
        		target_roll = 0.0f;
        		target_yaw_rate = 0.0f;

        		for(uint8_t i=6; i<8; i++)														//7-8ͨ��pwm
        		{
        			RC_Channel *rc_channel = rc().channel(i);
        			_pwm_value[i-6] = rc_channel->get_radio_in();
        		}

        		//_pwm_value[0] = copter.g2.user_parameters.get_ch7_pwm();
        		//_pwm_value[1] = copter.g2.user_parameters.get_ch8_pwm();

        		if((_channel_ctrl_state == 0x01) && ((_pwm_value[0]>1500) && (_pwm_value[1]<1500)))//��ʼ���ڵ�ֵ����Ч״̬��ͨ��7���������ýǶ�1�������Ǵ򵽸�λ��ͬʱͨ��8���ڵ�λ
        		{
        			_target_pitch_angle = _ctrl_angle1;
        			_channel_ctrl_state = 0x02;													//ȷ�ϽǶ�ģʽ���˳�
        		}																				//�ڶ���ִ�е��Ӽ�ģʽ

        		if((_pwm_value[1]>1500)	&& (_channel_ctrl_state == 0x02))						//ͨ��8���ڵ�λֱ�����ýǶ�2
        		{
        			_target_pitch_angle = _ctrl_angle2;
        			_target_climb_rate = _climb_rate_min;
        			_channel_ctrl_state = 0x03;
        		}

        		if(_channel_ctrl_state == 0x03)
        		{
        			if(copter.userhook.angle_pitch < _sen_angle_max)
        			{
        				_target_climb_rate = _climb_rate_min + ((_sen_angle_max - copter.userhook.angle_pitch) * _climb_rate_factor);
        			}else
        			{
        				_target_climb_rate = _climb_rate_min;
        			}
        		}

        		RC_Channel *rc_channel = rc().channel(1);										//����ͨ��
        		_pwm_value[2] = rc_channel->get_radio_in();										//����ͨ��pwmֵ
        		if(_pwm_value[2] < 1350)														//����1550��ʼ�ӽǶ�
        		{
        			_target_pitch_angle -= _angle_rate/400.f;									//���ٶȻ���
        		}

        		if(_pwm_value[2] > 1650)														//��������
        		{
        			_target_pitch_angle += _angle_rate/400.f;									//���Ƕ�
        			if(_target_pitch_angle>0)
        			{
        				_target_pitch_angle=0;
        			}
        		}
				gcs().send_text(MAV_SEVERITY_NOTICE, "_target_pitch_angle:%f\r\n",_target_pitch_angle);
        		target_pitch = _target_pitch_angle*100.f;
        	}
        }
    } else
    {
		gcs().send_text(MAV_SEVERITY_EMERGENCY, "RADIO FAILSAFE");
		copter.set_mode(Mode::Number::BRAKE, ModeReason::RADIO_FAILSAFE);
    }

    motors->set_desired_spool_state(AP_Motors::DesiredSpoolState::THROTTLE_UNLIMITED);			// set motors to full range

    attitude_control->input_euler_angle_roll_pitch_euler_rate_yaw(target_roll,constrain_float(target_pitch, -_pitch_max, _pitch_max), target_yaw_rate);// call attitude controller//constrain_float(target_pitch, -_pitch_max, _pitch_max)
	
    _target_climb_rate = constrain_float(_target_climb_rate, -get_pilot_speed_dn(), _climb_rate_max);//����������
    pos_control->set_pos_target_z_from_climb_rate_cm(_target_climb_rate);						// Send the commanded climb rate to the position controller
    pos_control->update_z_controller();															// run the vertical position controller and set output throttle
}

