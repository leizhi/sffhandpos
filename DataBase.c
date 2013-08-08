/****************************************************************************
*
*�ļ�(File):         DataBase.c
*
*�޸�(Modify):       2012-7-13 14:23:58
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
| V1.00  2012-7-13 14:23:58           sa 
----------------------------------------------------------------------------
****************************************************************************/
#include "DataBase.h"
#include "Common.h"
#include "Card.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "api.h"

extern  short Display(short flag);

unsigned int flagarr[PAGENUM];

//======================================================================
//��������FindDatabase 
//����  ���������ݿ� 
//����  ���� 
//����ֵ��short -1 ��ʾ��ʼ��ַʧ�ܣ�������ʾ���ݱ�ĸ��� 
//======================================================================
int FindDatabase(){
	int RET=-1;
	RET = DB_init_sys_param(0);

	if(RET != 1){
		return -1;
	}

	RET = DB_check_format(0,BLOCKNUM,sizeof(DataInfo));
	if(RET ==0){
		return 0;
	}

	return -1;
}
//======================================================================
//��������CreateDatabase 
//����  ���������ݿ� 
//����  ���� 
//����ֵ��short 
//======================================================================
short CreateDatabase(){
	int RET=-1;

	RET = DB_erase_db_sys();   //0 �ɹ� ����flashʧ��
	if(RET){
		DispStr_CE(0,0,"DB_erase_db_sys",DISP_CENTER|DISP_CLRSCR);
		delay_and_wait_key(0,EXIT_KEY_F2,0);
		return -1;
	} 

	RET = DB_format_db(0,BLOCKNUM,sizeof(DataInfo));   //0 �ɹ� ����ʧ�� 
	if(RET){
		DispStr_CE(0,0,"DB_format_db",DISP_CENTER|DISP_CLRSCR);
		delay_and_wait_key(0,EXIT_KEY_F2,0);
		return -1;
	}

	RET = DB_init_sys_param(0);         //1 ���ݱ��� 
	if(RET!= 1){
		DispStr_CE(0,0,"DB_init_sys_param",DISP_CENTER|DISP_CLRSCR);
		delay_and_wait_key(0,EXIT_KEY_F2,0);
		return -1;
	}

	RET = DB_check_format(0,BLOCKNUM,sizeof(DataInfo));//0��ʽ��ȷ �������� 
	if(RET){
		return -1;
	}

	return 0;
}

short AddOneRecord(unsigned char* name,char* time,char* code,char* record){
	datainfo.id = DB_count_records(0)+1;
	strcpy(datainfo.username,(char *)name);
	strcpy(datainfo.antifakecode,code);
	strcpy(datainfo.querytime,time);

	sprintf(record,"%d",datainfo.id);

	unsigned long ret;
	ret = DB_add_record(0,(void*)&datainfo);
	if(ret == 0){
		DispStr_CE(0,0,"�����¼ʧ��",DISP_CENTER|DISP_CLRSCR);
		delay_and_wait_key(0,EXIT_KEY_F2,0);
		return -1;
	}else{
		// DispStr_CE(0,0,"�����¼�ɹ�",DISP_CENTER);
		return 0;
	}
}

short  CheckDB(){//������ݿ��Ƿ���� 0 ��ʾ��ã�-1 ��ʾʧ�� 
	int RET = -1;

	RET=DB_init_sys_param(0);
	if(RET!=1)
		return -1;

	RET = DB_check_format(0,BLOCKNUM,sizeof(DataInfo));
	if(RET ==0){
		return 0;
	}

	return -1;
}

