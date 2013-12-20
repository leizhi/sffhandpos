
#ifndef _UIPROG_HEAD_
#define _UIPROG_HEAD_

extern char SetTime[];
extern char FormDB[];
extern char ExitSet[];
extern char Offline[];
extern char Upload[];
extern char Check[];
extern char SysSet[];
extern char VerInfo[]; 
extern char SCard[]; 


typedef struct
{
	//Xλ��
	unsigned int m_xpos;

	//Yλ��
	unsigned int m_ypos;
	
	//�˵�ָ��
	char *m_pMume;

	//�˵�����
	int m_MSize;

}MenuInfo;

//�˵��ṹ
typedef struct
{
	//ͼƬ��ʽ
	// 0 jpg
	// 1 bmp
	int m_BType;
	
	//�˵�����
	int m_MCount;

	//��ҳ��
	int m_PCount;
	
	//�˵�������ָ��
	//���3ҳ��,ÿҳ��9�˵�
	MenuInfo m_Mem[3][9];

	//ÿһҳ��Ĳ˵���
	int m_PMCount[3];
	
	//�˵���ʼҳ
	int m_SPage;

	//�˵���ǰҳ��
	int m_CPage;	
}SMemu;

int Browse_Icon(char *pTitle, char *menu, SMemu *pSMObj, int LineLen, int idx, int Prompt, int Font);

#endif
