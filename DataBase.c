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

char Menu[150*LINE_LEN+2];

static unsigned int pids[PAGE_SIZE];
static DataInfo datainfo;
static int PAGE_OFFSET=0;

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
		DispStr_CE(0,36,"��F2�˳���",DISP_POSITION);		
		delay_and_wait_key(0,EXIT_KEY_F2,0);
		return -1;
	} 

	RET = DB_format_db(0,BLOCKNUM,sizeof(DataInfo));   // 0 �ɹ� ����ʧ�� 
	if(RET){
		DispStr_CE(0,0,"DB_format_db",DISP_CENTER|DISP_CLRSCR);
		DispStr_CE(0,36,"��F2�˳���",DISP_POSITION);				
		delay_and_wait_key(0,EXIT_KEY_F2,0);
		return -1;
	}

	RET = DB_init_sys_param(0);         // 1     ���ݱ��� 
	if(RET!= 1){
		DispStr_CE(0,0,"DB_init_sys_param",DISP_CENTER|DISP_CLRSCR);
		DispStr_CE(0,36,"��F2�˳���",DISP_POSITION);				
		delay_and_wait_key(0,EXIT_KEY_F2,0);
		return -1;
	}

	RET = DB_check_format(0,BLOCKNUM,sizeof(DataInfo));// 0��ʽ��ȷ �������� 
	if(RET){
		return -1;
	}

	return 0;
}