short EncodeSendData(unsigned char* name ,unsigned char* passwd,unsigned char* senddata){  //������װ��¼ 
	short ret ;
	char flag; 
	DataInfo* pdi;
	DataInfo di;
	int recordnum; //���ݱ��м�¼���� 
	int temp =0;
	int encodenum=0; //��װ��¼���� 
	//������ݿ� 
	ret = CheckDB();
	if(ret == 0){  //���ݿ���� 
		recordnum =0;
		recordnum = DB_count_records(0);//�����Ѿ�ʹ�õļ�¼��
		char tempsenddata[30];
		memset(tempsenddata,0,30);

		sprintf(tempsenddata,"*1;%s;%s;",name,passwd);
		strcat((char *)senddata,tempsenddata);

		int norecord=0;

		int i=0;
		memset(Menu,'\0',150*PAGENUM+2);

		while(temp<recordnum){
			pdi=DB_jump_to_record(0,temp,&flag);

			if((flag ==1)||(flag == 2)){//�ռ�¼��ɾ���ļ�¼ 
				temp++;
				norecord++;
			}else{
				memcpy(&di,pdi,sizeof(DataInfo));

				sprintf(&Menu[i*28],"%s%s","Ѳ �� �ˣ�",di.username); 
				sprintf(&Menu[(i+1)*28],"%s%s","�� α �룺",di.antifakecode);
				sprintf(&Menu[(i+2)*28],"%s%s","Ѳ��ʱ�䣺",di.querytime);
				sprintf(&Menu[(i+3)*28],"%s%d","��¼������",di.id);
				sprintf(&Menu[(i+4)*28],"%s","<------------------------->");

				i=i+5;

				char tempid[10];
				memset(tempid,0,10);
				sprintf(tempid,"%d",di.id);

				strcat((char *)senddata,tempid);
				strcat((char *)senddata,",");

				filter(di.antifakecode,';');
				filter(di.antifakecode,',');
				strcat((char *)senddata,di.antifakecode);
				strcat((char *)senddata,",");

				strcat((char *)senddata,di.username);
				strcat((char *)senddata,",");

				strcat((char *)senddata,di.querytime);
				strcat((char *)senddata,"/");
				temp++;

				flagarr[encodenum]= di.id;

				encodenum++;
				if(encodenum == PAGENUM){//�ﵽ��������� 
					break;//����ѭ�� 
				}
			}
		}

		senddata[strlen((char *)senddata)-1]='#';
		senddata[strlen((char *)senddata)]='\n';

		if(norecord == recordnum){ //û�м�¼ 
			return -1; 
		}
	}
	return 0; 
}

short HandleRecvData(unsigned char* recvdata){
	char wronginfo[3000];
	char right[5];
	char wrong[5];
	memset(right,0,5);
	memset(wrong,0,5);
	strncpy(right,(char *)recvdata,2);
	strncpy(wrong,(char *)recvdata,2);

	if(strcmp(right,"*0")==0){
		//���´������ݿ� 
		//��������
		DataInfo di;
		DataInfo* pdi;
		int recordnum; 
		recordnum = DB_count_records(0);
		int rloop =0;
		char flag; 
		while(rloop<recordnum){
			pdi=DB_jump_to_record(0,rloop,(char *)&flag);
			if((flag ==1)||(flag == 2)){    //�ռ�¼����ɾ����¼ 
				rloop++;
			}else{
				memcpy(&di,pdi,sizeof(DataInfo));
				int j;
				for(j=0;j<PAGENUM;j++){   
					if(di.id == flagarr[j]){
						DB_delete_record(0,rloop);//ɾ���ü�¼ 
						break; 
					}
				} 
				rloop ++;
			}
		} 
		return 0;//��ȫ��ȷ 
	}else  if(strcmp(wrong,"*1")==0){   //�û������� �޸��û� 
		return 1; 
	}else if(strcmp(wrong,"*2") ==0) {  //������� 
		return 2;
	}else{	//�ϴ���¼�д��� 
		//�������ݿ�
		UpdateDatabase(recvdata,wronginfo) ;
		Disp_Clear();
		WarningBeep(2); 
		Display(1);
		WarningBeep(0); 
		DispStr_CE(0,4,"��ϲ�����������ύ���",DISP_CENTER|DISP_CLRSCR);
		EXT_ClearLine(36,0);
		Disp_Goto_XY(0,36);
		DispStr_CE(0,36,"��F1�˳���",DISP_CURRENT);
		DispStr_CE(0,36,"��F3�ϴ���һ����",DISP_RIGHT);

		long key_value; 
		key_value = delay_and_wait_key(0,EXIT_KEY_F1|EXIT_KEY_F3,0);

		switch(key_value){
			case EXIT_KEY_F1:{	//�˳� 
				return 3;
				break;
			}case EXIT_KEY_F3:{
				return 4;	//���� 
				break;
			}default:{
				WarningBeep(1);
				break;
			}
		}
	}
	return 0;
}

