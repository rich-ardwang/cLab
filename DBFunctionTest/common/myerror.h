#ifndef _WIND_MYERROR_H_
#define _WIND_MYERROR_H_

enum
{
	BE_SUCCESS = 0,							//成功
	BE_FAILED = -30,						//失败
	BE_ID_EXIST = -1,						//键值已经存在
	BE_NAME_EXIST = -2,						//键名已经存在
	BE_OVER_DEFINE_SIZE = -3,				//属性总大小超出最大定义
	BE_ERR_DEFINE_SIZE = -4,				//属性长度定义不符合规范或为0
	BE_NOT_ENOUGH_MEMORY = -5,				//没有足够的内存空间
	BE_NOT_ENOUGH_FLOW = -6,				//溢出区不够
	BE_NOT_ENOUGH_DATA = -7,				//数据区不够
	BE_NOT_DEF_PRIMARY = -15,				//主键没有定义
	BE_NOT_DEF_FIELD = -8,					//键值没有定义
	BE_NOT_DEF_TYPE = -9,					//类型没有定义




	BE_PRIMARY_INCOMPLETE = -16,			//主键不完整



	BE_NOT_MATCH_TYPE = -10,				//类型与结构中预定义的不匹配
	BE_DATA_EXIST = -11,					//插入数据时已经存在
	BE_DATA_NOT_FOUND = -12,				//数据没有找到
	BE_ERR_DECODE = -13,					//反序列化时流结尾错误
	BE_NOT_SUPPORT = -14,					//不支持的操作

	BE_MEMORY_TOO_SMALL = -20,				//要申请的内存空间太小
	BE_FILE_NOT_FOUND = -21,				//文件没有找到
	BE_TIME_OUT = -22,						//超时



	BE_NOT_EXIST_USER = -100,			//用户不存在
	BE_NOT_EXIST_USERGRP = -101,			//分组不存在
	BE_NOT_EXIST_GRP = -102,			//群不存在
	BE_EXIST_USERID = -103,			//用户ID已经存在
	BE_EXIST_GRPID = -104,			//群ID已经存在
	BE_EXIST_GRPNAME = -105,			//群名称已经存在
	BE_EXIST_USERGRPID = -106,			//分组ID已经存在
	BE_EXIST_USERGRPNAME = -107,			//分组名称已经存在
	BE_NO_RIGHT = -108,			//操作者没有权限
	BE_GRP_LIMIT = -109,			//"可以创建的群"达到上限
	BE_BLACK_LIST = -110,			//用户在黑名单中
	BE_PASSWORD_ERROR = -111,			//用户密码不正确
	BE_NOT_ALLOW_CHANGE_CRMID = -112,			//不允许切换CRMID
	BE_ANONY_LIMIT = -113,			//"可以创建的匿名用户"达到上限
	BE_DATA_NOT_COMPLETE = -114			//需要的数据不完整

};

#define RETURN_NOT_SUCCESS(r) do{if (r != BE_SUCCESS) return r;}while(0) 

#endif//_WIND_MYERROR_H_