short AddOneRecord(unsigned char* name,char* time,char* code,char* record){
	unsigned long ret = 0;
	
	datainfo.id = DB_count_records(0)+1;
	strcpy(datainfo.username,(char *)name);
	strcpy(datainfo.antifakecode,code);
	strcpy(datainfo.querytime,time);

	sprintf(record,"%d",datainfo.id);
	
	ret = DB_add_record(0,(void*)&datainfo);
	if(ret == 0){
		DispStr_CE(0,0,"�����¼ʧ��",DISP_CENTER|DISP_CLRSCR);
		DispStr_CE(0,36,"��F2�˳���",DISP_POSITION);				
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
	if(RET!=1){
		return -1;
	}

	RET = DB_check_format(0,BLOCKNUM,sizeof(DataInfo));
	if(RET ==0){
		return 0;
	}

	return RET;
}

DataInfo* dbRetrieve(char id[5]){
	char flag=0;
	char myid[5]={0};
	DataInfo di;
	DataInfo* pdi=NULL;	
	int i =0;	
	 
	int dbCount = DB_count_records(0); 
	for(i=0; i<dbCount;i++){
		pdi=DB_jump_to_record(0,i,&flag);
		if(flag==0){//�ǿռ�¼ && ���ڼ�¼
			memcpy(&di,pdi,sizeof(DataInfo));
			memset(myid,0,5);
			sprintf(myid,"%d",di.id); 
			if(strcmp(myid,id)==0){
				return pdi;
			}
		}
	}
	return NULL;
}

int dbDelete(unsigned int id){
	DataInfo* pdi=NULL;
	char flag = 0;
	int i =0;	
	int dbCount = DB_count_records(0);
	
	for(i=0; i<dbCount;i++){
		pdi=DB_jump_to_record(0,i,(char *)&flag);

		if(flag==0){//�ǿռ�¼ && ���ڼ�¼ 
			if(pdi->id == id){
				DB_delete_record(0,i);//ɾ���ü�¼
				return 0;
			}
		}
	}
	return -1;
}

int dbClean(){
	DataInfo* pdi=NULL;
	DataInfo di;
	char flag = 0;
	int i = 0;	
	int j = 0;
	int dbCount = DB_count_records(0);
	
	for(i=0; i<dbCount;i++){
		pdi=DB_jump_to_record(0,i,(char *)&flag);

		if(flag==0){//�ǿռ�¼ && ���ڼ�¼
			memcpy(&di,pdi,sizeof(DataInfo));
			for(j=0;j<PAGE_OFFSET;j++){
				if(di.id==pids[j]) DB_delete_record(0,i);//ɾ���ü�¼
			}
		}
	}
	//memset(pids,'\0',sizeof(unsigned int)*PAGE_SIZE);
	return 0;
}

short EncodeSendData(unsigned char* name ,unsigned char* passwd,unsigned char* senddata){  //��¼��װ Э�����ݸ�ʽ 
	int norecord=0;
	int i=0;
	int RET=-1 ;
	int dbCount=0; //���ݱ��м�¼����
	int rloop =0;
	int encodenum=0; //��װ��¼���� 
	
	DataInfo di;
	DataInfo* pdi = NULL;
	char flag = 0;	
	char tempsenddata[30];
	
	//������ݿ� 
	RET = CheckDB();
	if(RET != 0){
		return -1;
	}
	
	//���ݿ���� 
	dbCount = DB_count_records(0);//�����Ѿ�ʹ�õļ�¼��

	if(dbCount<1){ //û�м�¼
		return -1;
	}

	memset(tempsenddata,0,30);
	sprintf(tempsenddata,"*1;%s;%s;",name,passwd);
	strcat((char *)senddata,tempsenddata);

	memset(Menu,'\0',sizeof(Menu));
	while(rloop<dbCount){
		pdi=DB_jump_to_record(0,rloop,&flag);

		if(flag==0){ 
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

			pids[encodenum]= di.id;

			encodenum++;
			if(encodenum == PAGE_SIZE){//�ﵽ��������� 
				break;//����ѭ��
			}
		}else{//�ռ�¼��ɾ���ļ�¼
			norecord++;
		}//end if

		rloop++;
	}

	PAGE_OFFSET=encodenum;

	if(norecord==dbCount){ //û�м�¼
		return -1;
	}

	senddata[strlen((char *)senddata)-1]='#';
	senddata[strlen((char *)senddata)]='\n';

	return encodenum; 
}

short HandleRecvData(unsigned char* recvdata){
	char rets[5]={0};
	
	strncpy(rets,(char *)recvdata,2);

	if(strcmp(rets,"*0")==0){
		return 0;//��ȫ��ȷ
	}

	if(strcmp(rets,"*1")==0){	//�û������� �޸��û� 
		return 1; 
	}

	if(strcmp(rets,"*2") ==0) {	//������� 
		return 2;
	}

	if(strcmp(rets,"*3") ==0){	//�ϴ���¼�д���
		return 3;
	}

	return -1;
}

void UpdateDatabase(unsigned char* recvdata){
 	int i = 0;
	char temprecvdata[1024];
	char* token = NULL;

	memset(temprecvdata, '\0', sizeof(temprecvdata));
	strncpy(temprecvdata,(char *)recvdata,strlen((char *)recvdata));

	memset(Menu,'\0',sizeof(Menu));
	token= strtok(temprecvdata,";");//1 spit
	token = strtok(NULL,";");//2 spit

	while(token){
		char idarr[5]={0};
		char errcodearr[5]={0};
		char * ip=idarr;
		char * ep=errcodearr;
		char *p = token;

		int head=1;
		while(*p!=0 && *p!='#' && *p!='*'){
			if(head){
				if(*p!=',') {
					//printf("->%c\n",*p);
					*ip=*p;
					ip++;
				}else{
					head=0;
				}
			}else{
				//printf("<-%c\n",*p);
				*ep=*p;
				ep++;
			}
			p++;
		}

		if(head==0){
			DataInfo* pdi=dbRetrieve(idarr);
			DataInfo di;

			//ʧ�ܼ�¼  ��¼ʧ��ԭ��
			if(pdi!=NULL){
				memcpy(&di,pdi,sizeof(DataInfo));

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
				}else{  //����ԭ�� 
					sprintf(&Menu[(i+3)*28],"%s%d%s","����ԭ��",di.id,"����ԭ��");
					sprintf(&Menu[(i+4)*28],"%s","<------------------------->");
					// sprintf(temp_wronginfo,"%d%s%s%s%s", di.id,di.username,di.antifakecode,di.querytime,"����ԭ��");
				} 
				i=i+5;
			}
		}
		//������һ������
		token = strtok(NULL,";");
	}
	//�������ݿ�
	dbClean();
}
