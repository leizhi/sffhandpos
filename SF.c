/****************************************************************************
*
*�ļ�(File):         SF.c
*
*�޸�(Modify):       2012-7-13 13:58:26
*
*����(Author):       sa
*
*����(Compile):      ����ƽ̨(Smart Platform)
*
*����(Description):
*1..ARM ��Ŀ�ļ�(ARM Project):SF.dsp 
*2..��Ŀ����������ļ�(project binary output  file):SF.bin
*
----------------------------------------------------------------------------
|
| Version | Datetime             |   Author    | Description
| --------+----------------------+-------------+----------------------------
|
| V1.00  2012-7-13 13:58:26           sa 
----------------------------------------------------------------------------
****************************************************************************/

/*�ڴ����Ӵ���(TODO: add  code here)*/

#include "API.h"
#include "Card.h"
#include "DataBase.h"
#include "Gprs.h"
#include "UiProg.h"
#include "SF.h"

//======================================================================
//��������main 
//����  �������� ������� 
//����  ���� 
//����ֵ��int
//======================================================================
int main(void){
	InitSystem();
	MainMenu();
	return 0;
}


short browse_info(int startline,char *p_menu,int *p_cPtr,int *p_lPtr,short flag ){
	BROWINFO	bi;
	short	ret = 0;

	if(flag ==0){
		Disp_Clear();
		DispStr_CE(0,0,"�������ύ��¼��Ϣ����:",DISP_CENTER);
		KEY_Flush_FIFO();
		DispStr_CE(0,36,"��F1�˳��ύ����F3ȷ���ύ��",DISP_CLRLINE|DISP_CENTER);
	}else if(flag ==1){
		Disp_Clear();
		DispStr_CE(0,0,"�����¼��Ϣ����",DISP_CENTER);
		DispStr_CE(0,36,"��F3�˳������",DISP_RIGHT | DISP_CLRLINE);
	}

	if(startline>18) startline = 2;

	bi.startLine = startline;
	bi.dispLines = 18 - startline;
	bi.iStr = p_menu;
	bi.mInt = strlen(p_menu)/28;
	bi.lineMax = 28;
	bi.sFont = 0;
	bi.numEnable = 1;

	if(flag ==1){
		bi.qEvent = EXIT_KEY_F3;
	}else if(flag ==0){
		bi.qEvent = EXIT_KEY_F1|EXIT_KEY_F3;
	}

	bi.autoexit = 0;
	bi.cPtr = *p_cPtr;
	bi.lPtr = *p_lPtr;
	ret = brow_info( &bi );
	*p_cPtr = bi.cPtr;
	*p_lPtr = bi.lPtr;

	if(flag ==0) {
		if(bi.qEvent==EXIT_KEY_F1)
			return KEY_F1;

		if(bi.qEvent==EXIT_KEY_F3)
			return KEY_F3;
	}else if(flag ==1){
		if(bi.qEvent==EXIT_KEY_F3)
			return KEY_F3;
	}
	return 0;
}

void PackUpMenuData(char menu[], int MenuCount, int LineLen){
	int i = 0;
	menu[MenuCount * LineLen + 2] = '\0';
	for(i = 0; i < (MenuCount * LineLen + 1); ++i){
		if(menu[i] == '\0'){
			menu[i] = 0x20;
		}
	}
}

short Display(short flag){
	int lPtr = 0;
	int cPtr=0;
	int IntRet;

	PackUpMenuData(Menu, 150, 28);
	while(1){ 	
		IntRet = browse_info(2,Menu,&cPtr,&lPtr,flag);
		if(IntRet == KEY_F1){ //�˳� 
			return -1;
		}else if(IntRet == KEY_F3){//ȷ���ύ 
			return 0;
		}
	}
}

long Alert(){
	long key_value = 0; 
	
	WarningBeep(2);
	DispStr_CE(0,2,"�ر�����",DISP_CLRSCR|DISP_CENTER);
	EXT_Display_Multi_Lines("�˹���Ҫͨ��GPRSͨ�ţ����ܻ���Σ�գ���ȷ�������Ƿ�ȫ��",6,10);	
 	DispStr_CE(0,36,"��F1�˳���     ��F3������",DISP_CENTER | DISP_CLRLINE);
 	key_value = delay_and_wait_key(30,EXIT_KEY_ENTER|EXIT_KEY_F1|EXIT_KEY_F3|EXIT_AUTO_QUIT,30);
	return key_value;
}

