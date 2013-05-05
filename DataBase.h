/****************************************************************************
*
*�ļ�(File):         DataBase.h
*
*�޸�(Modify):       2012-7-13 14:23:46
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
| V1.00  2012-7-13 14:23:46           sa 
----------------------------------------------------------------------------
****************************************************************************/

#include "api.h"
#include "string.h"
#include "common.h"

#ifndef		DATABASE
#define		DATABASE

#define BLOCKNUM 10 //���ݿ��ռ����

#define PAGENUM 30

//�������ݱ�ṹ 
typedef struct { 
	unsigned int id;//id�ŷ������ 
	char username[USERNAME_LEN];//�û���
	char antifakecode[ANTIFAKECODE_LEN];//��α��
	char querytime[QUERYTIME_LEN];//Ѳ��ʱ��
}DataInfo;

DataInfo datainfo;

char Menu[150*28+2];

short FindDatabase();

short CreateDatabase();

short InitDatabase(); 

short AddOneRecord(char* name,char* time,char* code,char* record);

short EncodeSendData(char* name ,char* passwd,char* senddata);

#endif