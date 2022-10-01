#include "xmlParser/IMXMLConfig.h"
#include "logInterface/windlog.h"
#include "common/type.h"
#include "common/strex.h"
#include <algorithm>


using namespace windcom;


namespace wind
{
	CXMLConfig::CXMLConfig(const char *xmlFileName)
		: mXmlConfigFile(xmlFileName), mRootElem(0)
	{
		//加载配置文件
		if (!mXmlConfigFile.LoadFile())
		{
			WINDLOG_ERROR(("CXMLConfig"), " Load config file [" << xmlFileName << "] error!");
		}
		else
		{
			//得到配置文件的根结点
			mRootElem = mXmlConfigFile.RootElement();
		}
	}

	bool CXMLConfig::GetValue(const string& strNodePath, string& strValue)
	{
		strValue.clear();

		if (!mRootElem)
		{
			WINDLOG_WARN(("CXMLConfig"), "read root error!");
			return false;
		}

		TiXmlElement *pElem = mRootElem;
		vector<string> vecNodes;
		SplitAbso(NodePath(strNodePath), "/", vecNodes);

		for (vector<string>::iterator it = vecNodes.begin(); it != vecNodes.end(); ++it)
		{
			if ((*it).empty()) continue;
			pElem = pElem->FirstChildElement((*it).c_str());
			if (!pElem)
			{
				WINDLOG_WARN(("CXMLConfig"), "Read node [" << *it << "] error!");
				return false;
			}
		}

		strValue = pElem->GetText();
		return true;
	}

	bool CXMLConfig::EnumValue(const string& strNodePath, vector<pair<string, string> >& vecValue)
	{
		vecValue.clear();

		if (!mRootElem)
		{
			WINDLOG_WARN(("CXMLConfig"), "read root error!");
			return false;
		}

		TiXmlElement *pElem = mRootElem;
		vector<string> vecNodes;
		SplitAbso(NodePath(strNodePath), "/", vecNodes);

		for (vector<string>::iterator it = vecNodes.begin(); it != vecNodes.end(); ++it)
		{
			if ((*it).empty()) continue;
			pElem = pElem->FirstChildElement((*it).c_str());
			if (!pElem)
			{
				WINDLOG_WARN(("CXMLConfig"), "Read node [" << *it << "] error!");
				return false;
			}
		}

		for (pElem = pElem->FirstChildElement(); pElem != NULL; pElem = pElem->NextSiblingElement())
		{
			vecValue.push_back(make_pair(pElem->Value(), pElem->GetText()));
		}
	
		return true;
	}

	bool CXMLConfig::GetAttr(const string& strNodePath, string& strAttr)
	{
		strAttr.clear();

		if (!mRootElem)
		{
			WINDLOG_WARN(("CXMLConfig"), "read root error!");
			return false;
		}

		TiXmlElement *pElem = mRootElem;
		vector<string> vecNodes;
		SplitAbso(NodePath(strNodePath), "/", vecNodes);

		size_t i = 0;
		for (; i < vecNodes.size() - 1; ++i)//前面是节点的路径
		{
			if (vecNodes[i].empty()) continue;
			pElem = pElem->FirstChildElement(vecNodes[i].c_str());
			if (!pElem)
			{
				WINDLOG_WARN(("CXMLConfig"), "Read node [" << vecNodes[i] << "] error!");
				return false;
			}
		}

		//最后面是属性的路径
		const char *p = pElem->Attribute(vecNodes[i].c_str());
		if (p == NULL) return false;

		strAttr = p;
		return true;
	}
	
