/****************************************************************************
*
*�ļ�(File):         Gprs.c
*
*�޸�(Modify):       2012-7-17 8:17:48
*
*����(Author):       
*
*����(Compile):      ����ƽ̨(Smart Platform)
*
*����(Description):
*
*
*
----------------------------------------------------------------------------
|
| Version | Datetime             |   Author    | Description
| --------+----------------------+-------------+----------------------------
|
| V1.00  2012-7-17 8:17:48            
----------------------------------------------------------------------------
****************************************************************************/
#include "exlib.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "common.h"
#include "SIM900.h"
#include "gprs.h"

//extern void WarningBeep();

//�ر�gprsģ��
void sim300_close(void){
	DispStr_CE(0,4,"���ڹر�gprsģ�飬���Ե�",DISP_CENTER|DISP_CLRSCR);
	SIM900_Module_Close();
}

//�Ͽ�����������Ӳ��ر�gprsģ��
void DisConnectServer(){
	DispStr_CE(0,4,"����ע�������Ե�",DISP_CENTER|DISP_CLRSCR);
	unsigned char cshut[4]= "*3#\n";

	TCP_Send_Data(cshut,strlen((char *)&cshut));

	sim300_close();
}

//�������磬����0��ʾ�ɹ�������ʧ�� 
short sim300_init(void){
	int ret;
	
	DispStr_CE(0,0,"ģ���ʼ��...",DISP_POSITION|DISP_CLRSCR);
	ret = SIM900_Module_Init();
	if(ret == -1)
	{
		DispStr_CE(0,4,"����SIM������",DISP_POSITION);
		DispStr_CE(0,8,"�ر�ģ��...",DISP_POSITION);
		SIM900_Module_Close();
		return -1;
	}
	else if(ret == -2)
	{
		if(SIM900_Verify_PIN((unsigned char *)"1234"))
		{
			DispStr_CE(0,4,"��֤PIN�����",DISP_POSITION);
			DispStr_CE(0,8,"�ر�ģ��...",DISP_POSITION);
			SIM900_Module_Close();	
			return -1;
		}
	}
	else if(ret == -3)
	{
		DispStr_CE(0,4,"����ģ������",DISP_POSITION);
		DispStr_CE(0,8,"�ر�ģ��...",DISP_POSITION);
		SIM900_Module_Close();
		return -1;
	}
	
	DispStr_CE(0,4,"ģ���ʼ���ɹ�",DISP_POSITION);
	DispStr_CE(0,8,"ע������...",DISP_POSITION);

	ret = SIM900_REG_GSM();
	if(ret<0)
	{
		DispStr_CE(0,12,"ע������ʧ��",DISP_POSITION);
		DispStr_CE(0,16,"�ر�ģ��...",DISP_POSITION);
		SIM900_Module_Close();
		return -1;
	}

	DispStr_CE(0,12,"ע������ɹ�",DISP_POSITION);
	delay_and_wait_key(2,EXIT_KEY_ALL,0);

	return 0;

        /*
	DispStr_CE(0,0,"����ģ���ʼ��...",DISP_CENTER|DISP_CLRSCR);
	ret = sim_module_init();
	if(ret<0){
		WarningBeep(2);

		if( g_simerr == SIM_POSTYPE_ERROR ){
			DispStr_CE(0,2,"�˻��Ͳ�֧��GPRS",DISP_POSITION);
		}else if( g_simerr == SIM_MODULE_ERROR ){
			DispStr_CE(0,2,"����ģ������",DISP_POSITION);
			DispStr_CE(0,4,"�ر�ģ��...",DISP_POSITION);
			SIM900_Module_Close();
		}else if( g_simerr == SIM_NO_CARD ){
			DispStr_CE(0,2,"����SIM������",DISP_POSITION);
			DispStr_CE(0,4,"�ر�ģ��...",DISP_POSITION);
			SIM900_Module_Close();
		}
		 return -1;
	}else{
		DispStr_CE(0,2,"ģ���ʼ���ɹ�",DISP_POSITION);
		DispStr_CE(0,4,"��������...",DISP_POSITION);

                ret = sim_signal_report();
                if(ret<10){
			DispStr_CE(0,8,"�ź�̫��",DISP_POSITION);
			return -2;
		}
               DispStr_CE(0,8,"�ź�����",DISP_POSITION);

		ret = sim_detect_net();
		if(ret<0){
			DispStr_CE(0,10,"��������ʧ��",DISP_POSITION);
			 WarningBeep(2);
			SIM900_Module_Close();
			return -3;
			
		}
	}

	DispStr_CE(0,10,"��������ɹ�",DISP_POSITION);
	return 0;
*/
}



short ConnectServer(){//���ӷ�����   0��ʾ���ӳɹ���-1 ��ʾ����ʧ�� 
	short ret ;
	DispStr_CE(0,10,"���ӷ����������Ե�...",DISP_POSITION|DISP_CLRSCR);
	unsigned char host[] = "118.123.244.109";
	unsigned char port[] = "8000";
	ret = TCP_Create_Link(host,port);

	//���ӳɹ�
	if(ret==0){
		return 0;
	}

	if(ret==-1){
		DispStr_CE(0,8,"�������",DISP_CENTER);
	}else if(ret==-2){
		DispStr_CE(0,8,"���ӷ�����ʧ��",DISP_CENTER);
	}

	delay_and_wait_key(0,EXIT_KEY_F2,0);
	WarningBeep(2);
	SIM900_Module_Close();
	
	return -1;
}

short SendData(unsigned char* data){    //��������    0 ��ʾ���ͳɹ� -1 ��ʾʧ�� 
	DispStr_CE(0,12,"�����ϴ������Ե�...",DISP_CENTER|DISP_CLRSCR);  
	if(TCP_Send_Data(data,strlen((char *)data))==0){//���ͳɹ� 
		return 0;
	}else{ 
		return -1;
	}
}

short GetRecvData(unsigned char* recvdata){//���շ��������صĽ�� 0 ��ʾ���ճɹ���-1 ��ʾ����ʧ�� 
	int len;
	int RET=0;

	RET = TCP_Recv_Data(recvdata,&len,256);
	if(RET != 0){
		DispStr_CE(0,10,"����ʧ��",DISP_CENTER|DISP_CLRSCR);
		delay_and_wait_key(10,EXIT_KEY_F2,10);
		delay_and_wait_key(1,EXIT_KEY_F2|EXIT_AUTO_QUIT,1);
		WarningBeep(2);
		return -1;
	}

	return 0;
}
