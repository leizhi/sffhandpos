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

unsigned int flagarr[PAGENUM];
//======================================================================
//��������FindDatabase 
//����  ���������ݿ� 
//����  ���� 
//����ֵ��short -1 ��ʾ��ʼ��ַʧ�ܣ�������ʾ���ݱ�ĸ��� 
//======================================================================
short FindDatabase()
{
     short err;
     err = DB_init_sys_param(0);   
     if(err == 1)
     {
         err = DB_check_format(0,BLOCKNUM,sizeof(DataInfo));
         if(err ==0)
         {
            return 0;
         }
         else
         {
          return -1;
         }
     }
     else
     {
        return -1; 
     }
}
//======================================================================
//��������CreateDatabase 
//����  ���������ݿ� 
//����  ���� 
//����ֵ��short 
//======================================================================
short CreateDatabase()
{
   short ret;
   ret = DB_erase_db_sys();   //0 �ɹ� ����flashʧ��
   if(ret)
   {
     	DispStr_CE(0,0,"DB_erase_db_sys",DISP_CENTER|DISP_CLRSCR);
 	delay_and_wait_key(0,EXIT_KEY_F2,0);
      goto err_end;
   } 
   ret = DB_format_db(0,BLOCKNUM,sizeof(DataInfo));   //0 �ɹ� ����ʧ�� 
   if(ret)
   {
   DispStr_CE(0,0,"DB_format_db",DISP_CENTER|DISP_CLRSCR);
 	delay_and_wait_key(0,EXIT_KEY_F2,0);
    goto err_end;
   } 
   
   ret = DB_init_sys_param(0);         //1 ���ݱ��� 
   if(ret!= 1)
   {
   DispStr_CE(0,0,"DB_init_sys_param",DISP_CENTER|DISP_CLRSCR);
 	delay_and_wait_key(0,EXIT_KEY_F2,0);
            goto err_end;
   }
   
   ret = DB_check_format(0,BLOCKNUM,sizeof(DataInfo));     //0��ʽ��ȷ �������� 
   if(ret)
   {
    /*DispStr_CE(0,0,"DB_check_format",DISP_CENTER|DISP_CLRSCR);
    char kk[5];
    memset(kk,0,5);
    sprintf(kk,"ret=%d",ret);
        DispStr_CE(0,2,kk,DISP_CENTER);
 	delay_and_wait_key(0,EXIT_KEY_F2,0);  */ 
     goto err_end;
   }
   
    	/*DispStr_CE(0,0,"���ݿ��ʼ���ɹ���",DISP_CENTER|DISP_CLRSCR);
    	WarningBeep(1); 
    	delay_and_wait_key(0,EXIT_KEY_F2,0); 
    	*/
    	return 0;
    	
   err_end:
 	/*DispStr_CE(0,0,"���ݿ��ʼ��ʧ�ܣ�",DISP_CENTER|DISP_CLRSCR);
 	    	WarningBeep(2);
 	delay_and_wait_key(0,EXIT_KEY_F2,0);
 	*/ 
 	return -1; 
}



short AddOneRecord(char* name,char* time,char* code,char* record){
	datainfo.id = DB_count_records(0)+1;
	strcpy(datainfo.username,name);
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

short  CheckDB()   //������ݿ��Ƿ���� 0 ��ʾ��ã�-1 ��ʾʧ�� 
{
  int pp;
   if(DB_init_sys_param(0)==1)
   {
        //DispStr_CE(0,10,"init_sys_param ",DISP_CENTER|DISP_CLRSCR);
       //delay_and_wait_key(0,EXIT_KEY_F2,0);
      pp = DB_check_format(0,BLOCKNUM,sizeof(DataInfo));
       if(pp ==0)
       {
           // DispStr_CE(0,10,"checkformat ",DISP_CENTER|DISP_CLRSCR);
          // delay_and_wait_key(0,EXIT_KEY_F2,0);
         return 0;
       }
      else
      {
             return -1;
      }
   }
   else
   {
     return -1;
   }
   
} 

short EncodeSendData(char* name ,char* passwd,char* senddata){  //������װ��¼ 
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
		strcat(senddata,tempsenddata);

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

				strcat(senddata,tempid);
				strcat(senddata,",");

				strcat(senddata,di.antifakecode);
				strcat(senddata,",");

				strcat(senddata,di.username);
				strcat(senddata,",");

				strcat(senddata,di.querytime);
				strcat(senddata,"/");
				temp++;

				flagarr[encodenum]= di.id;

				encodenum++;
				if(encodenum == PAGENUM){//�ﵽ��������� 
					break;//����ѭ�� 
				}
			}
		}

		senddata[strlen(senddata)-1]='#';
		senddata[strlen(senddata)]='\n';

		if(norecord == recordnum){ //û�м�¼ 
			return -1; 
		}
	}
	return 0; 
}

