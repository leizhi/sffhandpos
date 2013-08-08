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

/*
*Display API
*/

short browse_info(int startline,char *p_menu,int *p_cPtr,int *p_lPtr,short flag ){
	BROWINFO	bi;
	short	ret;
	if(flag ==0){
		Disp_Clear();
		DispStr_CE(0,0,"�ύ��¼��Ϣ����",DISP_CENTER);
		Disp_Goto_XY(0,36);
		DispStr_CE(0,36,"��F1�˳��ύ��",DISP_CURRENT);
		DispStr_CE(0,36,"��F3ȷ���ύ��",DISP_RIGHT);
	}else if(flag ==1){
		Disp_Clear();
		DispStr_CE(0,0,"�����¼��Ϣ����",DISP_CENTER);
		Disp_Goto_XY(0,36);
		DispStr_CE(0,36,"��F3�˳������",DISP_RIGHT);
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
	WarningBeep(2);
	DispStr_CE(0,2,"�ر�����",DISP_CLRSCR|DISP_CENTER);
	EXT_Display_Multi_Lines("�˹���Ҫͨ��GPRSͨ�ţ����ܻ���Σ�գ���ȷ�������Ƿ�ȫ��",6,10);
	EXT_ClearLine(36,0);
	Disp_Goto_XY(0,36);
	DispStr_CE(0,36,"��F1�˳���",DISP_CURRENT);
	DispStr_CE(0,36,"��F3������",DISP_RIGHT);
	long key_value; 
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
	Sys_Auto_Poweroff_Set(60*2);		//Power off in 2 minutes if not do anything
	
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
		WarningBeep(2);
		delay_and_wait_key(0,EXIT_KEY_F2,0);
		return ; 
	}

	//�������ݿ�ɹ�
	DispStr_CE(0,4,"��ˢ����½",DISP_CENTER|DISP_CLRSCR);
	EXT_Display_Multi_Lines("�뽫������ݿ����ڸ�Ӧ���·���Ȼ��F3��½�����е�½������������˳����԰�F1�˳���",8,16);
	EXT_ClearLine(36,0);
	Disp_Goto_XY(0,36);
	DispStr_CE(0,36,"��F1�˳���",DISP_CURRENT);
	DispStr_CE(0,36,"��F3��½��",DISP_RIGHT);

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

	//EXT_Display_Multi_Lines("�����������Ѳ��",8,10);
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
			char record[200];
			memset(record,0,200); 
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

			EXT_ClearLine(36,0);
			Disp_Goto_XY(0,36);
			DispStr_CE(0,36,"��F1�˳���",DISP_CURRENT);
			DispStr_CE(0,36,"��F3������",DISP_RIGHT);
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

	RET = sim300_init();//��������
	if(RET ==0){//��������ɹ�
		RET = ConnectServer();//���ӷ����� 
		if(RET == 0){//���ӷ������ɹ�
			return 0; 
		}
	}

	DispStr_CE(0,4,"���ӷ�����ʧ�ܣ�������˳�",DISP_CENTER|DISP_CLRSCR);
	//WarningBeep(2);
	sim300_close();
	// delay_and_wait_key(0,EXIT_KEY_ALL,0);
	return -1;
}

