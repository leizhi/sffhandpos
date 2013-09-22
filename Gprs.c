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
	int Ret = 0;
	DispStr_CE(0,4,"����ע�������Ե�",DISP_CENTER|DISP_CLRSCR);
	unsigned char cshut[4]= "*3#\n";


	Ret = TCP_Check_Link();
	if(Ret == 0)
	{
		TCP_Send_Data(cshut,strlen((char *)&cshut));

		//�Ͽ������������
		TCP_Shut_Link();
	}
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


//���ӷ�����   0��ʾ���ӳɹ���-1 ��ʾ����ʧ�� 
short ConnectServer(){
	int i = 0;
	int RET=-1;

	DispStr_CE(0,10,"���ӷ����������Ե�...",DISP_POSITION|DISP_CLRSCR);
	
	//lzy ���Է�����	
	//unsigned char host[] = "113.89.1.149";
	//unsigned char port[] = "5300";

	//lzy ʵ�ʵķ�����
	unsigned char host[] = "118.123.244.109";
	unsigned char port[] = "8000";	

	//���ӳɹ�
	for(i = 0; i < 3; i++)
	{
		Sys_Delay_MS(100);
		RET = TCP_Create_Link(host,port);
		if(RET==0){
			return 0;
		}
		else{
			break;
		}
	}

	if(RET==-1)
		DispStr_CE(0,12,"�������",DISP_POSITION);
	else if(RET==-2)
		DispStr_CE(0,12,"���ӷ�����ʧ��",DISP_POSITION);
	else
		DispStr_CE(0,12,"���ӷ�����δ֪����",DISP_POSITION);

	DispStr_CE(0,36,"��F2�˳���",DISP_POSITION|DISP_CLRLINE); 	

	WarningBeep(2);
	KEY_Flush_FIFO();
	delay_and_wait_key(30,EXIT_KEY_F2,30);	
	return RET;
}

short SendData(unsigned char* data){    //��������    0 ��ʾ���ͳɹ� -1 ��ʾʧ�� 
	int RET = 0;

	//�����ź�ǿ��
	RET = TestSignal();
	if(RET != 0)
	{
		DispStr_CE(0,12,"�ź�̫��,���Ժ����ϴ�...",DISP_CENTER|DISP_CLRSCR);
		WarningBeep(2);
		KEY_Flush_FIFO();
		delay_and_wait_key(30,EXIT_KEY_ALL,30);			
		return RET;
	}

	//�����Ƿ������Ϸ�����
	RET = TCP_Check_Link();
	if(RET != 0)
	{
		DispStr_CE(0,12,"�������Ͽ�,�����ϴ�.",DISP_CENTER|DISP_CLRSCR);
		WarningBeep(2);
		KEY_Flush_FIFO();
		delay_and_wait_key(30,EXIT_KEY_ALL,30);				
		return RET;
	}
	
	DispStr_CE(0,12,"�����ϴ������Ե�...",DISP_CENTER|DISP_CLRSCR);
	RET=TCP_Send_Data(data,strlen((char *)data));
	if(RET==0){//���ͳɹ� 
		return 0;
	}

	return RET;
}

short GetRecvData(unsigned char* recvdata){//���շ��������صĽ�� 0 ��ʾ���ճɹ���-1 ��ʾ����ʧ�� 
	int len=0;
	int RET=-1;

	//�����Ƿ������Ϸ�����
	RET = TCP_Check_Link();
	if(RET != 0)
	{
		DispStr_CE(0,12,"�������Ͽ�,���ܽ�������.",DISP_CENTER|DISP_CLRSCR);
		WarningBeep(2);
		delay_and_wait_key(30,EXIT_KEY_ALL,30);				
		return RET;
	}

	RET = TCP_Recv_Data(recvdata,&len,SEVEN_SECOND);
	if(RET==0){
		return 0;
	}

	return RET;
}


//����GPRS��Ϣ��
int TestSignal(void){    //��������    0 ��ʾ���ͳɹ� -1 ��ʾʧ�� 
	int i = 0;
	int signal = 10;
	//int signal = 30;
	int RET = 0;
	char Temp[40];
	short Key = 0;
	
	DispStr_CE(0,12,"�źŲ��ԣ����Ե�...",DISP_CENTER|DISP_CLRSCR);
	while(1)
	{
		memset(Temp, '\0', sizeof(Temp));
		sprintf(Temp, "����ź�ǿ��%d", signal);
		DispStr_CE(0,14,Temp,DISP_CENTER|DISP_CLRLINE);				
		DispStr_CE(0,36,"��F1�˳���",DISP_CENTER | DISP_CLRLINE);
		RET=SIM900_Get_Signal();				
		
		memset(Temp, '\0', sizeof(Temp));
		sprintf(Temp, "�ź�ǿ��(0-32):%d,����:%d��", RET, ++i); 
		DispStr_CE(0,16,Temp,DISP_CENTER|DISP_CLRLINE);		
		if(RET>=signal)
		{
			Sys_Delay_MS(100);
			return(0);	
		}
		
		Sys_Delay_MS(500);
		Key = KEY_read();
		KEY_Flush_FIFO();
		if(Key == KEY_F1)
		{break;}
	}
	return(-1);
}

