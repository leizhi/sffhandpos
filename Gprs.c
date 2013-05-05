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
  #include"Gprs.h"
 void sim300_close(void) //�ر�gprsģ�� 
{
      DispStr_CE(0,4,"���ڹر�gprsģ�飬���Ե�",DISP_CENTER|DISP_CLRSCR);
      sim_module_close();
}
void DisConnectServer() //�Ͽ�����������Ӳ��ر�gprsģ�� 
{
       DispStr_CE(0,4,"����ע�������Ե�",DISP_CENTER|DISP_CLRSCR);
       tcp_send_data("*3#\n",strlen("*3#\n"));
       sim300_close();
}
 short sim300_init(void)   //�������磬����1��ʾ�ɹ���0 ��ʾʧ�� 
{
	short ret;
	DispStr_CE(0,0,"����ģ���ʼ��...",DISP_CENTER|DISP_CLRSCR);
	ret = sim_module_init();
	if(ret<0)
	{
	        WarningBeep(2);
		if( g_simerr == SIM_POSTYPE_ERROR )
		{
			DispStr_CE(0,2,"�˻��Ͳ�֧��GPRS",DISP_POSITION);
		}
		if( g_simerr == SIM_MODULE_ERROR )
		{
			DispStr_CE(0,2,"����ģ������",DISP_POSITION);
			DispStr_CE(0,4,"�ر�ģ��...",DISP_POSITION);
			sim_module_close();
		}
		else if( g_simerr == SIM_NO_CARD )
		{
			DispStr_CE(0,2,"����SIM������",DISP_POSITION);
			DispStr_CE(0,4,"�ر�ģ��...",DISP_POSITION);
			sim_module_close();
		}
		 return 0;
	}
	else
	{
		DispStr_CE(0,2,"ģ���ʼ���ɹ�",DISP_CENTER);
		DispStr_CE(0,4,"��������...",DISP_CENTER);		
		ret = sim_detect_net();
		if(ret<0)
		{
			DispStr_CE(0,6,"��������ʧ��",DISP_POSITION);
			 WarningBeep(2);
			sim_module_close();
			return 0;
			
		}
		else
		{
			DispStr_CE(0,6,"��������ɹ�",DISP_CENTER);
			return 1;
		}
	}
		
}



short ConnectServer()//���ӷ�����   0��ʾ���ӳɹ���-1 ��ʾ����ʧ�� 
{
         short ret ;
         DispStr_CE(0,10,"���ӷ����������Ե�...",DISP_CENTER|DISP_CLRSCR);	
            ret = tcp_create_link("118.123.244.109","8000");
            if(ret < 0)
	    {
		if( g_simerr == SIM_NO_ECHO )
		{
			DispStr_CE(0,6,"ģ������Ӧ",DISP_CENTER);
			delay_and_wait_key(0,EXIT_KEY_F2,0);
		}
		else if( g_simerr == SIM_COMMAND_ERROR )
		{
			DispStr_CE(0,6,"�������",DISP_CENTER);
			delay_and_wait_key(0,EXIT_KEY_F2,0);
		}
		else if( g_simerr == SIM_LINK_ERROR )
		{
			DispStr_CE(0,6,"����ʧ��",DISP_CENTER);
			delay_and_wait_key(0,EXIT_KEY_F2,0);
		}
		 WarningBeep(2);
		 sim_module_close();
		return -1;
	    }
	    else if(ret ==0)//���ӳɹ� 
	    {
	        return 0;
	           /* DispStr_CE(0,10,"���ӷ������ɹ�",DISP_CENTER|DISP_CLRSCR);
	            DispStr_CE(0,12,"�����ϴ������Ե�...",DISP_CENTER|DISP_CLRSCR);  
	             if(tcp_send_data(data,strlen(data))==0)//���ͳɹ� 
                     {    
                       // DispStr_CE(0,12,"���ͳɹ�",DISP_CENTER|DISP_CLRSCR);  
                        //delay_and_wait_key(0,EXIT_KEY_F2,0);                         
                      return 0;      
                     }
                     else
                     { 
                     WarningBeep(2);
                     DispStr_CE(0,10,"����ʧ��",DISP_CENTER|DISP_CLRSCR);
                     delay_and_wait_key(1,EXIT_KEY_F2|EXIT_AUTO_QUIT,1);
                     sim_module_close();
                     return -1;
                     } 
                     */
	    }
} 


short SendData(unsigned char* data)    //��������    0 ��ʾ���ͳɹ� -1 ��ʾʧ�� 
{
     DispStr_CE(0,12,"�����ϴ������Ե�...",DISP_CENTER|DISP_CLRSCR);  
     if(tcp_send_data(data,strlen(data))==0)//���ͳɹ� 
     {    
         /* DispStr_CE(0,12,"���ͳɹ�",DISP_CENTER|DISP_CLRSCR);  
         delay_and_wait_key(0,EXIT_KEY_F2,0);  
         */                       
         return 0;      
     }
     else
     { 
       //  WarningBeep(2);
        // DispStr_CE(0,10,"����ʧ��",DISP_CENTER|DISP_CLRSCR);
         //delay_and_wait_key(10,EXIT_KEY_F2|EXIT_AUTO_QUIT,10);
         return -1;
     } 
} 

 short GetRecvData(char* recvdata)     //���շ��������صĽ�� 0 ��ʾ���ճɹ���-1 ��ʾ����ʧ�� 
  {
                          short len;
                          int ret;
                        
                              ret = tcp_recv_data(recvdata,&len,256);
                              
                              if(ret == 0)                               
                              {
                              // DispStr_CE(0,10,"���ճɹ�",DISP_CENTER|DISP_CLRSCR);
                                //delay_and_wait_key(10,EXIT_KEY_F2,10);
                                 return 0;
                              }
                              else
                              {
                                      
                                DispStr_CE(0,10,"����ʧ��",DISP_CENTER|DISP_CLRSCR);
                                delay_and_wait_key(10,EXIT_KEY_F2,10);
                                      delay_and_wait_key(1,EXIT_KEY_F2|EXIT_AUTO_QUIT,1);
                                WarningBeep(2);
                                     return -1; 
                              } 
                            
  }
