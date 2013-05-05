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
extern char username[USERNAME_LEN];
extern char password[USERNAME_LEN];
extern unsigned char  anticode[ANTIFAKECODE_LEN];
extern char systime[QUERYTIME_LEN];

extern char submitname[USERNAME_LEN];
extern char submitpassword[USERNAME_LEN];
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
}

void PackUpMenuData(char Menu[], int MenuCount, int LineLen)
{
	int i = 0;
	Menu[MenuCount * LineLen + 2] = '\0';
	for(i = 0; i < (MenuCount * LineLen + 1); ++i)
	{
		if(Menu[i] == '\0')
		{Menu[i] = 0x20;}
	}
}

short Display(short flag)
 {
 	int lPtr = 0;
         int cPtr=0;	
	int RCount = 0;
       //int i=0;
       int IntRet;
       
        PackUpMenuData(Menu, 150, 28);
	while(1)
	{ 	
	
	IntRet = browse_info(2,Menu,&cPtr,&lPtr,flag);
	if(IntRet == KEY_F1) //�˳� 
	{
	    return -1;
	}
	else if(IntRet == KEY_F3)      //ȷ���ύ 
	{
	  return 0;
	}
      }
 }
 long Alert()
{
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
      /*switch(key_value)
       {
         case EXIT_KEY_F1:
          case EXIT_AUTO_QUIT:
          {
               return ; 
          }
          case EXIT_KEY_F3:
          case EXIT_KEY_ENTER: 
          {
               DispStr_CE(0,0,"gprsͨ��",DISP_CENTER|DISP_CLRSCR);
               delay_and_wait_key(0,EXIT_KEY_F2,0); 
             break;
          } 
       } */
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
	EXT_Display_Multi_Lines("�뽫�������ݿ����ڸ�Ӧ���·���Ȼ��F3��½�����е�½������������˳����԰�F1�˳���",8,16);
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
			//DispStr_CE(0,0,"LOGIN",DISP_CENTER|DISP_CLRSCR);
			//delay_and_wait_key(0,EXIT_KEY_F2,0); 
			break;
		}
	}

	ret = LoginByCard();
	if(ret != 0){//��½ʧ��
		//DispStr_CE(0,0,"��½ʧ��",DISP_CLRSCR|DISP_CENTER);
		// WarningBeep(2); 
		// delay_and_wait_key(0,EXIT_KEY_F2,0);
		return ; 
	}

	//��½�ɹ�
	DispStr_CE(0,4,"��ϲ����½�ɹ�",DISP_CENTER|DISP_CLRSCR);
	DispStr_CE(0,6,"�����������Ѳ��",DISP_CENTER);
	EXT_Display_Multi_Lines("�뽫Ѳ��оƬ���ڸ�Ӧ���·��������������Ѳ�졣",8,16);
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
				DispStr_CE(0,6,username,DISP_CURRENT);
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

