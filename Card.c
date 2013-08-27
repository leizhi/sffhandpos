/****************************************************************************
*
*�ļ�(File):         Card.c
*
*�޸�(Modify):       2012-7-13 14:15:56
*
*����(Author):       sa
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
| V1.00  2012-7-13 14:15:56           sa 
----------------------------------------------------------------------------
****************************************************************************/
#include "API.h"

#include "Card.h"

unsigned char	ULATQ[3];
unsigned char	ULSNO[4];

unsigned char username[USERNAME_LEN];//Ѳ���û���
unsigned char password[USERNAME_LEN];//Ѳ������

char anticode[ANTIFAKECODE_LEN];//��α�� 

unsigned char sname[USERNAME_LEN];//��¼�û���
unsigned char spass[USERNAME_LEN];////��¼����

/**
*0
*!=0 error
*/
int  ReadNameAndPassword(unsigned char* nname,unsigned char* npass){

	long key_value = 0; 
	int forever = 1;
	
	unsigned char dummy_key[6] = "\xFF\xFF\xFF\xFF\xFF\xFF";

	unsigned char 	MATQ[3];	//ATQ(2 bytes)
	unsigned char 	MSNO[5];	//Serial number(4 bytes)
	unsigned char 	MATS[2];	//mifare1:0x08(1 byte)
	unsigned char databuf[30];
	memset(databuf,0,30); 

	
	while(forever){
		Disp_Clear();
		Disp_Goto_XY(0,36);
		DispStr_CE(0,36,"��F1�˳���",DISP_CURRENT);
		DispStr_CE(0,36,"��F3���ԡ�",DISP_RIGHT);

		DispStr_CE(2,2,"���ڳ�ʼ������ģ��...",DISP_POSITION);

		if(RCX_Init(CARD_TYPE_14443A)!=RCX_OK){//��ʼ��ʧ��

			EXT_ClearLine(2,0); 
			DispStr_CE(2,6,"ģ���ʼ��ʧ��",DISP_POSITION);
			DispStr_CE(2,10,"�������˳�",DISP_POSITION);

			key_value = delay_and_wait_key(30,EXIT_KEY_F1|EXIT_KEY_F3|EXIT_AUTO_QUIT,30);
			switch(key_value){
				case EXIT_KEY_F1 :
				case EXIT_AUTO_QUIT:{
					return -1;
				}
				case EXIT_KEY_F3:{
					break;
				}
			}
			continue;
		}

		//��ʼ���ɹ� 
		DispStr_CE(2,6,"��ʼ������ģ��ɹ�",DISP_POSITION); 
		DispStr_CE(2,10,"���ڶ���...",DISP_POSITION); 

		//����
		char RET = CardTypeARequest(PICC_REQALL,MATQ); //may be Halt
		if(RET!=RCX_OK){
			RET = CardTypeARequest(PICC_REQALL,MATQ);//maybe active
		}

		if(RET==RCX_OK){
			RET = CardTypeAAnticollLevel1(MSNO);
		}
		if(RET==RCX_OK){
			RET = CardTypeASelectLevel1(MSNO,MATS);
		}

		//����ʧ��
		if(RET!=RCX_OK){
			EXT_ClearLine(10,0); 
			DispStr_CE(2,12,"����ʧ��",DISP_POSITION);
			WarningBeep(2);

			key_value = delay_and_wait_key(30,EXIT_KEY_F1|EXIT_KEY_F3|EXIT_AUTO_QUIT,30);
			switch(key_value){
				case EXIT_KEY_F1 :{
					RCX_Close();
					return -1;
				}
				case EXIT_AUTO_QUIT:
				case EXIT_KEY_F3:{
					break;
				}
			}
			continue;
		}

		//�����ɹ�
		//���û���ʧ��
		RET=CardMFCAuthKey(PICC_AUTHENT1A,MSNO,dummy_key,5);
		if(RET==RCX_OK) RET = CardMFCRead16Bytes(5,nname);
		
		if(RET!=RCX_OK){
			EXT_ClearLine(10,0);
			WarningBeep(2);
			DispStr_CE(2,12,"���û���ʧ��",DISP_POSITION);

			key_value = delay_and_wait_key(30,EXIT_KEY_F1|EXIT_KEY_F3|EXIT_AUTO_QUIT,30);
			switch(key_value){
				case EXIT_KEY_F1 :
				case EXIT_AUTO_QUIT:{
					RCX_Close();
					return -3;//�˳�ѭ�� �ص����˵�
				}
				case EXIT_KEY_F3:{
					break;
				}
			}
			continue;
		}

		//��ȡ�û����ɹ� & ������ʧ��
		if((CardMFCAuthKey(PICC_AUTHENT1A,MSNO,dummy_key,6)!=RCX_OK)
			||(CardMFCRead16Bytes(6,npass)!=RCX_OK)){

			EXT_ClearLine(10,0); 
			DispStr_CE(2,12,"������ʧ��",DISP_POSITION);
			key_value = delay_and_wait_key(30,EXIT_KEY_F1|EXIT_KEY_F3|EXIT_AUTO_QUIT,30);
			switch(key_value){
				case EXIT_KEY_F1 :
				case EXIT_AUTO_QUIT:{
					RCX_Close();
					return -2;
				} 
				case EXIT_KEY_F3:{
					break;
				}
			}
			continue;
		}

		//��֤�ɹ�&&�˳�
		break;
	}//end while

	RCX_Close();
	return 0;
}

