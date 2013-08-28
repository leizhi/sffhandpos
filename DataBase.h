#ifndef		DATABASE
#define		DATABASE

#include "common.h"

//���ݿ��ռ����
#define BLOCKNUM 10

//�г���
#define LINE_LEN 30

//ÿ���ϴ�����
//GPRSһ������ܴ���1024�ֽ�
// 1024 / sizeof(DataInfo) + 10
// 10Ϊ��¼���¼����ķָ���
#define PAGE_SIZE 20

//�������ݱ�ṹ 
typedef struct {
	unsigned int id;//id�ŷ������
	char username[USERNAME_LEN];//�û���
	char antifakecode[ANTIFAKECODE_LEN];//��α��
	char querytime[QUERYTIME_LEN];//Ѳ��ʱ��
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