//======================================================================
//��������InitSystem 
//����  ��ϵͳ��ʼ������ 
//����  ���� 
//����ֵ���� 
//======================================================================
void InitSystem()
{
  Disp_Init(DISP_INIT_ON);		//Open LCD
	Disp_Clear();				
	
	KEY_Init(KEY_BEEP_KEY|KEY_AUTO_EL_ON);	//Open Keyborad
	KEY_Beep_Mask_Set(0x00000000);			
	KEY_EL_Mask_Set(0xffffffff);
	EL_Set_Timeout(60);			//close EL in 10 second if not press any key
	Sys_Auto_Poweroff_Set(AutoPowerOff);		//Power off in 2 minutes if not do anything
	
	//Meanless for 0518
	Disp_Icon_Battery_Time_Set(1,ICON_AT_TAIL);
	Disp_Icon_Battery_Time_Refresh();
	
	Disp_set_color(SET_FONT_COLOR, 0x0000);		//Black
	Disp_Set_Color(SET_BACK_COLOR, 0xffff);	//green
	Disp_set_color(SET_SELECT_COLOR,  0x07E0);	//Green
	Disp_set_color(SET_CLEAR_COLOR, 0xffff);	//White
}

//======================================================================
//��������Query 
//����  �����Ѳ�칦�� 
//����  ���� 
//����ֵ���� 
//======================================================================
void Query(){
	int ret ;
	long key_value;
	char record[200];

	ret = FindDatabase(); 
	//û�ҵ����ݿ� ���� 
	if(ret != 0){
		int i=0; 
		for(i=0;i<4;i++){
			if((ret = CreateDatabase())==0)
				break;
		}
	}

	if(ret != 0){
		DispStr_CE(0,0,"�������ݿ�ʧ��",DISP_CLRSCR|DISP_CENTER);
		DispStr_CE(0,36,"��F2�˳���",DISP_POSITION | DISP_CLRLINE);		
		WarningBeep(2);
		delay_and_wait_key(0,EXIT_KEY_F2,0);
		return ; 
	}

	//�������ݿ�ɹ�
	DispStr_CE(0,4,"��ˢ����½",DISP_CENTER|DISP_CLRSCR);
	EXT_Display_Multi_Lines("�뽫������ݿ����ڸ�Ӧ���·���Ȼ��F3��½�����е�½������������˳����԰�F1�˳���",8,16);
 	DispStr_CE(0,36,"��F1�˳���     ��F3��½��",DISP_CENTER | DISP_CLRLINE); 
	key_value = delay_and_wait_key(30,EXIT_KEY_ENTER|EXIT_KEY_F1|EXIT_KEY_F3|EXIT_AUTO_QUIT,30);
	switch(key_value){
		case EXIT_KEY_F1:
		case EXIT_AUTO_QUIT:{
			return ; 
		}

		case EXIT_KEY_F3:
		case EXIT_KEY_ENTER:{
			break;
		}
	}

	ret = LoginByCard();
	if(ret != 0){//��½ʧ��
		return ; 
	}

	//��½�ɹ�
	Disp_Clear();

	DispStr_CE(0,2,"��½�ɹ�!",DISP_CENTER|DISP_CLRSCR);
	DispStr_CE(0,4,"Ѳ �� ��:",DISP_POSITION);
	DispStr_CE(0,4,(char *)&username,DISP_CURRENT);

	DispStr_CE(0,8,"�����������Ѳ��",DISP_CENTER);
	EXT_Display_Multi_Lines("�뽫Ѳ��оƬ���ڸ�Ӧ���·��������������Ѳ�졣",10,16);
	WarningBeep(0);
	delay_and_wait_key(0,EXIT_KEY_ALL,0);

	//�õ���username
	//��ȡ��α�� �� ʱ��
	int flag =1; 
	while(flag){
		memset(systime,0,QUERYTIME_LEN);   //���ϵͳʱ�� 
		memset(anticode,0,ANTIFAKECODE_LEN);  //��շ�α�� 
		//��ȡ��α��
		if(ultralight_local_read_card(anticode)==0){
			//��ȡ��α��ɹ� 
			//��ȡϵͳʱ��
			GetSysTime(systime);
			//�����¼ 
			//short AddOneRecord(char* name,char* time,char* code)			
			memset(record,0,sizeof(record)); 

			/*
			{//lzy ���ɲ�������
				int i = 0;
				for(i = 0; i < 400; i++)
				{
					memset(systime, '\0', sizeof(systime));
					GetSysTime(systime);
					AddOneRecord(username,systime,anticode,record);
					Sys_Delay_MS(500);
				}
			}
			*/
			
			ret = AddOneRecord(username,systime,anticode,record);
			if(ret == 0){   //����ɹ� 
				DispStr_CE(0,0,"Ѳ��ɹ�",DISP_CENTER|DISP_CLRSCR);
				DispStr_CE(0,2,"Ѳ��������",DISP_CENTER);
				DispStr_CE(0,6,"Ѳ �� �ˣ�",DISP_POSITION);
				DispStr_CE(0,6,(char *)username,DISP_CURRENT);
				DispStr_CE(0,8,"�� α �룺",DISP_POSITION);
				DispStr_CE(0,8,anticode,DISP_CURRENT);
				DispStr_CE(0,10,"Ѳ��ʱ�䣺",DISP_POSITION);
				DispStr_CE(0,10,systime,DISP_CURRENT);
				DispStr_CE(0,12,"�� ¼ �ţ�",DISP_POSITION);
				DispStr_CE(0,12,record,DISP_CURRENT);
			}else{//����ʧ�� 
				DispStr_CE(0,0,"�����¼ʧ�ܣ������Ѳ��",DISP_CENTER|DISP_CLRSCR); 
			}

 			DispStr_CE(0,36,"��F1�˳���     ��F3������",DISP_CENTER | DISP_CLRLINE); 
			key_value = delay_and_wait_key(0,EXIT_KEY_F1|EXIT_KEY_F3,0);
			switch(key_value){
				case EXIT_KEY_F1:{
					flag = 0;//�˳�ѭ��
					break;
				}
				case EXIT_KEY_F3:{
					flag =1; 
					break; 
				}
			}
		}else{
			flag = 0; 
		} 
	}//end while
}

