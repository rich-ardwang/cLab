#ifndef _IM_CONF_H_
#define _IM_CONF_H_

#include "xmlParser/IMXMLConfig.h"
#include "dbInterface/dbmgr.h"




typedef struct tagConf
{
	windcom::db::TDBParamater confDB;
} CAppConf;


namespace wind
{
    class CCommonConf
    {
    public:
		~CCommonConf();

		//获取单实例
        static CCommonConf* GetInst();

		//获取配置值
        CAppConf& GetConfItem();

    private:
		CCommonConf();
		CCommonConf(const CCommonConf& rhs) {}
		CCommonConf& operator =(const CCommonConf& rhs) {}

	private:
		int Init();
		bool GetAttByBool(CXMLConfig& config, const string& strNode, const string& strSubNode);

    private:
        static CCommonConf* m_pImServerConf;
        CAppConf m_confItem;
    };

#define IMCONFIG CCommonConf::GetInst()->GetConfItem()

}

#endif //_IM_CONF_H_