//======================================================================
//��������SubmitData 
//����  ����������ύ 
//����  ���� 
//����ֵ���� 
//======================================================================
void SubmitData(){

	long choose ;
	int cLoop =0;

	int i=0;
	
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
	EXT_ClearLine(36,0);
	Disp_Goto_XY(0,36);
	DispStr_CE(0,36,"��F1�˳���",DISP_CURRENT);
	DispStr_CE(0,36,"��F3��½��",DISP_RIGHT);
	long key_value; 
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

	int RET = LoginToSubmit();
/*	DispStr_CE(0,0,"ReadNameAndPassword",DISP_CENTER|DISP_CLRSCR);

	char tmpbuf[100];
	sprintf(tmpbuf,"RET: %d",RET);
	DispStr_CE(0,0,tmpbuf,DISP_POSITION|DISP_5x7);
*/
	if(RET != 0){
		DispStr_CE(0,0,"ˢ��ʧ��!",DISP_CENTER|DISP_CLRSCR);
		return;
	}

	//ˢ����½�ɹ�   ��ȡ�˵�½�û���������
	int maxs=60*PAGENUM+5;
	int maxr=6*PAGENUM+5;

	unsigned char senddata[maxs];
	unsigned char recvdata[maxr];

	memset(senddata,0,maxs);
	memset(recvdata,0,maxr);

	//�״μ������
	RET=EncodeSendData(sname,spass,senddata);
	if(RET<0){
		CreateDatabase();

		WarningBeep(0); 
		DispStr_CE(0,0,"û�������ύ����������˳�",DISP_CENTER|DISP_CLRSCR);
		delay_and_wait_key(30,EXIT_AUTO_QUIT|EXIT_KEY_ALL,30);

		return; 
	}

	//��GPRSģ�� && ������
	for(i=0;i<3;i++){
		if((RET=OpenGPRS())==0){
			break;
		}
	}

	//GPRS��ʧ��
	if(RET != 0){ 
		DispStr_CE(0,0,"����SIM����ͨ��ģ��",DISP_CENTER|DISP_CLRSCR); 
		DispStr_CE(0,2,"��������˳�",DISP_CENTER); 
		delay_and_wait_key(30,EXIT_AUTO_QUIT|EXIT_KEY_ALL,30);

		return;
	}

	int upCount =0;
	while(1){
		memset(senddata,0,maxs);
		memset(recvdata,0,maxr);

		//�ݴ�û������
		RET=EncodeSendData(sname,spass,senddata);

		if(RET<0){
			CreateDatabase();

			WarningBeep(0); 
			DispStr_CE(0,0,"�����ϴ���ϣ���������˳�",DISP_CENTER|DISP_CLRSCR);
			delay_and_wait_key(30,EXIT_AUTO_QUIT|EXIT_KEY_ALL,30);

			break; 
		}

		//�ϴ�����
		WarningBeep(0);
		RET = Display(0);
		if(RET == -1){//�˳� 
			break;
		}

		upCount++;
		//��������
		RET = SendData(senddata);
		if(RET==-1){
			//����ʧ��
			WarningBeep(2);

			DispStr_CE(0,0,"����Ͽ�������������",DISP_CENTER|DISP_CLRSCR);

			Disp_Goto_XY(0,36);
			DispStr_CE(0,36,"��F1�˳�������",DISP_CURRENT);

			long temp_value; 
			temp_value=delay_and_wait_key(30,EXIT_KEY_F1,30);
			if(EXIT_KEY_F1 == temp_value || cLoop==3){//�˳� 
				break;
			}
			continue;
		}

		//���ͳɹ� && ���շ���ֵ
		RET =  GetRecvData(recvdata);
		if(RET !=0){ //����ʧ��
			WarningBeep(2); 
			DispStr_CE(0,4,"�����쳣 �Ժ�����!",DISP_CENTER|DISP_CLRSCR);
			Disp_Goto_XY(0,36);
			DispStr_CE(0,36,"��F1�˳���",DISP_CURRENT);
			delay_and_wait_key(0,EXIT_KEY_F1,0);

			break;
		}

		//���������Ϣ
		RET =HandleRecvData(recvdata);
		if(RET == 1){//�û������� 
			WarningBeep(2); 
			DispStr_CE(0,4,"�û�������",DISP_CENTER|DISP_CLRSCR);
			DispStr_CE(0,6,"��ȷ�Ϻ����ύ��ллʹ�ã�",DISP_CENTER);
			Disp_Goto_XY(0,36);
			DispStr_CE(0,36,"��F1�˳���",DISP_CURRENT);
			delay_and_wait_key(0,EXIT_KEY_F1,0);

			break;
		}else if(RET ==2){//�û�������� 
			WarningBeep(2);
			DispStr_CE(0,4,"�û��������",DISP_CENTER|DISP_CLRSCR);
			DispStr_CE(0,6,"��ȷ�Ϻ����ύ��ллʹ�ã�",DISP_CENTER);
			Disp_Goto_XY(0,36);
			DispStr_CE(0,36,"��F1�˳���",DISP_CURRENT);
			delay_and_wait_key(0,EXIT_KEY_F1,0);

			break;
		}else if(RET ==3){//�˳� 

			break;
		}else{
			WarningBeep(0);
			DispStr_CE(0,0,"��������ύ��һ��",DISP_CENTER|DISP_CLRSCR);
			delay_and_wait_key(30,EXIT_AUTO_QUIT|EXIT_KEY_ALL,30);
		}
	}//loop while

	//free
	sim300_close();
	DisConnectServer();
}

void FormatDatabase(){
     WarningBeep(2);
     DispStr_CE(0,4,"��ܰ����",DISP_CENTER|DISP_CLRSCR);
     
     EXT_Display_Multi_Lines("�ò�������ȫ�����������ݣ������ָܻ��������ݣ����������!",8,16);
     EXT_ClearLine(36,0);
     Disp_Goto_XY(0,36);
     DispStr_CE(0,36,"��F1ȡ����",DISP_CURRENT);
     DispStr_CE(0,36,"��F3ȷ����",DISP_RIGHT);  
     long key_value ;
     key_value = delay_and_wait_key(30,EXIT_KEY_F1|EXIT_KEY_F3|EXIT_AUTO_QUIT,30);
     if((key_value == EXIT_KEY_F1)||(key_value == EXIT_AUTO_QUIT))
     {
          return ;                             
     }
     else if(key_value == EXIT_KEY_F3)
     {
         int i =0;
         while(i<3)
         {
           if(CreateDatabase()==0)
           {
               WarningBeep(0);
               DispStr_CE(0,4,"��ʽ���ɹ���������˳�",DISP_CENTER|DISP_CLRSCR);
               delay_and_wait_key(30,EXIT_KEY_ALL|EXIT_AUTO_QUIT,30);
              break;
           }
           else
           {
             i++;
           }
         }
         if(i ==3)
         {
                WarningBeep(2);
               DispStr_CE(0,4,"��ʽ��ʧ�ܣ�������˳�",DISP_CENTER|DISP_CLRSCR);
               delay_and_wait_key(30,EXIT_KEY_ALL|EXIT_AUTO_QUIT,30);
              return ;
         }
         
     }
     

}