short OpenGPRS(){
	short RET=-1;

	RET = sim900_init();//��������
	if(RET ==0){//��������ɹ�
		RET = ConnectServer();//���ӷ����� 
		if(RET == 0){//���ӷ������ɹ�
			return 0; 
		}
	}

	DispStr_CE(0,4,"���ӷ�����ʧ�ܣ�������˳�",DISP_CENTER|DISP_CLRSCR);
	sim900_close();
	KEY_Flush_FIFO();
	return -1;
}

//======================================================================
//��������SubmitData 
//����  ����������ύ 
//����  ���� 
//����ֵ���� 
//======================================================================
void SubmitData(){
	int cLoop =0;
	int i=0;
	int RET = 0;	
	long choose = 0;
	long key_value = 0; 
	unsigned char data[1024];
	
	choose = Alert(); 
	switch(choose){
		case EXIT_KEY_F1:
		case EXIT_AUTO_QUIT:{
			return;
		}
		
		case EXIT_KEY_F3:
		case EXIT_KEY_ENTER: {
			break;
		}
	}

	//ˢ����½ ��Ϣ��֤ 
	DispStr_CE(0,4,"��ˢ����½",DISP_CENTER|DISP_CLRSCR);
	EXT_Display_Multi_Lines("�뽫������ݿ����ڸ�Ӧ���·���Ȼ��F3��½�����е�½������������˳����԰�F1�˳���",8,16);	
 	DispStr_CE(0,36,"��F1�˳���     ��F3��½��",DISP_CENTER | DISP_CLRLINE); 	
	key_value = delay_and_wait_key(30,EXIT_KEY_ENTER|EXIT_KEY_F1|EXIT_KEY_F3|EXIT_AUTO_QUIT,30);
	switch(key_value){
		case EXIT_KEY_F1:
		case EXIT_AUTO_QUIT:{
			return ;
		}

		case EXIT_KEY_F3:
		case EXIT_KEY_ENTER:{
			break;
		}
	}
 	RET = LoginToSubmit();
	if(RET != 0){
		DispStr_CE(0,2,"ˢ��ʧ��!",DISP_POSITION|DISP_CLRSCR);
		return;
	}

	memset(data,'\0',sizeof(data));

	//�״μ������
	RET=EncodeSendData(sname,spass,data);
	if(RET<0){
		CreateDatabase();

		WarningBeep(0); 
		DispStr_CE(0,2,"û�������ύ����������˳�",DISP_POSITION|DISP_CLRSCR);
		KEY_Flush_FIFO();
		delay_and_wait_key(30,EXIT_AUTO_QUIT|EXIT_KEY_ALL,30);

		return;
	}
	//��ʼ��ģ��
	RET = sim900_init();
	if(RET != 0)
	{
		DispStr_CE(0,4,"���ӷ�����ʧ�ܣ�������˳�",DISP_CENTER|DISP_CLRSCR);
		KEY_Flush_FIFO();
		delay_and_wait_key(0,EXIT_KEY_ALL,0);
		return;
	}
	
	//��GPRSģ�� && ������
	for(i=0;i<3;i++){
		RET = ConnectServer();
		if(RET == 0){
			break;
		}
	}

	//GPRS��ʧ��
	if(RET != 0){ 
		DispStr_CE(0,2,"����SIM����ͨ��ģ��",DISP_POSITION|DISP_CLRSCR); 
		DispStr_CE(0,4,"��������˳�",DISP_POSITION); 
		KEY_Flush_FIFO();
		delay_and_wait_key(30,EXIT_AUTO_QUIT|EXIT_KEY_ALL,30);

		return;
	}
 
	while(1){
		memset(data,'\0',sizeof(data));

		//�ݴ�û������
		RET=EncodeSendData(sname,spass,data);
		if(RET<1){
			DB_erase_filesys(0);
			//CreateDatabase();

			WarningBeep(0);
			DispStr_CE(0,2,"���������ϴ���ϣ���������˳�",DISP_POSITION|DISP_CLRSCR);
			KEY_Flush_FIFO();
			delay_and_wait_key(30,EXIT_AUTO_QUIT|EXIT_KEY_ALL,30);

			break;
		}

		//�ϴ�����
		WarningBeep(0);
		RET = Display(0);
		if(RET == -1){//�˳� 
			break;
		}

		//��������
		RET = SendData(data);

		if(RET==-1){
			//����ʧ��
			cLoop++;
			
			WarningBeep(2);
			DispStr_CE(0,4,"���ݷ���ʧ��!",DISP_POSITION|DISP_CLRSCR);
			DispStr_CE(0,36,"��F1�˳�������",DISP_POSITION | DISP_CLRLINE);
			KEY_Flush_FIFO();
			
			long temp_value;
			temp_value=delay_and_wait_key(30,EXIT_KEY_F1,30);
			if((EXIT_KEY_F1 == temp_value) && (cLoop == 3)){//�˳� 
				break;
			}

			continue;
		}

		memset(data, '\0', sizeof(data));
		//���ͳɹ� && ���շ���ֵ
		RET = GetRecvData(data);
		if(RET<0){ //����ʧ��
			WarningBeep(2);
			DispStr_CE(0,4,"�ϴ��ɹ�",DISP_POSITION|DISP_CLRSCR); 
			//DispStr_CE(0,36,"��F1�˳���",DISP_POSITION | DISP_CLRLINE);
			KEY_Flush_FIFO();
			delay_and_wait_key(0,EXIT_KEY_ALL,0);
			//break;
		}
		else
		{
			//���������Ϣ
			RET =HandleRecvData(data);
			if(RET<0){
				//dbClean();
				/*WarningBeep(2);
				DispStr_CE(0,4,"no data",DISP_POSITION|DISP_CLRSCR);
				KEY_Flush_FIFO();
				delay_and_wait_key(0,EXIT_KEY_F1,0);
				*/
			}else if(RET == 0){
				dbClean();
			}else if(RET == 1){//�û�������
				WarningBeep(2);
				DispStr_CE(0,4,"�û�������",DISP_POSITION|DISP_CLRSCR);
				DispStr_CE(0,6,"��ȷ�Ϻ����ύ��ллʹ�ã�",DISP_POSITION);
				DispStr_CE(0,36,"��F1�˳���",DISP_POSITION | DISP_CLRLINE);
				KEY_Flush_FIFO();
				delay_and_wait_key(0,EXIT_KEY_F1,0);

				break;
			}else if(RET == 2){//�û�������� 
				WarningBeep(2);
				DispStr_CE(0,4,"�û��������",DISP_POSITION|DISP_CLRSCR);
				DispStr_CE(0,6,"��ȷ�Ϻ����ύ��ллʹ�ã�",DISP_POSITION);
				DispStr_CE(0,36,"��F1�˳���",DISP_POSITION | DISP_CLRLINE);
				KEY_Flush_FIFO();
				delay_and_wait_key(0,EXIT_KEY_F1,0);

				break;
			}else if(RET == 3){//�˳�
				//�������ݿ�
				UpdateDatabase(data);
			}
		}
	}//loop while

	//free
	DisConnectServer();
 }