	bool CXMLConfig::EnumAttr(const string& strNodePath, vector<pair<string, string> >& vecValue)
	{
		vecValue.clear();

		if (!mRootElem)
		{
			WINDLOG_WARN(("CXMLConfig"), "read root error!");
			return false;
		}

		TiXmlElement *pElem = mRootElem;
		vector<string> vecNodes;
		SplitAbso(NodePath(strNodePath), "/", vecNodes);

		for (vector<string>::iterator it = vecNodes.begin(); it != vecNodes.end(); ++it)
		{
			if ((*it).empty()) continue;
			pElem = pElem->FirstChildElement((*it).c_str());
			if (!pElem)
			{
				WINDLOG_WARN(("CXMLConfig"), "Read node [" << *it << "] error!");
				return false;
			}
		}

		//最后面是属性的路径
		for (TiXmlAttribute *pAttr = pElem->FirstAttribute(); pAttr != NULL; pAttr = pAttr->Next())
		{
			vecValue.push_back(make_pair(pAttr->Name(), pAttr->Value()));
		}

		return true;
	}

	std::string CXMLConfig::GetValue(const std::string &nodeName)
	{
		if (!mRootElem)
		{
			WINDLOG_WARN(("CXMLConfig"), "read root error!");

			return "";
		}

		TiXmlElement *pElem = mRootElem->FirstChildElement(nodeName.c_str());
		if (!pElem)
		{
			WINDLOG_WARN(("CXMLConfig"), "Read " << nodeName << " node error");

			return "";
		}
		return pElem->GetText();
	}

	int CXMLConfig::GetValueByNum(const std::string &nodeName, int &nValue)
	{
		if (!mRootElem)
		{
			WINDLOG_WARN(("CXMLConfig"), "read root error!");
			return Failure;
		}

		TiXmlElement *pElement = mRootElem->FirstChildElement(nodeName.c_str());
		if (!pElement)
		{
			WINDLOG_WARN(("CXMLConfig"), "Read " << nodeName << " node error");
			return Failure;
		}
		pElement = pElement->FirstChildElement();
		if (!pElement)
		{
			WINDLOG_WARN(("CXMLConfig"), "Read " << nodeName << " node error");
			return Failure;
		}
		TiXmlAttribute *pAttribute = pElement->FirstAttribute();
		if (!pAttribute)
		{
			return Failure;
		}
		nValue = pAttribute->IntValue();
		return Success;
	}

	std::string CXMLConfig::GetAttribute(const std::string &Name)
	{
		if (!mRootElem)
		{
			WINDLOG_WARN(("CXMLConfig"), "read root error!");

			return "";
		}
		TiXmlElement *pElement = mRootElem->FirstChildElement(Name.c_str());
		if (pElement == NULL)
		{
			WINDLOG_WARN(("CXMLConfig"), "pElement is NULL");
			return "";
		}
		pElement = pElement->FirstChildElement();
		if (!pElement)
		{
			WINDLOG_WARN(("CXMLConfig"), "pElement  error");
			return "";
		}

		TiXmlAttribute *pAttribute = pElement->FirstAttribute();

		for (; pAttribute != NULL; pAttribute = pAttribute->Next())
		{
			//cout << pAttribute->Name() << " : " << pAttribute->Value() << std::endl;
		}
		return pAttribute->Value();

	}

	int CXMLConfig::GetAttributeByList(const std::string &Name, list<string> &lstImApp)
	{
		lstImApp.clear();
		if (!mRootElem)
		{
			WINDLOG_WARN(("CXMLConfig"), "read root error!");
			return Failure;
		}

		TiXmlElement *pElement = mRootElem->FirstChildElement(Name.c_str());
		if (pElement == NULL)
		{
			WINDLOG_WARN(("CXMLConfig"), "pElement is NULL");
			return Failure;
		}
		pElement = pElement->FirstChildElement();
		if (!pElement)
		{
			WINDLOG_WARN(("CXMLConfig"), "pElement is NULL");
			return Failure;
		}
		for (; pElement != NULL; pElement = pElement->NextSiblingElement())
		{
			lstImApp.push_back(pElement->GetText());
		}
		return Success;
	}

