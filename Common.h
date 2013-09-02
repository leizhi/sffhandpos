#ifndef COMMON
#define COMMON

#include "api.h"

#define USERNAME_LEN 17 //max 8 chinese
#define ANTIFAKECODE_LEN 17
#define QUERYTIME_LEN 21

extern int g_LINESpacing;
extern int g_DisplyLine;
extern int AutoPowerOff;
extern char systime[20];

//��ɫ����

//����
#define BLACK 0x0000

//����
#define NAVY 0x0010

//���
#define MAROON 0x8000

//ǳ�� 
#define BLUE 0x001F

//���
#define RED 0xF800

//Ʒ��
#define FUCHSIA 0xF81F

//����  
#define WHITE 0xFFFF   

void WarningBeep(int type);
void GetSysTime(char* time);
char* filter(char *c1,char c2);
int input_date_time(int line,int maxNum,int minNum,int minValue,int maxValue, char *pPrompt,int *retValue);
int Modify_Time(void);
int Modify_Date(void);
void DispStr_CEEX(unsigned int x, unsigned int y, char	*str, unsigned int  bgc, unsigned int  fc, int xyFlag);

#endif