/*
short EncodeSendData(char* name ,char* passwd,char* senddata)
{
   short ret ;
   char flag; 
   DataInfo* pdi;
   DataInfo di;
   int recordnum;
   int temp =0;
   int encodenum=0; 
   if(DB_init_sys_param(0)==1)
   {
      //DispStr_CE(0,10,"init_sys_param ",DISP_CENTER|DISP_CLRSCR);
     //delay_and_wait_key(0,EXIT_KEY_F2,0);
   }
   int pp = DB_check_format(0,BLOCKNUM,sizeof(DataInfo));
   if(pp ==0)
   {
        // DispStr_CE(0,10,"checkformat ",DISP_CENTER|DISP_CLRSCR);
    // delay_and_wait_key(0,EXIT_KEY_F2,0);
   }
   else
   {
         char temppp[9];
          memset(temppp,0,9);
          sprintf(temppp,"pp=%d",pp);
          DispStr_CE(0,10,temppp,DISP_CENTER|DISP_CLRSCR);
         delay_and_wait_key(0,EXIT_KEY_F2,0);
   }
   
   recordnum = DB_count_records(0);//�����Ѿ�ʹ�õļ�¼��
   char tempsenddata[30];
   memset(tempsenddata,0,30);
  
   sprintf(tempsenddata,"*1;%s;%s;",name,passwd);
   strcat(senddata,tempsenddata);
   int norecord=0;
   while(temp<recordnum)
   {
       pdi=DB_jump_to_record(0,temp,&flag);
       if((flag ==1)||(flag == 2))
       {
          temp++;
          norecord++;
       }
       else
       {
          
            memcpy(&di,pdi,sizeof(DataInfo));
            char tempid[10];
            memset(tempid,0,10);
            sprintf(tempid,"%d",di.id);
            strcat(senddata,tempid);
            strcat(senddata,",");
         
            strcat(senddata,di.antifakecode);
            strcat(senddata,",");
            strcat(senddata,di.username);
            strcat(senddata,",");
            strcat(senddata,di.querytime);
            strcat(senddata,"/");              
           temp++;
       }
        
   }
   senddata[strlen(senddata)-1]='#';
   senddata[strlen(senddata)]='\n';
   if(norecord == recordnum) //û�м�¼ 
   {
     return 1; 
   }
   DispStr_CE(0,0,"�����ύ�������£�",DISP_CENTER|DISP_CLRSCR);
    DispStr_CE(0,2,"��������ύ",DISP_CENTER);
   EXT_Display_Multi_Lines(senddata,4,34);
  
   delay_and_wait_key(0,EXIT_KEY_ALL,0);
            
            return 0;
 
}
 */