	int CXMLConfig::GetAttributeByVector(const std::string &FarName, const std::string &Name, const std::string &Node, vector<string> &lstImApp)
	{
		lstImApp.clear();
		if (!mRootElem)
		{
			WINDLOG_WARN(("CXMLConfig"), "read root error!");
			return Failure;
		}

		TiXmlElement *pElement = mRootElem->FirstChildElement(FarName.c_str());
		if (pElement == NULL)
		{
			WINDLOG_WARN(("CXMLConfig"), "pElement is NULL");
			return Failure;
		}

		pElement = pElement->FirstChildElement(Name.c_str());
		if (!pElement)
		{
			WINDLOG_WARN(("CXMLConfig"), "pElement is NULL");
			return Failure;
		}


		pElement = pElement->FirstChildElement(Node.c_str());
		if (!pElement)
		{
			WINDLOG_WARN(("CXMLConfig"), "pElement is NULL");
			return Failure;
		}

		for (; pElement != NULL; pElement = pElement->NextSiblingElement())
		{
			lstImApp.push_back(pElement->GetText());
		}
		return Success;
	}

	int CXMLConfig::GetAttributeByList(const std::string &Name, list<unsigned short> &lstImApp)
	{
		lstImApp.clear();
		if (!mRootElem)
		{
			WINDLOG_WARN(("CXMLConfig"), "read root error!");

			return Failure;
		}

		TiXmlElement *pElement = mRootElem->FirstChildElement(Name.c_str());
		if (pElement == NULL)
		{
			WINDLOG_WARN(("CXMLConfig"), "pElement is NULL");
			return Failure;
		}
		pElement = pElement->FirstChildElement();
		if (!pElement)
		{
			WINDLOG_WARN(("CXMLConfig"), "pElement is NULL");
			return Failure;
		}
		for (; pElement != NULL; pElement = pElement->NextSiblingElement())
		{
			TiXmlAttribute *pAttribute = pElement->FirstAttribute();
			lstImApp.push_back(pAttribute->IntValue());

		}
		return Success;
	}

	int CXMLConfig::GetAttributeByList(const std::string &Name, list<int> &lstImApp)
	{
		lstImApp.clear();
		if (!mRootElem)
		{
			WINDLOG_WARN(("CXMLConfig"), "read root error!");

			return Failure;
		}
		TiXmlElement *pElement = mRootElem->FirstChildElement(Name.c_str());
		if (pElement == NULL)
		{
			WINDLOG_WARN(("CXMLConfig"), "pElement is NULL");
			return Failure;
		}
		pElement = pElement->FirstChildElement();
		if (!pElement)
		{
			WINDLOG_WARN(("CXMLConfig"), "pElement is NULL");
			return Failure;
		}
		for (; pElement != NULL; pElement = pElement->NextSiblingElement())
		{
			TiXmlAttribute *pAttribute = pElement->FirstAttribute();
			lstImApp.push_back(pAttribute->IntValue());
		}
		return Success;
	}

	int CXMLConfig::GetAttributeByMap(const std::string &Name, map<unsigned short, string> &mapIndicator, unsigned short *p, unsigned int len)
	{
		mapIndicator.clear();
		if (!mRootElem)
		{
			WINDLOG_WARN(("CXMLConfig"), "read root error!");

			return Failure;
		}
		TiXmlElement *pElement = mRootElem->FirstChildElement(Name.c_str());
		if (pElement == NULL)
		{
			WINDLOG_WARN(("CXMLConfig"), "pElement is NULL");
			return Failure;
		}
		pElement = pElement->FirstChildElement();
		if (!pElement)
		{
			WINDLOG_WARN(("CXMLConfig"), "pElement is NULL");
			return Failure;
		}
		for (; pElement != NULL; pElement = pElement->NextSiblingElement())
		{
			TiXmlAttribute *pAttribute = pElement->FirstAttribute();

			string strIndicatorName =  pAttribute->Value();
			StringToUpper(strIndicatorName);
			pAttribute = pAttribute->Next();
			if (!pAttribute)
			{
				return Failure;
			}
			int IndicatorValue = pAttribute->IntValue();

			mapIndicator.insert(std::make_pair(IndicatorValue, strIndicatorName));

			pAttribute = pAttribute->Next();
			if (!pAttribute)
			{
				return Failure;
			}

			int imIndVaule = pAttribute->IntValue();
			if (imIndVaule == 0 || imIndVaule >= (int)len)
			{
				//WINDLOG_WARN(("CXMLConfig"), "IndicatorValue="<<IndicatorValue<<" IMServerVal="<<imIndVaule);
			}
			else
			{
				//
				if (p)
					*(p+imIndVaule) = IndicatorValue;
			}
		}
		return Success;
	}