void FormatDatabase(){
	long key_value = 0;
	WarningBeep(2);
	DispStr_CE(0,4,"��ܰ����",DISP_CENTER|DISP_CLRSCR);

	EXT_Display_Multi_Lines("�ò�������ȫ�����������ݣ������ָܻ��������ݣ����������!",8,16);
 	DispStr_CE(0,36,"��F1�˳���     ��F3ȷ����",DISP_CENTER | DISP_CLRLINE); 	
	key_value = delay_and_wait_key(30,EXIT_KEY_F1|EXIT_KEY_F3|EXIT_AUTO_QUIT,30);
	if((key_value == EXIT_KEY_F1)||(key_value == EXIT_AUTO_QUIT))	{
		return ;
	}else if(key_value == EXIT_KEY_F3){
		int i =0;
		while(i<3){
			if(CreateDatabase()==0){
				WarningBeep(0);
				DispStr_CE(0,4,"��ʽ���ɹ���������˳�",DISP_CENTER|DISP_CLRSCR);
				delay_and_wait_key(30,EXIT_KEY_ALL|EXIT_AUTO_QUIT,30);
				break;
			}else{
				i++;
			}
		}

		if(i ==3){
			WarningBeep(2);
			DispStr_CE(0,4,"��ʽ��ʧ�ܣ�������˳�",DISP_CENTER|DISP_CLRSCR);
			delay_and_wait_key(30,EXIT_KEY_ALL|EXIT_AUTO_QUIT,30);
			return ;
		}
	}
}