short HandleRecvData(char* recvdata)
{
       char wronginfo[3000];
       char right[5];
       char wrong[5];
       memset(right,0,5);
       memset(wrong,0,5);       
       strncpy(right,recvdata,2);
       strncpy(wrong,recvdata,2);
       
       if(strcmp(right,"*0")==0)
       {
          //���´������ݿ� 
          //DispStr_CE(0,10,"��ȫ��ȷ",DISP_CENTER|DISP_CLRSCR);
        // delay_and_wait_key(0,EXIT_KEY_F2,0);
          
          //��������
         DataInfo di;
         DataInfo* pdi;
         int recordnum; 
         recordnum = DB_count_records(0);
         int index =0;
         int flag; 
       while(index<recordnum)
       {
          
         pdi=DB_jump_to_record(0,index,&flag);
         if((flag ==1)||(flag == 2))    //�ռ�¼����ɾ����¼ 
         {
             index++;
         }
         else
         {
            
            memcpy(&di,pdi,sizeof(DataInfo));
            int j;
            for(j=0;j<PAGENUM;j++)
            {   
                if(di.id == flagarr[j])
                {
                    DB_delete_record(0,index);//ɾ���ü�¼ 
                  break; 
                }
            } 
            index ++;
         }
       } 
         /* int index=0; 
          int ret; 
     while(1)
     { 
        ret = CreateDatabase();
        if(ret!= 0)
        {
         index++;
         if(index ==4)
         { 
            break; 
         } 
        }
        else
        {
          break; 
        } 
     } 
      */
          return 0;//��ȫ��ȷ 
       }
       else  if(strcmp(wrong,"*1")==0)   //�û������� �޸��û� 
       {
             // DispStr_CE(0,0,"�û���������",DISP_CENTER|DISP_CLRSCR); 
             // delay_and_wait_key(0,EXIT_KEY_F2,0);
              return 1; 
       }
       else if(strcmp(wrong,"*2") ==0)  //������� 
       {
            //DispStr_CE(0,0,"�û���������",DISP_CENTER|DISP_CLRSCR); 
             // delay_and_wait_key(0,EXIT_KEY_F2,0);
              return 2;
       } 
       else   //�ϴ���¼�д��� 
       {
           //�������ݿ�            
            /* DispStr_CE(0,0,"�������ݿ�",DISP_CENTER|DISP_CLRSCR);
             delay_and_wait_key(0,EXIT_KEY_F2,0);
             */
             
            UpdateDatabase(recvdata,wronginfo) ;
            
             Disp_Clear();
             
              //DispStr_CE(0,0,"������Ϣ",DISP_CENTER|DISP_CLRSCR);
             
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
              
              switch(key_value)
              {
                  case EXIT_KEY_F1:   //�˳� 
                  {
                     return 3;
                     break;
                  }
                 case EXIT_KEY_F3:
                 {
                     return 4;   //���� 
                    break;
                 }
                 default:
                 {
                    WarningBeep(1);
                    break;
                 }
              }
       
           
       }
} 

