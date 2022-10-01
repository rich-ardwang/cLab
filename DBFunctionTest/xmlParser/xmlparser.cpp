/*
* =====================================================================================
*
*       Filename:  xmlparser.cpp
*    Description:
*				   用来解析xml配置文件（陈万鸣：2012-7-10）
*        Version:  1.0
*        Created:  2012-7-10
*       Revision:  丁富来合入Common库@2015-4-13
*       Compiler:  gcc
*
*         Author:  chenwanming
*        Company:  wind, Inc.
*
* =====================================================================================
*/

#include "xmlParser/xmlparser.h"

#include <algorithm>
#include <string>
#include "common/type.h"
#include "logInterface/logger.h"

using namespace windcom;

namespace windcom
{

#define XML_NOT_EXIST(_pElement, _strFunc) do{\
	if (!_pElement)\
	{\
		LOG4CXX_WARN(GETLOGGER("XMLParser::"#_strFunc), "Read node error!");\
		return false;\
	}\
}while(0)

#define XML_CHILD1_NAME_NOT_EXIST(_pElement, _strFunc, _strName) do{\
	_pElement = _pElement->FirstChildElement(_strName.c_str());\
	if (!_pElement)\
	{\
		LOG4CXX_WARN(GETLOGGER("CWindXMLConfig::"#_strFunc), "Read " << _strName << " node error");\
		return false;\
	}\
}while(0)

#define XML_CHILD1_NOT_EXIST(_pElement, _strFunc) do{\
	_pElement = _pElement->FirstChildElement();\
	if (!_pElement)\
	{\
		LOG4CXX_WARN(GETLOGGER("CWindXMLConfig::"#_strFunc), "Read node error");\
		return "";\
	}\
}while(0)


	XMLParser::XMLParser(const char *xmlFileName) : mXmlConfigFile(xmlFileName), mRootElem(0)
	{
		//加载配置文件
		if (!mXmlConfigFile.LoadFile())
		{
			LOG4CXX_ERROR(GETLOGGER("CWindXMLConfig::CWindXMLConfig"), " Load config file ["<<xmlFileName<<"]error!");
		}
		else
		{
			//得到配置文件的根结点
			mRootElem = mXmlConfigFile.RootElement();
		}
	}

	//////////////////////////////////////////////////////////////////////////

	TiXmlElement*  XMLParser::GetSubNode(const string& strName)
	{
		TiXmlElement *pElement = mRootElem;
		if (!pElement)
		{
			LOG4CXX_WARN(GETLOGGER("XMLParser::GetSubNode"), "Read node error!");
			return NULL;
		}

		pElement = pElement->FirstChildElement(strName.c_str());
		if (!pElement)
		{
			LOG4CXX_WARN(GETLOGGER("CWindXMLConfig::GetSubNode"), "Read " << strName << " node error");
			return NULL;
		}

		return pElement;
	}

	bool XMLParser::GetValue(TiXmlElement *pNode, const std::string &strName, std::string &strValue)
	{
		strValue.clear();
		TiXmlElement *pElement = pNode;
		XML_NOT_EXIST(pElement, "GetValue");
		XML_CHILD1_NAME_NOT_EXIST(pElement, "GetValue", strName);

		strValue = pElement->GetText();

		return true;
	}

	bool XMLParser::GetAttribute(TiXmlElement *pNode, const std::string &strName, std::string &strValue)
	{
		strValue.clear();
		TiXmlElement *pElement = pNode;
		XML_NOT_EXIST(pElement, "GetAttribute");
		XML_CHILD1_NAME_NOT_EXIST(pElement, "GetAttribute", strName);
		//XML_CHILD1_NOT_EXIST(pElement, "GetAttribute");

		TiXmlAttribute *pAttribute = pElement->FirstAttribute();
		strValue = pAttribute?pAttribute->Value():"";
		return pAttribute?true:false;
	}

	bool XMLParser::GetAttribute(TiXmlElement *pNode, const string &strName, vector<string> &oAttributeList)
	{
		oAttributeList.clear();
		TiXmlElement *pElement = pNode;
		XML_NOT_EXIST(pElement, "GetAttribute");
		XML_CHILD1_NAME_NOT_EXIST(pElement, "GetAttribute", strName);
		//XML_CHILD1_NOT_EXIST(pElement, "GetAttribute");

		TiXmlAttribute *pAttribute = pElement->FirstAttribute();
		for(; pAttribute != NULL; pAttribute = pAttribute->Next())
		{
			oAttributeList.push_back(pAttribute->Value());
		}
		return oAttributeList.empty()?false:true;
	}

	bool XMLParser::GetAttribute(TiXmlElement *pNode, const string &strName, const string& strSub, vector<vector<string> > &oAttributeMatrix)
	{
		oAttributeMatrix.clear();
		TiXmlElement *pElement = pNode;
		XML_NOT_EXIST(pElement, "GetAttribute");
		XML_CHILD1_NAME_NOT_EXIST(pElement, "GetAttribute", strName);
		XML_CHILD1_NAME_NOT_EXIST(pElement, "GetAttribute", strSub);

		for(; pElement != NULL; pElement = pElement->NextSiblingElement())
		{
			if(strcmp(strSub.c_str(), pElement->Value()) != 0)
				continue;

			TiXmlAttribute *pAttribute = pElement->FirstAttribute();
			vector<string> oAttributeList;
			for(; pAttribute != NULL; pAttribute = pAttribute->Next())
			{
				oAttributeList.push_back(pAttribute->Value());
			}
			oAttributeMatrix.push_back(oAttributeList);
		}

		return oAttributeMatrix.empty()?false:true;
	}

	bool XMLParser::GetAttribute(TiXmlElement *pNode, const string &strName, const string &strAttr, string &strValue)
	{
		TiXmlElement *pElement = pNode;
		XML_NOT_EXIST(pElement, "GetAttribute");
		XML_CHILD1_NAME_NOT_EXIST(pElement, "GetAttribute", strName);

		for(; pElement != NULL; pElement = pElement->NextSiblingElement())
		{
			TiXmlAttribute *pAttribute = pElement->FirstAttribute();
			for (; pAttribute != NULL; pAttribute = pAttribute->Next())
			{
				if (0 == strcmp(strAttr.c_str(), pAttribute->Name()))
				{
					strValue = pAttribute->Value();
					return true;
				}
			}
		}
		return false;
	}

	bool XMLParser::GetAttribute(TiXmlElement *pNode, const string &strName, const string &strAttr, int &iValue)
	{
		TiXmlElement *pElement = pNode;
		XML_NOT_EXIST(pElement, "GetAttribute");
		XML_CHILD1_NAME_NOT_EXIST(pElement, "GetAttribute", strName);

		for(; pElement != NULL; pElement = pElement->NextSiblingElement())
		{
			TiXmlAttribute *pAttribute = pElement->FirstAttribute();
			for (; pAttribute != NULL; pAttribute = pAttribute->Next())
			{
				if (0 == strcmp(strAttr.c_str(), pAttribute->Name()))
				{
					iValue = pAttribute->IntValue();
					return true;
				}
			}
		}
		return false;
	}
	
	//////////////////////////////////////////////////////////////////////////

	bool XMLParser::GetValue(const string &strName, string& strValue)
	{
		return GetValue(mRootElem, strName, strValue);
	}

	bool XMLParser::GetAttribute(const string &strName, string& strValue)
	{
		return GetAttribute(mRootElem, strName, strValue);
	}

	bool XMLParser::GetAttribute(const string &strName, vector<string>& oAttributes)
	{
		return GetAttribute(mRootElem, strName, oAttributes);
	}

	bool XMLParser::GetAttribute(const string &strName, const string &strSub, vector<vector<string> > &oAttributeMatrix)
	{
		return GetAttribute(mRootElem, strName, strSub, oAttributeMatrix);
	}

	bool XMLParser::GetAttribute(const string& strName, const string& strAttr, string& strValue)
	{
		return GetAttribute(mRootElem, strName, strAttr, strValue);
	}

	bool XMLParser::GetAttribute(const string& strName, const string& strAttr, int& iValue)
	{
		return GetAttribute(mRootElem, strName, strAttr, iValue);
	}

	#if 0
	void XMLParser::StringToUpper(string &strValue)
	{
		transform(strValue.begin(), strValue.end(), strValue.begin(), ::toupper);
	}

	int XMLParser::GetValueByNum(const std::string &nodeName, int &nValue)
	{
		if (!mRootElem)
		{
			LOG4CXX_WARN(GETLOGGER("CWindXMLConfig::GetValueByNum"), "read root error!");
			return Failure;
		}

		TiXmlElement *pElement = mRootElem->FirstChildElement(nodeName.c_str());
		if (!pElement)
		{
			LOG4CXX_WARN(GETLOGGER("CWindXMLConfig::GetValueByNum"), "Read " << nodeName << " node error");
			return Failure;
		}
		pElement = pElement->FirstChildElement();
		if (!pElement)
		{
			LOG4CXX_WARN(GETLOGGER("CWindXMLConfig::GetValueByNum"), "Read " << nodeName << " node error");
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



	int XMLParser::GetAttributeByList(const std::string &Name, list<unsigned short> &lstImApp)
	{
		lstImApp.clear();
		if (!mRootElem)
		{
			LOG4CXX_WARN(GETLOGGER("CWindXMLConfig::GetAttributeByList"), "read root error!");

			return Failure;
		}

		TiXmlElement *pElement = mRootElem->FirstChildElement(Name.c_str());
		if (pElement == NULL)
		{
			LOG4CXX_WARN(GETLOGGER("CWindXMLConfig::GetAttributeByList"), "pElement is NULL");
			return Failure;
		}
		pElement = pElement->FirstChildElement();
		if (!pElement)
		{
			LOG4CXX_WARN(GETLOGGER("CWindXMLConfig::GetAttributeByList"), "pElement is NULL");
			return Failure;
		}
		for (; pElement != NULL; pElement = pElement->NextSiblingElement())
		{
			TiXmlAttribute *pAttribute = pElement->FirstAttribute();
			lstImApp.push_back(pAttribute->IntValue());

		}
		return Success;
	}

	int XMLParser::GetAttributeByList(const std::string &Name,list<string> &lstValues)
	{
		lstValues.clear();
		if (!mRootElem)
		{
			LOG4CXX_WARN(log4cxx::Logger::getLogger("CWindXMLConfig::GetAttributeByList"),"read root error!");

			return Failure;
		}

		TiXmlElement *pElement = mRootElem->FirstChildElement(Name.c_str());
		if (pElement == NULL)
		{
			LOG4CXX_WARN(log4cxx::Logger::getLogger("CWindXMLConfig::GetAttributeByList"),"pElement is NULL");
			return Failure;
		}
		pElement = pElement->FirstChildElement();
		if (!pElement)
		{
			LOG4CXX_WARN(log4cxx::Logger::getLogger("CWindXMLConfig::GetAttributeByList"),"pElement is NULL");
			return Failure;
		}
		for (; pElement != NULL; pElement = pElement->NextSiblingElement())
		{
			TiXmlAttribute *pAttribute = pElement->FirstAttribute();
			lstValues.push_back(pAttribute->Value());

		}
		return Success;
	}

	int XMLParser::GetAttributeByList(const std::string &Name, list<int> &lstImApp)
	{
		lstImApp.clear();
		if (!mRootElem)
		{
			LOG4CXX_WARN(GETLOGGER("CWindXMLConfig::GetAttributeByList"), "read root error!");

			return Failure;
		}
		TiXmlElement *pElement = mRootElem->FirstChildElement(Name.c_str());
		if (pElement == NULL)
		{
			LOG4CXX_WARN(GETLOGGER("CWindXMLConfig::GetAttributeByList"), "pElement is NULL");
			return Failure;
		}
		pElement = pElement->FirstChildElement();
		if (!pElement)
		{
			LOG4CXX_WARN(GETLOGGER("CWindXMLConfig::GetAttributeByList"), "pElement is NULL");
			return Failure;
		}
		for (; pElement != NULL; pElement = pElement->NextSiblingElement())
		{
			TiXmlAttribute *pAttribute = pElement->FirstAttribute();
			lstImApp.push_back(pAttribute->IntValue());

		}
		return Success;
	}

	int XMLParser::GetAttributeByMap(const std::string &Name, map<unsigned short, string> &mapIndicator)
	{
		mapIndicator.clear();
		if (!mRootElem)
		{
			LOG4CXX_WARN(GETLOGGER("CWindXMLConfig::GetAttributeByMap"), "read root error!");

			return Failure;
		}
		TiXmlElement *pElement = mRootElem->FirstChildElement(Name.c_str());
		if (pElement == NULL)
		{
			LOG4CXX_WARN(GETLOGGER("CWindXMLConfig::GetAttributeByMap"), "pElement is NULL");
			return Failure;
		}
		pElement = pElement->FirstChildElement();
		if (!pElement)
		{
			LOG4CXX_WARN(GETLOGGER("CWindXMLConfig::GetAttributeByMap"), "pElement is NULL");
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
			
		}
		return Success;
	}



	int XMLParser::GetAttributeByMap(const std::string &Name, map<int, int> &mapMemPool)
	{
		mapMemPool.clear();
		if (!mRootElem)
		{
			LOG4CXX_WARN(GETLOGGER("CWindXMLConfig::GetAttributeByMap"), "read root error!");

			return Failure;
		}
		TiXmlElement *pElement = mRootElem->FirstChildElement(Name.c_str());
		if (pElement == NULL)
		{
			LOG4CXX_WARN(GETLOGGER("CWindXMLConfig::GetAttributeByMap"), Name<<" pElement is NULL");
			return Failure;
		}
		pElement = pElement->FirstChildElement();
		if (!pElement)
		{
			LOG4CXX_WARN(GETLOGGER("CWindXMLConfig::GetAttributeByMap"), "pElement is NULL");
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

	int XMLParser::GetAttributeByMap(const std::string &Name, map<string, int> &mapSys)
	{
		if (!mRootElem)
		{
			LOG4CXX_WARN(GETLOGGER("CWindXMLConfig::GetAttributeByMap"), "read root error!");

			return Failure;
		}
		TiXmlElement *pElement = mRootElem->FirstChildElement(Name.c_str());
		if (pElement == NULL)
		{
			LOG4CXX_WARN(GETLOGGER("CWindXMLConfig::GetAttributeByMap"), Name<<" pElement is NULL");
			return Failure;
		}
		pElement = pElement->FirstChildElement();
		if (!pElement)
		{
			LOG4CXX_WARN(GETLOGGER("CWindXMLConfig::GetAttributeByMap"), "pElement is NULL");
			return Failure;
		}
		for (; pElement != NULL; pElement = pElement->NextSiblingElement())
		{
			TiXmlAttribute *pAttribute = pElement->FirstAttribute();
			if (!pAttribute)
			{
				return Failure;
			}
			string groupName = pAttribute->Value();

			pAttribute = pAttribute->Next();
			if (!pAttribute)
			{
				return Failure;
			}
			int nTotalSize = pAttribute->IntValue();

			mapSys.insert(std::make_pair(groupName, pAttribute->IntValue()));
		}
		return Success;
	}

#endif
}
