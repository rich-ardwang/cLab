/*
* =====================================================================================
*
*       Filename:  xmlparser.h
*    Description:
*				   用来解析xml配置文件（陈万鸣：2012-7-10）
*        Version:  1.0
*        Created:  2012-7-10
*       Revision:  丁富来修改合入Common中@2015-4-13
*       Compiler:  gcc
*
*         Author:  chenwanming
*        Company:  wind, Inc.
*
* =====================================================================================
*/
#ifndef _WIND_XML_PARSER_H_
#define _WIND_XML_PARSER_H_

#ifndef _WIN32
#ifndef __linux__
#error "Unknown os platform, _WIN32 or __linux__ not defined."
#endif
#endif

#include <string>
#include <iostream>
#include <list>
#include <vector>
#include <map>
#include "xmlParser/tinyxml.h"

using namespace std;

namespace windcom
{
	class XMLParser
	{
	public:
		explicit XMLParser(const char *file);
		virtual ~XMLParser(){}

	public:
		inline bool Good() const { return NULL != mRootElem;}

	public:
		/*
		 * 获取根节点的子节点 
		 * <Root> <Name> </Name> </Root>
		 */
		TiXmlElement* GetSubNode(const string& strName);

		/*
		 * 获取Node节点下一级节点(Name)的值(Value)
		 * <Node>
		 *    <Name> Value </Name>
		 * </Node>
		 */
		bool GetValue(TiXmlElement* pNode, const string& strName, string& strValue);

		/*
		 * 获取Node节点下一级节点(Name)的第一个属性值(Value)
		 * <Node>
		 *     <Name Attr1="Value" />
		 * </Node>
		 */
		bool GetAttribute(TiXmlElement* pNode, const string &strName, string& strValue);

		/*
		 * 获取Node节点下一级节点(Name)的所有属性值
		 * <Node>
		 *     <Name Attr1="Value1" Attr2="Value2" />
		 * </Node>
		 */
		bool GetAttribute(TiXmlElement* pNode, const string& strName, vector<string>& oValueList);

		/* 
		 * 获取Node节点下一级节点(Name)的所有子节点(Sub)的所有属性值
		 * <Node>
		 *     <Name>
		 *         <Sub Attr1="Value1" Attr2="Value2" />
		 *         <Sub Attr1="Value1" Attr2="Value2" />
		 *     </Name>
		 * </Node>
		 */
		bool GetAttribute(TiXmlElement* pNode, const string& strName, const string& strSub, vector<vector<string> >& oAttributeMatrix);

		// 根据标签名和属性名查询
		bool GetAttribute(TiXmlElement* pNode, const string& strName, const string& strAttr, string & value);
		bool GetAttribute(TiXmlElement* pNode, const string& strName, const string& strAttr, int & value);


		// 以下接口中标签为二级标签，即为Root的下一级
		//////////////////////////////////////////////////////////////////////////
		bool GetValue(const string& strName, string& strValue);
		bool GetAttribute(const string &Name, string& strValue);
		bool GetAttribute(const string& strName, vector<string>& oAttributeList);
		bool GetAttribute(const string& strName, const string& strSub, vector<vector<string> >& oAttributeMatrix);

		// 根据标签名和属性名查询
		bool GetAttribute(const string& strName, const string& strAttr, string & value);
		bool GetAttribute(const string& strName, const string& strAttr, int & value);

		//////////////////////////////////////////////////////////////////////////
		/*int GetAttributeByList(const std::string &Name, list<string> &lstValues);
		int GetAttributeByList(const std::string &Name, list<unsigned short> &lstImApp);
		int GetAttributeByList(const std::string &Name, list<int> &lstImApp);
		int GetAttributeByMap(const std::string &Name, map<unsigned short, string> &mapIndicator);
		int GetAttributeByMap(const std::string &Name, map<int, int> &mapMemPool);
		int GetAttributeByMap(const std::string &Name, map<string, int> &mapSys);*/

		//int GetValueByNum(const std::string &nodeName, int &nValue);
		//void StringToUpper(string &strValue);

	private:
		TiXmlDocument    mXmlConfigFile;
		TiXmlElement    *mRootElem;
	};

}

#endif