void UpdateDatabase(char* recvdata,char* wronginfo){
	int recordnum;
	memset(wronginfo,0,3000); 
	int index =0;
	DataInfo di;
	DataInfo* pdi;
	char flag; 
	char temprecvdata[200];
	char temprecvdata1[200];

	recordnum = DB_count_records(0);//�����Ѿ�ʹ�õļ�¼��

	/* DispStr_CE(0,8,"��ʼ�������ݿ�",DISP_CENTER|DISP_CLRSCR);
	DispStr_CE(0,10,recvdata,DISP_CENTER);
	delay_and_wait_key(0,EXIT_KEY_F2,0);
	*/
	memset(temprecvdata,0,200);  
	strncpy(temprecvdata,recvdata,strlen(recvdata));

	int i=0;
	memset(Menu,'\0',150*28+2);

	while(index<recordnum) {
	strncpy(temprecvdata,recvdata,strlen(recvdata));
	pdi=DB_jump_to_record(0,index,&flag);
	if((flag ==1)||(flag == 2)){//�ռ�¼����ɾ����¼ 
		index++;
	}else{
		memcpy(&di,pdi,sizeof(DataInfo));


		// DispStr_CE(0,0,"Update recvdata",DISP_CENTER|DISP_CLRSCR);
		// DispStr_CE(0,2,temprecvdata,DISP_CENTER);

		//delay_and_wait_key(0,EXIT_KEY_F2,0);

		int k=0;

		/* DispStr_CE(0,4,temprecvdata,DISP_CENTER|DISP_CLRSCR);
		DispStr_CE(0,2,"TEMPRECVDATA",DISP_CENTER);
		delay_and_wait_key(0,EXIT_KEY_F2,0);
		*/
		char* token = NULL;
		token= strtok(temprecvdata,";");
		token = strtok(NULL,";");

		/* DispStr_CE(0,2,"token before while",DISP_CENTER|DISP_CLRSCR);
		DispStr_CE(0,4,token,DISP_CENTER);
		delay_and_wait_key(0,EXIT_KEY_F2,0);
		*/

		while(strcmp(token,"#")){
			int len = strlen(token);
			int j=0;
			char idarr[5];
			char errcodearr[5];
			memset(idarr,0,5);
			memset(errcodearr,0,5);
			int flag=0;
			int index=0;
			for(;j<len;j++)	{
				if((token[j]!=',')&&(flag ==0)){
					index++;
					idarr[j] = token[j];
				}else  if((token[j] == ',')&&(flag ==0)){
					flag =1;
					index++;
					continue;
				}else   if((token[j] != ',')&&(flag ==1)){
					errcodearr[j-index]=token[j];
				}
			}
			/* DispStr_CE(0,0,"id+ err",DISP_CENTER|DISP_CLRSCR);
			DispStr_CE(0,2,idarr,DISP_CENTER);
			DispStr_CE(0,4,errcodearr,DISP_CENTER);
			delay_and_wait_key(0,EXIT_KEY_F2,0);
			*/


			char myid[5];
			memset(myid,0,5);
			sprintf(myid,"%d",di.id); 

			/*DispStr_CE(0,0,myid,DISP_CENTER|DISP_CLRSCR);
			DispStr_CE(0,2,"toke",DISP_CENTER);
			DispStr_CE(0,4,token,DISP_CENTER);
			delay_and_wait_key(0,EXIT_KEY_F2,0);
			*/

                  if(strcmp(myid,idarr)==0)  //ʧ�ܼ�¼  ��¼ʧ��ԭ��  
                  {
                     
                      char temp_wronginfo[100];
                      memset(temp_wronginfo,0,100);
                      
                       sprintf(&Menu[i*28],"%s%s","Ѳ �� �ˣ�",di.username); 
                       sprintf(&Menu[(i+1)*28],"%s%s","�� α �룺",di.antifakecode);
                       sprintf(&Menu[(i+2)*28],"%s%s","Ѳ��ʱ�䣺",di.querytime);

                      if(strcmp(errcodearr,"1")==0)    //�������� 
                      {
                             sprintf(&Menu[(i+3)*28],"%s%d%s","����ԭ��",di.id,"��̨�޸ÿ���Ϣ");
                                 sprintf(&Menu[(i+4)*28],"%s","<------------------------->");
                         // sprintf(temp_wronginfo,"%d%s%s%s%s", di.id,di.username,di.antifakecode,di.querytime,"��̨�޸ÿ���Ϣ");
                      }
                      else if(strcmp(errcodearr,"2")==0)//��Ϊ������
                      {
                             sprintf(&Menu[(i+3)*28],"%s%d%s","����ԭ��",di.id,"�ÿ�δ������");
                                 sprintf(&Menu[(i+4)*28],"%s","<------------------------->");
                            // sprintf(temp_wronginfo,"%d%s%s%s%s", di.id,di.username,di.antifakecode,di.querytime,"�ÿ�δ������");
                      }
                      else  if(strcmp(errcodearr,"3")==0)  //����ԭ�� 
                      {
                                  sprintf(&Menu[(i+3)*28],"%s%d%s","����ԭ��",di.id,"����ԭ��");
                                 sprintf(&Menu[(i+4)*28],"%s","<------------------------->");
                              // sprintf(temp_wronginfo,"%d%s%s%s%s", di.id,di.username,di.antifakecode,di.querytime,"����ԭ��");
                      } 
                       i=i+5; 
                       
                      strcat(wronginfo,temp_wronginfo);
                     
                            /*DispStr_CE(0,4,"ɾ�������¼",DISP_CENTER|DISP_CLRSCR);
                      delay_and_wait_key(0,EXIT_KEY_F2,0);
                      */
                      DB_delete_record(0,di.id-1);//ɾ���ü�¼ 
                      
                      break; 
                    
                  }
                  else   //������һ������ 
                  {
                       token = strtok(NULL,";") ;
                       continue;
                  }
                 
            }
            if(strcmp(token,"#")==0)  //recvdata��û�иü�¼ ���¼�ϴ��ɹ� 
            {
                /* DispStr_CE(0,4,"TOKEN == #",DISP_CENTER|DISP_CLRSCR);
                 delay_and_wait_key(0,EXIT_KEY_F2,0);
                 */
                 
                 
                      int kk =0;
                      for(;kk<PAGENUM;kk++)
                      {
                       
                        if(di.id == flagarr[kk])
                        {       /*DispStr_CE(0,4,"ɾ��������¼",DISP_CENTER|DISP_CLRSCR);
                          delay_and_wait_key(0,EXIT_KEY_F2,0);
                          */
                          DB_delete_record(0,index);//ɾ���ü�¼ 
                         }
                      }
            }
          
             
              
            
            
             /*DispStr_CE(0,4,"out while",DISP_CENTER|DISP_CLRSCR);
              delay_and_wait_key(0,EXIT_KEY_F2,0);
              */
              memset(temprecvdata,0,200);
           
          index++;
                    
       }
       
   } 
}