short LoginByCard(){
	int RET=-1;
	
	memset(username,0,USERNAME_LEN);//����û��� 
	memset(password,0,USERNAME_LEN);
	
	RET = ReadNameAndPassword(username, password);

	//��ȡ�ɹ� ��֤ 
	if(RET==0) return 0; 

	return -1;
}

short LoginToSubmit(){
	int RET=-1;
	
	memset(sname,0,USERNAME_LEN);
	memset(spass,0,USERNAME_LEN);

	RET = ReadNameAndPassword(sname, spass);
	//��ȡ�ɹ� ��֤ 
	if(RET==0) return 0; 

	return -1;
} 

short ultralight_init(){
	short err = 0;

	err = RCX_Init(CARD_TYPE_14443A);
	if(err == RCX_OK)
	{
		err = 1;
	}
	else
	{
		err = 0;
	}
	return  err; 
}

void ultralight_close(){
  RCX_Close();
}

short ultralight_find_card(){
	short ret = 0; 
	short err = 0;
	unsigned char	sak = 0;
	
	// ��ʼѰ�� 
	ret = CardTypeARequest(PICC_REQALL,ULATQ); 
	if( (ret)||(ULATQ[0]!=0x44 )||(ULATQ[1]!=0x00 ) )
	{		
		err = 0;
		return err;
	} 
	ret = CardTypeAAnticoll(PICC_ANTICOLL1,ULSNO);
	ret |= CardTypeASelect(PICC_ANTICOLL1,ULSNO,&sak);		
	if(sak&0x04)
	{		
		ret |= CardTypeAAnticoll(PICC_ANTICOLL2,ULSNO);	
		ret |= CardTypeASelect(PICC_ANTICOLL2,ULSNO,&sak);			
		if(sak&0x04)
		{		
			ret |= CardTypeAAnticoll(PICC_ANTICOLL3,ULSNO);				
			ret |= CardTypeASelect(PICC_ANTICOLL3,ULSNO,&sak);			
		}
	}
	else
	{
   err = 0;
   return err;
	}
	if( ret ) 
  {
    err = 0;
    return err;
  }
	err =1;
	return err;
}

short ultralight_read_card(unsigned char block_add,unsigned char* databuf)
{
  short err = 0;
  err = CardMFCRead16Bytes(block_add,databuf);
  if( err != RCX_OK)
  {
     ultralight_close();
     err = 0;        
  }
  else
  {
    ultralight_close();
    err = 1;
  }
  return err;
}