void  SysSetMenu()
{
    int ret;
    int flag =1;
         BROWINFO  sys_set_menu;
        char MAIN_MENU[] = "1. ����ʱ��  2. ��ʽ���ݱ�3. �˳�����  "; 
        char welcome[20];
        char choose[20];
        memset(welcome,0,20);
        memset(choose,0,20);
        strcpy(welcome ,"��ӭʹ��");
        strcpy(choose,"��ѡ����ģ��");
        
       
        sys_set_menu.iStr = MAIN_MENU;
        sys_set_menu.lPtr = 0;
        sys_set_menu.cPtr = 0;
        sys_set_menu.startLine = 10;
        while(flag)
        {
           Disp_Clear();
           
           DispStr_CE(0,4,welcome,DISP_CENTER|DISP_CLRSCR);
           DispStr_CE(0,6,choose,DISP_CENTER);
           
           
           sys_set_menu.dispLines = 3;
           sys_set_menu.mInt = 3;
           sys_set_menu.lineMax = 13;
           sys_set_menu.sFont = 0;
           sys_set_menu.numEnable = 1;
           sys_set_menu.qEvent = EXIT_KEY_F1 ; 
            Disp_Set_Magnification(2);
           ret = EXT_Brow_Select(&sys_set_menu);
            Disp_Set_Magnification(1);
         
            
           switch(ret)
           {
              case 0:
              {
                 Modify_Date();
                 Modify_Time();
                break;
              }
              case 1:
              {
                  FormatDatabase(); 
                 break;
              }
              case 2:
              {
                flag =0;
                break;
              }
              default:
              {
                break;
              }
           }
        }
} 

void GetInfo(){ //��ǩУ�� 

	long key_value;
	int flag = 1;
	//int index=0; 

	while(flag){
		memset(anticode,0,ANTIFAKECODE_LEN);//��շ�α�� 
		short err = ultralight_local_read_card(anticode);

		//��ȡ��α��
		if(err ==0){
			//Disp_Set_Magnification(3);
			//Disp_Set_Magnification(1);

			DispStr_CE(0,4,"��ǩ��ȡ�ɹ�",DISP_CENTER|DISP_CLRSCR);
			DispStr_CE(0,8,"У �� �룺",DISP_POSITION);
			DispStr_CE(0,8,anticode,DISP_CURRENT);

			EXT_ClearLine(36,0);
			Disp_Goto_XY(0,36);
			DispStr_CE(0,36,"��F1�˳���",DISP_CURRENT);
			DispStr_CE(0,36,"��F3������",DISP_RIGHT);

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
	short ret;
	BROWINFO  main_menu;
	char MAIN_MENU[] = "1. ����Ѳ��2. GPRS�ϴ�3. ��ǩУ��4. ϵͳ����5. SFV1.6"; 
	char welcome[20];
	char choose[20];
	memset(welcome,0,20);
	memset(choose,0,20);
	strcpy(welcome ,"��ӭʹ��");
	strcpy(choose,"��ѡ����ģ��");
	main_menu.iStr = MAIN_MENU;
	main_menu.lPtr = 0;
	main_menu.cPtr = 0;
	main_menu.startLine = 10;

	while(1){
		Disp_Clear();
		DispStr_CE(0,4,welcome,DISP_CENTER|DISP_CLRSCR);
		DispStr_CE(0,6,choose,DISP_CENTER);

		main_menu.dispLines = 5;
		main_menu.mInt = 5;
		main_menu.lineMax = 11;
		main_menu.sFont = 0;
		main_menu.numEnable = 1;
		main_menu.qEvent = EXIT_KEY_F1 ;

		Disp_Set_Magnification(2);
		ret = EXT_Brow_Select(&main_menu);
		Disp_Set_Magnification(1);

		switch(ret){
			case 0:{
				//Ѳ��
				Query(); 
				break;
			}
			case 1:{
				//�ύ���� 
				SubmitData();
				break;
			}
			case 2:{
				GetInfo();
				break; 
			} 
			case 3:{  
				SysSetMenu();
			} 
                       
                        default :{
				break;
			}
		}
	} 
}

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