	void CXMLConfig::StringToUpper(string &strValue)
	{
		transform(strValue.begin(), strValue.end(), strValue.begin(), ::toupper);
	}


	int CXMLConfig::GetAttributeByMap(const std::string &Name, map<int, int> &mapMemPool)
	{
		mapMemPool.clear();
		if (!mRootElem)
		{
			WINDLOG_WARN(("CXMLConfig"), "read root error!");

			return Failure;
		}
		TiXmlElement *pElement = mRootElem->FirstChildElement(Name.c_str());
		if (pElement == NULL)
		{
			WINDLOG_WARN(("CXMLConfig"), Name<<" pElement is NULL");
			return Failure;
		}
		pElement = pElement->FirstChildElement();
		if (!pElement)
		{
			WINDLOG_WARN(("CXMLConfig"), "pElement is NULL");
			return Failure;
		}
		for (; pElement != NULL; pElement = pElement->NextSiblingElement())
		{
			TiXmlAttribute *pAttribute = pElement->FirstAttribute();

			int nPieceSize =  pAttribute->IntValue();
			pAttribute = pAttribute->Next();
			if (!pAttribute)
			{
				return Failure;
			}
			int nTotalSize = pAttribute->IntValue();

			//mapMemPool[nPieceSize] = nTotalSize;
			mapMemPool.insert(std::make_pair(nPieceSize, nTotalSize));
		}
		return Success;
	}




	int CXMLConfig::GetValue(const string & nodeName, 
		const string & attrName, 
		string & value)
	{
		if (!mRootElem)
		{
			WINDLOG_WARN(("CXMLConfig"), "read root error!");
			return Failure;
		}

		TiXmlElement *pElement = mRootElem->FirstChildElement(nodeName.c_str());
		if (pElement == NULL)
		{
			WINDLOG_WARN(("CXMLConfig"), nodeName<<" pElement is NULL");
			return Failure;
		}


		TiXmlAttribute *pAttribute = pElement->FirstAttribute();

		for (; pAttribute != NULL; pAttribute = pAttribute->Next())
		{
			if (0 == strcmp(attrName.c_str(), pAttribute->Name()))
			{
				value = pAttribute->Value();
				return Success;
			}
		}

		return Failure;
	}

	int CXMLConfig::GetValue(const string & nodeName, const string & subNodeName, 
		const string & attrName, string & value)
	{
		if (!mRootElem)
		{
			WINDLOG_WARN(("CXMLConfig"), "read root error!");
			return Failure;
		}

		TiXmlElement *pElement = mRootElem->FirstChildElement(nodeName.c_str());
		if (pElement == NULL)
		{
			WINDLOG_WARN(("CXMLConfig"), nodeName<<" pElement is NULL");
			return Failure;
		}

		pElement = pElement->FirstChildElement(subNodeName.c_str());
		if (pElement == NULL)
		{
			WINDLOG_WARN(("CXMLConfig"), subNodeName<<" pElement is NULL");
			return Failure;
		}

		TiXmlAttribute *pAttribute = pElement->FirstAttribute();

		for (; pAttribute != NULL; pAttribute = pAttribute->Next())
		{
			if (0 == strcmp(attrName.c_str(), pAttribute->Name()))
			{
				value = pAttribute->Value();
				return Success;
			}
		}

		return Failure;
	}

