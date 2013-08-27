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
void sim900_close(void){
	DispStr_CE(0,4,"���ڹر�gprsģ�飬���Ե�",DISP_CENTER|DISP_CLRSCR);
	SIM900_Module_Close();
}

//�Ͽ�����������Ӳ��ر�gprsģ��
void DisConnectServer(){
	DispStr_CE(0,4,"����ע�������Ե�",DISP_CENTER|DISP_CLRSCR);
	unsigned char cshut[4]= "*3#\n";

	TCP_Send_Data(cshut,strlen((char *)&cshut));

	sim900_close();
}

//�������磬����0��ʾ�ɹ�������ʧ�� 
short sim900_init(void){
	int RET=0;
	
	DispStr_CE(0,0,"ģ���ʼ��...",DISP_POSITION|DISP_CLRSCR);
	RET = SIM900_Module_Init();
	if(RET == -1)
	{
		DispStr_CE(0,4,"����SIM������",DISP_POSITION);
		DispStr_CE(0,8,"�ر�ģ��...",DISP_POSITION);
		SIM900_Module_Close();
		return -1;
	}
	else if(RET == -2)
	{
		if(SIM900_Verify_PIN((unsigned char *)"1234"))
		{
			DispStr_CE(0,4,"��֤PIN�����",DISP_POSITION);
			DispStr_CE(0,8,"�ر�ģ��...",DISP_POSITION);
			SIM900_Module_Close();	
			return -1;
		}
	}
	else if(RET == -3)
	{
		DispStr_CE(0,4,"����ģ������",DISP_POSITION);
		DispStr_CE(0,8,"�ر�ģ��...",DISP_POSITION);
		SIM900_Module_Close();
		return -1;
	}
	
	DispStr_CE(0,4,"ģ���ʼ���ɹ�",DISP_POSITION);
	DispStr_CE(0,8,"ע������...",DISP_POSITION);

	RET = SIM900_REG_GSM();
	if(RET<0)
	{
		DispStr_CE(0,12,"ע������ʧ��",DISP_POSITION);
		DispStr_CE(0,16,"�ر�ģ��...",DISP_POSITION);
		SIM900_Module_Close();
		return -1;
	}

	DispStr_CE(0,12,"ע������ɹ�",DISP_POSITION);
	delay_and_wait_key(2,EXIT_KEY_ALL,0);

	return 0;
}



short ConnectServer(){//���ӷ�����   0��ʾ���ӳɹ���-1 ��ʾ����ʧ�� 
	int RET=-1;

	DispStr_CE(0,10,"���ӷ����������Ե�...",DISP_POSITION|DISP_CLRSCR);
	unsigned char host[] = "118.123.244.109";
	unsigned char port[] = "8000";

	RET = TCP_Create_Link(host,port);

	//���ӳɹ�
	if(RET==0){
		return 0;
	}

	if(RET==-1)
		DispStr_CE(0,8,"�������",DISP_POSITION);
	else if(RET==-2)
		DispStr_CE(0,8,"���ӷ�����ʧ��",DISP_POSITION);
	else
		DispStr_CE(0,8,"���ӷ�����δ֪����",DISP_POSITION);

	delay_and_wait_key(0,EXIT_KEY_F2,0);
	WarningBeep(2);
	SIM900_Module_Close();

	return RET;
}

short SendData(unsigned char* data){    //��������    0 ��ʾ���ͳɹ� -1 ��ʾʧ�� 
	int RET = 0;
	
	DispStr_CE(0,12,"�����ϴ������Ե�...",DISP_CENTER|DISP_CLRSCR);
	RET=TCP_Send_Data(data,strlen((char *)data));
	if(RET==0){//���ͳɹ� 
		return 0;
	}

	return RET;
}

short GetRecvData(unsigned char* recvdata){//���շ��������صĽ�� 0 ��ʾ���ճɹ���-1 ��ʾ����ʧ�� 
	int len=0;
	int RET=0;

	RET = TCP_Recv_Data(recvdata,&len,FIVE_SECOND);
	if(RET==0){
		return 0;
	}

	return RET;
}