void UpdateDatabase(unsigned char* recvdata,char* wronginfo){
	int recordnum;
	memset(wronginfo,0,3000); 
	int rloop =0;
	DataInfo di;
	DataInfo* pdi;
	char flag; 
	char temprecvdata[200];
	recordnum = DB_count_records(0);//�����Ѿ�ʹ�õļ�¼��

	memset(temprecvdata,0,200);  
	strncpy(temprecvdata,(char *)recvdata,strlen((char *)recvdata));

	int i=0;
	memset(Menu,'\0',150*28+2);

	while(rloop<recordnum) {
		strncpy(temprecvdata,(char *)recvdata,strlen((char *)recvdata));
		pdi=DB_jump_to_record(0,rloop,&flag);
		if((flag ==1)||(flag == 2)){//�ռ�¼����ɾ����¼ 
			rloop++;
		}else{
			memcpy(&di,pdi,sizeof(DataInfo));
			char* token = NULL;
			token= strtok(temprecvdata,";");
			token = strtok(NULL,";");

			while(strcmp(token,"#")){
				int len = strlen(token);
				int j=0;
				char idarr[5];
				char errcodearr[5];
				memset(idarr,0,5);
				memset(errcodearr,0,5);
				int sign=0;
				rloop=0;
				for(;j<len;j++)	{
					if((token[j]!=',')&&(sign ==0)){
						rloop++;
						idarr[j] = token[j];
					}else  if((token[j] == ',')&&(sign ==0)){
						sign =1;
						rloop++;
						continue;
					}else   if((token[j] != ',')&&(sign ==1)){
						errcodearr[j-rloop]=token[j];
					}
				}

				char myid[5];
				memset(myid,0,5);
				sprintf(myid,"%d",di.id); 

				if(strcmp(myid,idarr)==0){  //ʧ�ܼ�¼  ��¼ʧ��ԭ��  
					char temp_wronginfo[100];
					memset(temp_wronginfo,0,100);

					sprintf(&Menu[i*28],"%s%s","Ѳ �� �ˣ�",di.username); 
					sprintf(&Menu[(i+1)*28],"%s%s","�� α �룺",di.antifakecode);
					sprintf(&Menu[(i+2)*28],"%s%s","Ѳ��ʱ�䣺",di.querytime);

					if(strcmp(errcodearr,"1")==0){    //�������� 
						sprintf(&Menu[(i+3)*28],"%s%d%s","����ԭ��",di.id,"��̨�޸ÿ���Ϣ");
						sprintf(&Menu[(i+4)*28],"%s","<------------------------->");
						// sprintf(temp_wronginfo,"%d%s%s%s%s", di.id,di.username,di.antifakecode,di.querytime,"��̨�޸ÿ���Ϣ");
					}else if(strcmp(errcodearr,"2")==0){//��Ϊ������
						sprintf(&Menu[(i+3)*28],"%s%d%s","����ԭ��",di.id,"�ÿ�δ������");
						sprintf(&Menu[(i+4)*28],"%s","<------------------------->");
						// sprintf(temp_wronginfo,"%d%s%s%s%s", di.id,di.username,di.antifakecode,di.querytime,"�ÿ�δ������");
					}else  if(strcmp(errcodearr,"3")==0){  //����ԭ�� 
						sprintf(&Menu[(i+3)*28],"%s%d%s","����ԭ��",di.id,"����ԭ��");
						sprintf(&Menu[(i+4)*28],"%s","<------------------------->");
						// sprintf(temp_wronginfo,"%d%s%s%s%s", di.id,di.username,di.antifakecode,di.querytime,"����ԭ��");
					} 
					i=i+5; 

					strcat(wronginfo,temp_wronginfo);
					DB_delete_record(0,di.id-1);//ɾ���ü�¼ 
					break; 
				}else {  //������һ������ 
					token = strtok(NULL,";") ;
					continue;
				}
			}
			if(strcmp(token,"#")==0){  //recvdata��û�иü�¼ ���¼�ϴ��ɹ� 
				int kk =0;
				for(;kk<PAGENUM;kk++){
					if(di.id == flagarr[kk]){ 
						DB_delete_record(0,rloop);//ɾ���ü�¼ 
					}
				}
			}
			memset(temprecvdata,0,200);
			rloop++;
		}
	}
}