	int CXMLConfig::GetValue(const string & nodeName, 
		const string & attrName, 
		int & value)
	{
		if (!mRootElem)
		{
			WINDLOG_WARN(("CXMLConfig"), "read root error!");

			return Failure;
		}
		TiXmlElement *pElement = mRootElem->FirstChildElement(nodeName.c_str());
		if (pElement == NULL)
		{
			WINDLOG_WARN(("CXMLConfig"), nodeName<<" pElement is NULL");
			return Failure;
		}

		TiXmlAttribute *pAttribute = pElement->FirstAttribute();

		for (; pAttribute != NULL; pAttribute = pAttribute->Next())
		{
			if (0 == strcmp(attrName.c_str(), pAttribute->Name()))
			{
				value = pAttribute->IntValue();
				return Success;
			}
		}

		return Failure;
	}

	int CXMLConfig::GetValue(const string & nodeName, const string & subNodeName, 
		const string & attrName, int & value)
	{
		if (!mRootElem)
		{
			WINDLOG_WARN(("CXMLConfig"), "read root error!");

			return Failure;
		}
		TiXmlElement *pElement = mRootElem->FirstChildElement(nodeName.c_str());
		if (pElement == NULL)
		{
			WINDLOG_WARN(("CXMLConfig"), nodeName<<" pElement is NULL");
			return Failure;
		}

		pElement = pElement->FirstChildElement(subNodeName.c_str());
		if (pElement == NULL)
		{
			WINDLOG_WARN(("CXMLConfig"), subNodeName<<" pElement is NULL");
			return Failure;
		}

		TiXmlAttribute *pAttribute = pElement->FirstAttribute();

		for (; pAttribute != NULL; pAttribute = pAttribute->Next())
		{
			if (0 == strcmp(attrName.c_str(), pAttribute->Name()))
			{
				value = pAttribute->IntValue();
				return Success;
			}
		}

		return Failure;
	}

	int CXMLConfig::GetNodeValue(const string & nodeName, const string & subNodeName, 
		string & value)
	{
		if (!mRootElem)
		{
			WINDLOG_WARN(("CXMLConfig"), "read root error!");
			return Failure;
		}

		TiXmlElement *pElement = mRootElem->FirstChildElement(nodeName.c_str());
		if (pElement == NULL)
		{
			WINDLOG_WARN(("CXMLConfig"), "pElement is NULL");
			return Failure;
		}

		pElement = pElement->FirstChildElement(subNodeName.c_str());
		if (!pElement)
		{
			WINDLOG_WARN(("CXMLConfig"), "pElement is NULL");
			return Failure;
		}

		value = pElement->GetText();

		return Success;
	}

	/*
	int CXMLConfig::GetAttributeByMap(const std::string &Name, map<int, string> &mapSys)
	{
		if (!mRootElem)
		{
			WINDLOG_WARN(("CXMLConfig"), "read root error!");

			return Failure;
		}
		TiXmlElement *pElement = mRootElem->FirstChildElement(Name.c_str());
		if (pElement == NULL)
		{
			WINDLOG_WARN(("CXMLConfig"), Name<<" pElement is NULL");
			return Failure;
		}
		pElement = pElement->FirstChildElement();
		if (!pElement)
		{
			WINDLOG_WARN(("CXMLConfig"), "pElement is NULL");
			return Failure;
		}
		for (; pElement != NULL; pElement = pElement->NextSiblingElement())
		{
			TiXmlAttribute *pAttribute = pElement->FirstAttribute();
			if (!pAttribute)
			{
				return Failure;
			}

			string groupName = CODE_CONVERT_TO_UTF8(pAttribute->Value());

			pAttribute = pAttribute->Next();
			if (!pAttribute)
			{
				return Failure;
			}
			int nTotalSize = pAttribute->IntValue();

			mapSys.insert(std::make_pair(pAttribute->IntValue(), groupName));
		}
		return Success;
	}
	*/

	string CXMLConfig::NodePath(const string& strPath)
	{
		if (strPath.empty()) return strPath;
		string::size_type head = strPath.find_first_not_of("/");
		string::size_type tail = strPath.find_last_not_of("/");
		return strPath.substr(head, tail - head + 1);
	}
}
