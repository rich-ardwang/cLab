#ifndef _IM_XML_CONFIG_H_
#define _IM_XML_CONFIG_H_

#include <string>
#include <map>
#include <vector>
#include <list>
#include "xmlParser/xml.h"


using namespace std;


namespace wind
{
	class CXMLObj
	{
	public:
		virtual void FromXML(const vector<pair<string, string> >& vecMember) = 0;
	};


	class DBSet : public CXMLObj
	{
	public:
		string dataSourceType;
		string dbDomain;
		string dbName;
		string dbUserName;
		string dbPwd;
		string dbDsnInfo;

		virtual void FromXML(const vector<pair<string, string> >& vecMember)
		{
			vector<pair<string, string> >::const_iterator it = vecMember.begin();
			for (; it != vecMember.end(); ++it)
			{
				if ((*it).first == "dataSourceType")
					dataSourceType = (*it).second;
				else if ((*it).first == "dbDomain")
					dbDomain = (*it).second;
				else if ((*it).first == "dbName")
					dbName = (*it).second;
				else if ((*it).first == "dbUserName")
					dbUserName = (*it).second;
				else if ((*it).first == "dbPwd")
					dbPwd = (*it).second;
				else if ((*it).first == "dbDsnInfo")
					dbDsnInfo = (*it).second;
			}
		}
	};

	class CXMLConfig
	{
	public:
		explicit CXMLConfig(const char *xmlFileName);

	public:
		//新接口，支持路径访问，jbian
		bool GetValue(const string& strNodePath, string& strValue);
		bool GetAttr(const string& strNodePath, string& strAttr);
		bool EnumValue(const string& strNodePath, vector<pair<string, string> >& vecValue);
		bool EnumAttr(const string& strNodePath, vector<pair<string, string> >& vecValue);

		template <class T> bool GetObject(const string& strNodePath, T& obj)
		{
			vector<pair<string, string> > vecAttr;
			if (EnumAttr(strNodePath, vecAttr) == false)
				return false;

			obj.FromXML(vecAttr);
			return true;
		}


		//得到nodeName结点的值
		std::string GetValue(const std::string &nodeName);
		std::string GetAttribute(const std::string &Name);
		int GetAttributeByList(const std::string &Name, list<string> &lstImApp);
		int GetAttributeByVector(const std::string &FarName, const std::string &Name, const std::string &Node, vector<string> &lstImApp);
		int GetAttributeByList(const std::string &Name, list<unsigned short> &lstImApp);
		int GetAttributeByList(const std::string &Name, list<int> &lstImApp);
		int GetAttributeByMap(const std::string &Name, map<unsigned short, string> &mapIndicator, unsigned short *p, unsigned int len);
		int GetAttributeByMap(const std::string &Name, map<int, int> &mapMemPool);
		int GetValueByNum(const std::string &nodeName, int &nValue);

		//根据节点名和属性名查询
		int GetValue(const string & nodeName, 
			const string & attrName, 
			string & value);

		//根据二级节点获取属性值
		int GetValue(const string & nodeName, const string & subNodeName, 
			const string & attrName, 
			string & value);

		int GetValue(const string & nodeName, 
			const string & attrName, 
			int & value);

		//根据二级节点获取属性值
		int GetValue(const string & nodeName, const string & subNodeName, 
			const string & attrName, 
			int & value);

		int GetNodeValue(const string & nodeName, const string & subNodeName, 
			string & value);

		//int GetAttributeByMap(const std::string &Name, map<int, string> &mapSys);

		void StringToUpper(string &strValue);

	private:
		string NodePath(const string& strPath);

	private:

		TiXmlDocument    mXmlConfigFile;
		TiXmlElement    *mRootElem;
	};
}

#endif
