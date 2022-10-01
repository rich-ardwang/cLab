#ifndef _WIND_MYERROR_H_
#define _WIND_MYERROR_H_

enum
{
	BE_SUCCESS = 0,							//�ɹ�
	BE_FAILED = -30,						//ʧ��
	BE_ID_EXIST = -1,						//��ֵ�Ѿ�����
	BE_NAME_EXIST = -2,						//�����Ѿ�����
	BE_OVER_DEFINE_SIZE = -3,				//�����ܴ�С���������
	BE_ERR_DEFINE_SIZE = -4,				//���Գ��ȶ��岻���Ϲ淶��Ϊ0
	BE_NOT_ENOUGH_MEMORY = -5,				//û���㹻���ڴ�ռ�
	BE_NOT_ENOUGH_FLOW = -6,				//���������
	BE_NOT_ENOUGH_DATA = -7,				//����������
	BE_NOT_DEF_PRIMARY = -15,				//����û�ж���
	BE_NOT_DEF_FIELD = -8,					//��ֵû�ж���
	BE_NOT_DEF_TYPE = -9,					//����û�ж���




	BE_PRIMARY_INCOMPLETE = -16,			//����������



	BE_NOT_MATCH_TYPE = -10,				//������ṹ��Ԥ����Ĳ�ƥ��
	BE_DATA_EXIST = -11,					//��������ʱ�Ѿ�����
	BE_DATA_NOT_FOUND = -12,				//����û���ҵ�
	BE_ERR_DECODE = -13,					//�����л�ʱ����β����
	BE_NOT_SUPPORT = -14,					//��֧�ֵĲ���

	BE_MEMORY_TOO_SMALL = -20,				//Ҫ������ڴ�ռ�̫С
	BE_FILE_NOT_FOUND = -21,				//�ļ�û���ҵ�
	BE_TIME_OUT = -22,						//��ʱ



	BE_NOT_EXIST_USER = -100,			//�û�������
	BE_NOT_EXIST_USERGRP = -101,			//���鲻����
	BE_NOT_EXIST_GRP = -102,			//Ⱥ������
	BE_EXIST_USERID = -103,			//�û�ID�Ѿ�����
	BE_EXIST_GRPID = -104,			//ȺID�Ѿ�����
	BE_EXIST_GRPNAME = -105,			//Ⱥ�����Ѿ�����
	BE_EXIST_USERGRPID = -106,			//����ID�Ѿ�����
	BE_EXIST_USERGRPNAME = -107,			//���������Ѿ�����
	BE_NO_RIGHT = -108,			//������û��Ȩ��
	BE_GRP_LIMIT = -109,			//"���Դ�����Ⱥ"�ﵽ����
	BE_BLACK_LIST = -110,			//�û��ں�������
	BE_PASSWORD_ERROR = -111,			//�û����벻��ȷ
	BE_NOT_ALLOW_CHANGE_CRMID = -112,			//�������л�CRMID
	BE_ANONY_LIMIT = -113,			//"���Դ����������û�"�ﵽ����
	BE_DATA_NOT_COMPLETE = -114			//��Ҫ�����ݲ�����

};

#define RETURN_NOT_SUCCESS(r) do{if (r != BE_SUCCESS) return r;}while(0) 

#endif//_WIND_MYERROR_H_
