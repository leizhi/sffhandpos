#ifndef		DATABASE
#define		DATABASE

#include "common.h"

//���ݿ��ռ����
#define BLOCKNUM 10

//ÿ���ϴ�����
#define PAGE_SIZE 30

//�������ݱ�ṹ 
typedef struct {
	unsigned int id;//id�ŷ������
	char username[USERNAME_LEN];//�û���
	char antifakecode[ANTIFAKECODE_LEN];//��α��
	char querytime[QUERYTIME_LEN];//Ѳ��ʱ��
}DataInfo;

DataInfo datainfo;

char Menu[150*PAGE_SIZE+2];

int FindDatabase();

short CreateDatabase();

short InitDatabase(); 

short AddOneRecord(unsigned char* name,char* time,char* code,char* record);

short EncodeSendData(unsigned char* name ,unsigned char* passwd,unsigned char* senddata);

short HandleRecvData(unsigned char* recvdata);

void UpdateDatabase(unsigned char* recvdata);

int dbClean();

#endif