short ultralight_local_read_card(char* ac){
	short flag = 0;
	short init_err = 0;
	short find_card_err = 0;
	short init_flag = 0; 
	short read_card_err = 0;
	long  key_value = 0; 
	unsigned char block_add= 0;
	unsigned char databuf[30];
	
	memset(databuf,0,30);
	block_add = 4;

	flag = 1;
	init_flag =0;
	while(flag)
	{
		Disp_Clear();
		Disp_Goto_XY(0,36);
		DispStr_CE(0,36,"��F1�˳���",DISP_CURRENT);
		DispStr_CE(0,36,"��F3���ԡ�",DISP_RIGHT);
         
		if(init_flag == 0)
		{
			DispStr_CE(0,2,"���ڳ�ʼ������ģ��UL...",DISP_CENTER); 
			init_err = ultralight_init();
			if(0 == init_err )  //��ʼ��ʧ�� 
			{
				EXT_ClearLine(2,0); 
				DispStr_CE(0,4,"ģ���ʼ��ʧ��",DISP_CENTER);
				ultralight_close();
				DispStr_CE(0,6,"�������˳�",DISP_CENTER);       
				key_value = delay_and_wait_key(30,EXIT_KEY_F1|EXIT_KEY_F3|EXIT_AUTO_QUIT,30);
        switch(key_value)
        {
					case EXIT_KEY_F1 :               
					case EXIT_AUTO_QUIT:
					{
						init_flag =0;
						flag = 0;     //�˳�ѭ�� �ص����˵� 
						break;
					}
					case EXIT_KEY_F3:
					{
						init_flag =1;
						break; 
					}  
        } 
			}
			else//��ʼ���ɹ� 
			{
				DispStr_CE(0,4,"��ʼ������ģ��ɹ�",DISP_CENTER); 
				DispStr_CE(0,6,"����Ѱ��...",DISP_CENTER); 
				find_card_err = ultralight_find_card();
				if(0 == find_card_err)//Ѱ��ʧ��
				{
					EXT_ClearLine(8,0); 
					DispStr_CE(0,8,"�޿�����˶ԣ�",DISP_CENTER);
					WarningBeep(2);
					key_value = delay_and_wait_key(30,EXIT_KEY_F1|EXIT_KEY_F3|EXIT_AUTO_QUIT,30);
					switch(key_value)
					{
						case EXIT_KEY_F1 :
						case EXIT_AUTO_QUIT:
						{
							flag =0;     //�˳�ѭ�� �ص����˵� 
							break;
						} 
						case EXIT_KEY_F3:
						{
							break;
						}
					}       
				}
				else//Ѱ���ɹ� ׼����ȡ�� 
				{
	        EXT_ClearLine(8,0); 
	        DispStr_CE(0,8,"Ѱ���ɹ�",DISP_CENTER); 
	        DispStr_CE(0,10,"���ڶ���...",DISP_CENTER); 
	        read_card_err = ultralight_read_card(block_add, databuf);
	        if( 0 == read_card_err)     //����ʧ�� 
	        {
						EXT_ClearLine(10,0); 
						DispStr_CE(0,10,"����ʧ��",DISP_CENTER);
						WarningBeep(2);
						key_value = delay_and_wait_key(30,EXIT_KEY_F1|EXIT_KEY_F3|EXIT_AUTO_QUIT,30);
						switch(key_value)
						{
							case EXIT_KEY_F1 :
							case EXIT_AUTO_QUIT:
							{
								flag =0;     //�˳�ѭ�� �ص����˵� 
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
						/* EXT_ClearLine(12,0); 
						DispStr_CE(0,12,"�����ɹ�",DISP_CENTER);
						EXT_ClearLine(16,0);
						DispStr_CE(0,16,"��α��Ϊ��",DISP_CENTER);
						DispStr_CE(0,18,databuf,DISP_CENTER);
						*/
            strcpy(ac,(char *)&databuf);
            
            WarningBeep(0);
            return  0; 
            
						EXT_ClearLine(36,0);
						Disp_Goto_XY(0,36);
						DispStr_CE(0,36,"��F1�˳���",DISP_CURRENT);
						DispStr_CE(0,36,"��F3������",DISP_RIGHT);
						key_value = delay_and_wait_key(30,EXIT_KEY_F1|EXIT_KEY_F3|EXIT_AUTO_QUIT,30);
						switch(key_value)
						{
							case EXIT_KEY_F1:
							case EXIT_AUTO_QUIT:
							{
								flag = 0;//�˳�ѭ�� 
								return -1; 
								break;
							}
							case EXIT_KEY_F3:
							{
								break; 
							} 
						} 
					}
				} 
			}
		}
	} 
	return 0;
}
