#ifndef   CARD

#define   CARD

#include "Common.h"

//ultralight 
extern unsigned char	ULATQ[3];
extern unsigned char	ULSNO[4];

extern unsigned char username[USERNAME_LEN];//Ѳ���û���
extern unsigned char password[USERNAME_LEN];//Ѳ������

extern char anticode[ANTIFAKECODE_LEN];//��α�� 

extern unsigned char sname[USERNAME_LEN];//��¼�û���
extern unsigned char spass[USERNAME_LEN];////��¼����

short ultralight_init();
short ultralight_find_card();
short ultralight_read_card(unsigned char block_add,unsigned char* databuf);
void ultralight_close();
short ultralight_local_read_card(char* ac);

int ReadNameAndPassword(unsigned char* name,unsigned char* password);

short LoginByCard();

short LoginToSubmit();

#endif
