#ifndef		DATABASE
#define		DATABASE

#include "common.h"

#define USERNAME_LEN 17 //max 8 chinese
#define ANTIFAKECODE_LEN 17
#define QUERYTIME_LEN 21


//���ݿ��ռ����
#define BLOCKNUM 10

//�г���
#define LINE_LEN 30

//ÿ���ϴ�����
//GPRSһ������ܴ���1024�ֽ�
// (1024 - 20) / (sizeof(DataInfo) + 10)
//	20 Ϊǰ�������
// 10Ϊ��¼���¼����ķָ���
#define PAGE_SIZE 14

//�������ݱ�ṹ 
typedef struct {
	//id�ŷ������
	unsigned int id;
	
	//�û���
	char username[USERNAME_LEN];

	//��α��
	char antifakecode[ANTIFAKECODE_LEN];

	//Ѳ��ʱ��
	char querytime[QUERYTIME_LEN];
}DataInfo;

extern char Menu[150*LINE_LEN+2];

int FindDatabase();

short CreateDatabase();

short InitDatabase(); 

short AddOneRecord(unsigned char* name,char* time,char* code,char* record);

short EncodeSendData(unsigned char* name ,unsigned char* passwd,unsigned char* senddata);

short HandleRecvData(unsigned char* recvdata);

void UpdateDatabase(unsigned char* recvdata);

int dbClean();

#endif
