#ifndef   CARD

#define   CARD

#include "Common.h"

//ultralight 
unsigned char	ULATQ[3];
unsigned char	ULSNO[4];

unsigned char username[USERNAME_LEN];//Ѳ���û���
unsigned char password[USERNAME_LEN];//Ѳ������

char anticode[ANTIFAKECODE_LEN];//��α�� 

unsigned char sname[USERNAME_LEN];//��¼�û���
unsigned char spass[USERNAME_LEN];////��¼����

short ultralight_init();
short ultralight_find_card();
short ultralight_read_card(unsigned char block_add,unsigned char* databuf);
void ultralight_close();
short ultralight_local_read_card(char* ac);

int ReadNameAndPassword(unsigned char* name,unsigned char* password);

short LoginByCard();

short LoginToSubmit();

#endif
