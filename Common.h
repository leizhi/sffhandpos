/****************************************************************************
*
*�ļ�(File):         Common.h
*
*�޸�(Modify):       2012-7-16 8:35:30
*
*����(Author):       �B$g)B��
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
| V1.00  2012-7-16 8:35:30           �B$g)B�� 
----------------------------------------------------------------------------
****************************************************************************/
#include "API.h"

#ifndef COMMON

#define COMMON

#define USERNAME_LEN 17 //max 8 chinese

#define ANTIFAKECODE_LEN 17

#define QUERYTIME_LEN 21

char systime[20];

void WarningBeep(int type);

void GetSysTime(unsigned char* time);

#endif