void  SysSetMenu(){
	int ret = 0; 
	int LineLen = 26;
	SMemu SMemuObj =
	{
		1, 3, 1,
		{		
			{
				{12, 64,	SetTime,	0}, {88, 64, FormDB, 0}, {164, 64, ExitSet, 0}, 
			},	
		},
		{3, 0, 0},0, 0,
	};		

	char MAIN_MENU[7 * 26 + 2] = 
		{
			"1. ����ʱ��               "
			"2. ��ʽ���ݱ�             "
			"3. �˳�����               "
		};


	while(1){
		Disp_Clear();

		ret = Browse_Icon("��ѡ����ģ��", MAIN_MENU, &SMemuObj, LineLen, 1, 0, 0);
		switch(ret){
			case 0:{
				Modify_Date();
				Modify_Time();
				break;
			}
			case 1:{
				FormatDatabase(); 
				break;
			}
			
			case 2:{
				return;
				break;
			}
			case -1:{
				return;
				}break;
			
			default:{
				break;
			}
		}
	}
} 

void GetInfo(){ //��ǩУ�� 
	long key_value = 0;
	int flag = 1;
 
	while(flag){
		memset(anticode,0,ANTIFAKECODE_LEN);//��շ�α�� 
		short err = ultralight_local_read_card(anticode);

		//��ȡ��α��
		if(err ==0){ 
			DispStr_CE(0,4,"��ǩ��ȡ�ɹ�",DISP_CENTER|DISP_CLRSCR);
			DispStr_CE(0,8,"У �� �룺",DISP_POSITION);
			DispStr_CE(0,8,anticode,DISP_CURRENT);

 			DispStr_CE(0,36,"��F1�˳���     ��F3������",DISP_CENTER | DISP_CLRLINE); 
			key_value = delay_and_wait_key(0,EXIT_KEY_F1|EXIT_KEY_F3,0); 
			switch(key_value){
				case EXIT_KEY_F1:{
					flag = 0;//�˳�ѭ�� 
					break;
				}
				case EXIT_KEY_F3:{
					break; 
				} 
			}

		}else{
			flag =0;
		}
	}
}

//======================================================================
//��������MainMenu 
//����  �����˵����� 
//����  ���� 
//����ֵ���� 
//======================================================================
void MainMenu(){
	//��ʼ���˵� 
	short ret = 0;
	int LineLen = 26;
	
	SMemu SMemuObj =
	{
		1, 5, 1,
		{		
			{
				{12, 64,	Offline,	0}, {88, 64, Upload, 0}, {164, 64, Check, 0}, 
				{12, 140, SysSet, 0}, {88, 140, VerInfo, 0},
			},	
		},
		{5, 0, 0},0, 0,
	};		
	
	char MAIN_MENU[7 * 26 + 2] = 
		{
			"1. ����Ѳ��               "
			"2. GPRS�ϴ�               "
			"3. ��ǩУ��               "
			"4. ϵͳ����               "
			"5. SFV2.08                "
		};

	Disp_Clear();
	while(1){
		Disp_Clear();
		ret = Browse_Icon("��ѡ����ģ��", MAIN_MENU, &SMemuObj, LineLen, 1, 0, 0);
		switch(ret){
			
			//Ѳ��
			case 0:{				
				Query(); 
				break;
			}

			//�ύ���� 
			case 1:{				
				SubmitData();
				break;
			}

			//��ǩУ��
			case 2:{
				GetInfo();
				break; 
			}

			//ϵͳ����
			case 3:{  
				SysSetMenu();
			} 

			default :{
				break;
			}
		}
	} 
}

