#include "xmlParser/IMConfig.h"
#include "logInterface/windLog.h"
#include "common/type.h"
#include "common/file.h"


using namespace windcom;


namespace wind
{

	CCommonConf* CCommonConf::m_pImServerConf = NULL;

	CCommonConf::CCommonConf()
	{
	}

	CCommonConf::~CCommonConf()
	{
	}

	CCommonConf* CCommonConf::GetInst()
	{
		if (m_pImServerConf == NULL)
		{
			m_pImServerConf = new CCommonConf();

			int nInit = m_pImServerConf->Init();
			if (nInit != Success)
			{
				WINDLOG_ERROR(("CommonConf"), "Load config file failed [" << nInit << "]");
			}
		}

		return m_pImServerConf;
	}

	CAppConf& CCommonConf::GetConfItem()
	{
		return m_confItem;
	}

	int CCommonConf::Init()
	{
		string strConfPath = GetProcessPath() + "/DBFunction.xml";
		CXMLConfig stXMLConfig(strConfPath.c_str());

		//读取数据库读写配置和连接数
		stXMLConfig.GetValue("dbParameter", "dbReadType", m_confItem.confDB.nReadType);
		stXMLConfig.GetValue("dbParameter", "dbWriteType", m_confItem.confDB.nWriteType);
		stXMLConfig.GetValue("dbParameter", "dbConNum", m_confItem.confDB.nConnNum);

		// 读取ConnectionStringMYSQL标签内属性值
		stXMLConfig.GetValue("ConnectionStringMYSQL", "dataSourceType", m_confItem.confDB.nDataSourceTypeMysql);
		stXMLConfig.GetValue("ConnectionStringMYSQL", "dbDomain", m_confItem.confDB.strDomainMysql);
		stXMLConfig.GetValue("ConnectionStringMYSQL", "dbName", m_confItem.confDB.strNameMysql);
		stXMLConfig.GetValue("ConnectionStringMYSQL", "dbUserName", m_confItem.confDB.strUserNameMysql);
		stXMLConfig.GetValue("ConnectionStringMYSQL", "dbPwd", m_confItem.confDB.strPwdMysql);
		stXMLConfig.GetValue("ConnectionStringMYSQL", "dbDsnInfo", m_confItem.confDB.strDsnInfoMysql);
		stXMLConfig.GetValue("ConnectionStringMYSQL", "mySQLDriverName", m_confItem.confDB.strDriverName);

		return Success;
	}

	bool CCommonConf::GetAttByBool(CXMLConfig& config, const string& strNode, const string& strSubNode)
	{
		int nRet = 0;
		config.GetValue(strNode, strSubNode, nRet);
		bool bRet = (nRet != 0 ? true : false);
		return bRet;
	}
}
