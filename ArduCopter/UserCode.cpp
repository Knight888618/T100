#include "Copter.h"

#ifdef USERHOOK_INIT
void Copter::userhook_init()
{
	if(!userhook.initialised)
	{
		userhook.local_port = AP::serialmanager().find_serial(AP_SerialManager::SerialProtocol_WIT, 0);//���ұ��ض˿�

		if(userhook.local_port)
		{
			userhook.initialised = true;
		}
	}
}
#endif

#ifdef USERHOOK_FASTLOOP
void Copter::userhook_FastLoop()
{
    // put your 100Hz code here
}
#endif

#ifdef USERHOOK_50HZLOOP
void Copter::userhook_50Hz()
{
    // put your 50Hz code here
}
#endif

#ifdef USERHOOK_MEDIUMLOOP
void Copter::userhook_MediumLoop()
{
	uint16_t num_byte=0;
	uint8_t check_sum=0;
	uint8_t data_byte=0;
	uint8_t buffer_index = 0;
	uint8_t frame_flag=0;
    uint8_t data_buffer_r[100];																	//���ջ���

	if(userhook.initialised)
	{
		num_byte = userhook.local_port->available();												//��ȡ�����������ֽ���

		while(num_byte--)																			//һ�ζ�ȡһ���ֽ�
		{
			data_byte = userhook.local_port->read();												//��ȡһ���ֽ�

			if(frame_flag==0)																		//��ʼͬ����־
			{
				if((data_byte == 0x55) && (buffer_index==0))										//����֡ͷ1
				{
					data_buffer_r[buffer_index++] = data_byte;										//���浽������
					continue;																		//�˳�����ѭ��
				}

				if((data_byte == 0x53) && (buffer_index==1))										//��ȡ��һ���ֽڣ��������֡ͷ0xBB
				{
					data_buffer_r[buffer_index++] = data_byte;										//���浽������
					frame_flag = 1;
					continue;																		//�˳�����ѭ��
				}
			}
			if(frame_flag == 1)
			{
				data_buffer_r[buffer_index++] = data_byte;											//�����֡ͷ�������ֽ�

				if(buffer_index == 11)																//��ȡ��һ֡��������
				{
					for(buffer_index=0; buffer_index<10; buffer_index++)							//����У���
					{
						check_sum = check_sum + data_buffer_r[buffer_index];
					}

					if(check_sum == data_buffer_r[10])												//У���ƥ�䡢�յ����Ǵӻ�Ҫ�����ͷ��ϵͳID
					{
						short value = data_buffer_r[3] << 8 | data_buffer_r[2];
						userhook.angle_pitch = ((float)value/32768)*180.0f;
					}
				}
			}
		}
	}
}
#endif

#ifdef USERHOOK_SLOWLOOP
void Copter::userhook_SlowLoop()
{
    // put your 3.3Hz code here
}
#endif

#ifdef USERHOOK_SUPERSLOWLOOP
void Copter::userhook_SuperSlowLoop()
{
    // put your 1Hz code here
}
#endif

#ifdef USERHOOK_AUXSWITCH
void Copter::userhook_auxSwitch1(const RC_Channel::AuxSwitchPos ch_flag)
{
    // put your aux switch #1 handler here (CHx_OPT = 47)
}

void Copter::userhook_auxSwitch2(const RC_Channel::AuxSwitchPos ch_flag)
{
    // put your aux switch #2 handler here (CHx_OPT = 48)
}

void Copter::userhook_auxSwitch3(const RC_Channel::AuxSwitchPos ch_flag)
{
    // put your aux switch #3 handler here (CHx_OPT = 49)
}
#endif