short OpenGPRS()
{
      short ret;
  
     ret = sim300_init();  //��������
     if(ret ==1)//��������ɹ�
     {
            ret = ConnectServer();  //���ӷ����� 
            if(ret == 0)  //���ӷ������ɹ�
           {
              return 0; 
           }
           else
           {
              goto err;
           } 
     } 
     else
     {
       goto err;
     }
     
     err:
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
	short failedtimes =0;
	short opengprstimes=0;
	choose = Alert(); 
	switch(choose){
		case EXIT_KEY_F1:
		case EXIT_AUTO_QUIT:{
			//DispStr_CE(0,0,"ѡ�����˳�",DISP_CENTER|DISP_CLRSCR);
			// delay_and_wait_key(0,EXIT_KEY_F2,0); 
			return;
		}
		case EXIT_KEY_F3:
		case EXIT_KEY_ENTER: {
			// DispStr_CE(0,0,"gprsͨ��",DISP_CENTER|DISP_CLRSCR);
			//delay_and_wait_key(0,EXIT_KEY_F2,0); 
			break;
		}
	}

	//ˢ����½ ��Ϣ��֤ 
	DispStr_CE(0,4,"��ˢ����½",DISP_CENTER|DISP_CLRSCR);
	EXT_Display_Multi_Lines("�뽫�������ݿ����ڸ�Ӧ���·���Ȼ��F3��½�����е�½������������˳����԰�F1�˳���",8,16);
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

	int ret ;
	ret = LoginToSubmit();
	if(ret == 0){//ˢ����½�ɹ�   ��ȡ�˵�½�û���������
		int flag=0;//����Ƿ��gprsģ�� 

		char senddata[3000];
		memset(senddata,0,3000);
		char recvdata[3000];
		memset(recvdata,0,3000);
		int index =0;
		while(1){
			/* DispStr_CE(0,0,"while ��ʼѭ��",DISP_CENTER|DISP_CLRSCR);  
			delay_and_wait_key(0,EXIT_KEY_ALL,0);  ;
			*/

			memset(senddata,0,3000);
			memset(recvdata,0,3000);

			if(EncodeSendData(submitname,submitpassword,senddata)==1){
				WarningBeep(0); 

				DispStr_CE(0,0,"��¼ȫ���ύ����������˳�",DISP_CENTER|DISP_CLRSCR);

				delay_and_wait_key(30,EXIT_AUTO_QUIT|EXIT_KEY_ALL,30);

				if(index ==0){
					sim300_close();
				}else{
					index =0;
					DisConnectServer();
				}

				CreateDatabase();

				return; 
			}else{
				index++;
				if(flag == 0){ //û�д�gprsģ��   ��gprsmok 
					ret=OpenGPRS();
					if(ret ==0){
						flag = 1;
					}else{//ʧ�� 
						opengprstimes++;

						if(opengprstimes ==3){
							//WarningBeep(2);
							// sim300_close(); 
							DispStr_CE(0,0,"����SIM����ͨ��ģ��",DISP_CENTER|DISP_CLRSCR); 
							DispStr_CE(0,2,"��������˳�",DISP_CENTER); 
							delay_and_wait_key(30,EXIT_AUTO_QUIT|EXIT_KEY_ALL,30);
							return;
						}

						continue;
						flag = 0;
					}
				}else{	//�Ѿ��򿪷����� 
					//��ʾҪ���͵�����
					// DispStr_CE(0,0,"�����ύ�������£�",DISP_CENTER|DISP_CLRSCR);
					//DispStr_CE(0,2,"��������ύ",DISP_CENTER);

					WarningBeep(0);

					// EXT_Display_Multi_Lines(senddata,4,34);

					short err = Display(0);
					if(err == -1){//�˳� 
						DisConnectServer();
						return;
					}else if(err == 0){//ȷ���ύ 

					}
					//��������
					if(SendData(senddata)==-1){
						//����ʧ��
						WarningBeep(2);
						failedtimes++;
						if(failedtimes<3){
							DispStr_CE(0,0,"����Ͽ�������������",DISP_CENTER|DISP_CLRSCR);

							Disp_Goto_XY(0,36);
							DispStr_CE(0,36,"��F1�˳�������",DISP_CURRENT);
							// DispStr_CE(0,36,"��F3�ٴγ��ԡ�",DISP_RIGHT);
							long temp_value; 
							temp_value=delay_and_wait_key(30,EXIT_KEY_F1,30);
							if(EXIT_KEY_F1 == temp_value){//�˳� 
								sim300_close();
								return; 
							}
						}else{
							failedtimes =0;
							DispStr_CE(0,0,"���Դ������࣬��˶Ժ������ԣ�",DISP_CENTER|DISP_CLRSCR);
						}
						delay_and_wait_key(0,EXIT_KEY_ALL,0);
					}else{//���ͳɹ� 
						//���շ���ֵ
						ret =  GetRecvData(recvdata);
						if(ret ==0){ //���ճɹ� 
							//������Ϣ
							int err =HandleRecvData(recvdata);
							if(err ==0){//��ȫ��ȷ �˳�
								//������һ��
								// EXT_ClearLine(36,0);
								//Disp_Goto_XY(0,36);
								// DispStr_CE(0,36,"��F3��һ���ϴ���",DISP_RIGHT);
								continue; 
							}else if(err == 4){//����
								// DispStr_CE(0,0,"�����ϴ���һ�� ���Ե�",DISP_CENTER|DISP_CLRSCR);
								//  delay_and_wait_key(0,EXIT_KEY_F2,0); 
								continue; 
							}else if(err ==3){    //�˳� 
								DisConnectServer();
								return; 
							}else if(err == 1){   //�û������� 
								WarningBeep(2); 
								DispStr_CE(0,4,"�û�������",DISP_CENTER|DISP_CLRSCR);
								DispStr_CE(0,6,"��ȷ�Ϻ����ύ��ллʹ�ã�",DISP_CENTER);
								Disp_Goto_XY(0,36);
								DispStr_CE(0,36,"��F1�˳���",DISP_CURRENT);
								delay_and_wait_key(0,EXIT_KEY_F1,0);
								DisConnectServer();
								return; 
							}else if(err ==2){//�û�������� 
								WarningBeep(2);
								DispStr_CE(0,4,"�û��������",DISP_CENTER|DISP_CLRSCR);
								DispStr_CE(0,6,"��ȷ�Ϻ����ύ��ллʹ�ã�",DISP_CENTER);
								Disp_Goto_XY(0,36);
								DispStr_CE(0,36,"��F1�˳���",DISP_CURRENT);
								delay_and_wait_key(0,EXIT_KEY_F1,0);
								DisConnectServer();
								return;
							}
						}else{//����ʧ��
							flag =0;
							sim300_close();
						}
					}
				}
			}
		}
	}
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

  void GetInfo() //��ǩУ�� 
{

long key_value;
int flag = 1;
int index=0; 
char temp[5];
   while(flag)
   {      
      memset(anticode,0,ANTIFAKECODE_LEN);  //��շ�α�� 
      short err = ultralight_local_read_card(anticode); 
    
    //��ȡ��α��
    if(err ==0)
    {
             DispStr_CE(0,2,"��ȡ��Ϣ�ɹ�",DISP_CENTER|DISP_CLRSCR);
             DispStr_CE(0,6,"��ǩ�������",DISP_CENTER);          
             DispStr_CE(0,10,"��α��:",DISP_POSITION);
   	     
                Disp_Set_Magnification(3);
              for(index =0;index<4;index++)
              {
                 memset(temp,0,5);
                strncpy(temp,anticode+index*4,4);
                
                DispStr_CE(0,10+index*6,temp,DISP_CENTER);  
                  //DispStr_CE(0,10+index*4,"  ",DISP_CURRENT);  
              //  delay_and_wait_key(0,EXIT_KEY_F2,0);
             }
             Disp_Set_Magnification(1);
             
             EXT_ClearLine(36,0);
             Disp_Goto_XY(0,36);
             DispStr_CE(0,36,"��F1�˳���",DISP_CURRENT);
             DispStr_CE(0,36,"��F3������",DISP_RIGHT);
             
             key_value = delay_and_wait_key(0,EXIT_KEY_F1|EXIT_KEY_F3,0); 
             switch(key_value)
             {
             case EXIT_KEY_F1:         
             {
            
                flag = 0;//�˳�ѭ�� 
                
                break;
             }
             case EXIT_KEY_F3:
              {
                         break; 
               } 
                           
             }    
             
    }
    else
   {
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
void MainMenu()
{      
        //��ʼ���˵� 
        short ret;
        BROWINFO  main_menu;
        char MAIN_MENU[] = "1. ����Ѳ��2. GPRS�ϴ�3. ��ǩУ��4. ϵͳ����"; 
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
        while(1)
        {
           
           Disp_Clear();
           DispStr_CE(0,4,welcome,DISP_CENTER|DISP_CLRSCR);
           DispStr_CE(0,6,choose,DISP_CENTER);
            
           main_menu.dispLines = 4;
           main_menu.mInt = 4;
           main_menu.lineMax = 11;
           main_menu.sFont = 0;
           main_menu.numEnable = 1;
           main_menu.qEvent = EXIT_KEY_F1 ;
            
           Disp_Set_Magnification(2);
           ret = EXT_Brow_Select(&main_menu);
           Disp_Set_Magnification(1);
             
           switch(ret)
           {
                 case 0:
                 {
                
                    //Ѳ��
                   Query(); 
                   break;
                 }
                 case 1:
                 {
             
                  //�ύ���� 
                  SubmitData();
                   break;
                 }
                 case 2:
                 {        
                    
                      GetInfo();
                      
                      break; 
                 } 
                 case 3:
                  {  
                      SysSetMenu();
                  } 
                 default :
                 {   
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