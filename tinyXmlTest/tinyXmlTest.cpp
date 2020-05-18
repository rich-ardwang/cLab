// tinyXmlTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "tinyxml.h"
#include <Windows.h>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

#include "XmlOperator.h"

inline char* UnicodeToAnsi(const wchar_t* szStr) {
    int nLen = WideCharToMultiByte(CP_ACP, 0, szStr, -1, NULL, 0, NULL, NULL);
    if (nLen == 0) {
        return NULL;
    }
    char* pResult = new char[nLen];
    WideCharToMultiByte(CP_ACP, 0, szStr, -1, pResult, nLen, NULL, NULL);
    return pResult;
}

void printSchoolXml() {
    using namespace std;
    TiXmlDocument doc;
    const wchar_t* xmlWFile = L".\\中文\\school.xml";
    const char* xmlFile = UnicodeToAnsi(xmlWFile);
    if (doc.LoadFile(xmlFile)) {
        doc.Print();
    }
    else {
        std::cout << "can not parse xml : school.xml" << std::endl;
    }
}

void readSchoolXml() {
    using namespace std;
    const wchar_t* xmlWFile = L".\\中文\\school.xml";
    const char* xmlFile = UnicodeToAnsi(xmlWFile);
    TiXmlDocument doc;
    if (doc.LoadFile(xmlFile)) {
        doc.Print();
    }
    else {
        std::cout << "can not parse xml : school.xml" << std::endl;
        return;
    }

    TiXmlElement* rootElement = doc.RootElement();  //School元素  
    TiXmlElement* classElement = rootElement->FirstChildElement();  // Class元素
    TiXmlElement* studentElement = classElement->FirstChildElement();  //Students  

    for (; studentElement != NULL; studentElement = studentElement->NextSiblingElement()) {
        TiXmlAttribute* attributeOfStudent = studentElement->FirstAttribute();  //获得student的name属性  
        for (; attributeOfStudent != NULL; attributeOfStudent = attributeOfStudent->Next()) {
            std::cout << attributeOfStudent->Name() << " : " << attributeOfStudent->Value() << std::endl;
        }

        TiXmlElement* studentContactElement = studentElement->FirstChildElement();//获得student的第一个联系方式 

        for (; studentContactElement != NULL; studentContactElement = studentContactElement->NextSiblingElement()) {
            std::string contactType = studentContactElement->Value();
            const char *next = studentContactElement->GetText();
            std::string contactValue;
            if (next)
            {
                contactValue = next;
                studentContactElement->ReplaceChild(studentContactElement->LastChild(), TiXmlText("KKK@163.com"));
            }
            else
            {
                studentContactElement->LinkEndChild(new TiXmlText("nnn@163.com"));
            }
            std::cout << contactType << " : " << contactValue << std::endl;
        }
    }
    doc.SaveFile(xmlFile);
}

void writeSchoolXml() {
    using namespace std;
    const char * xmlFile = "cschool-write.xml";

    TiXmlDocument doc;
    TiXmlDeclaration * decl = new TiXmlDeclaration("1.0", "", "");
    TiXmlElement * schoolElement = new TiXmlElement("School");
    TiXmlElement * classElement = new TiXmlElement("Class");
    classElement->SetAttribute("name", "C++");

    TiXmlElement * stu1Element = new TiXmlElement("Student");
    stu1Element->SetAttribute("name", "tinyxml");
    stu1Element->SetAttribute("number", "123");
    TiXmlElement * stu1EmailElement = new TiXmlElement("email");
    stu1EmailElement->LinkEndChild(new TiXmlText("tinyxml@163.com"));
    TiXmlElement * stu1AddressElement = new TiXmlElement("address");
    stu1AddressElement->LinkEndChild(new TiXmlText("中国"));
    stu1Element->LinkEndChild(stu1EmailElement);
    stu1Element->LinkEndChild(stu1AddressElement);

    TiXmlElement * stu2Element = new TiXmlElement("Student");
    stu2Element->SetAttribute("name", "jsoncpp");
    stu2Element->SetAttribute("number", "456");
    TiXmlElement * stu2EmailElement = new TiXmlElement("email");
    stu2EmailElement->LinkEndChild(new TiXmlText("jsoncpp@163.com"));
    TiXmlElement * stu2AddressElement = new TiXmlElement("address");
    stu2AddressElement->LinkEndChild(new TiXmlText("美国"));
    stu2Element->LinkEndChild(stu2EmailElement);
    stu2Element->LinkEndChild(stu2AddressElement);

    classElement->LinkEndChild(stu1Element);
    classElement->LinkEndChild(stu2Element);
    schoolElement->LinkEndChild(classElement);

    doc.LinkEndChild(decl);
    doc.LinkEndChild(schoolElement);
    doc.SaveFile(xmlFile);
}

void testXmlOperatorW() {
    CXmlOperator xml;
    std::wstring name;
    std::wstring val;

    xml.SetDeclaration(L"1.0", L"UTF-8", L"");

    name = L"T1";
    xml.AddElement(name);

    xml.Access();
    name = L"T1-1";
    xml.AddElement(name);
    name = L"T1-2";
    xml.AddElement(name);
    name = L"T1-3";
    xml.AddElement(name);
    name = L"T1-4";
    xml.AddElement(name);
    name = L"T1-5";
    xml.AddElement(name);

    xml.Access();
    name = L"T1-5-1";
    val = L"1";
    xml.AddElement(name, val);
    name = L"T1-5-2";
    val = L"2";
    xml.AddElement(name, val);
    name = L"T1-5-3";
    val = L"3";
    xml.AddElement(name, val);
    name = L"T1-5-4";
    val = L"4";
    xml.AddElement(name, val);
    name = L"T1-5-5";
    val = L"5";
    xml.AddElement(name, val);
    xml.Leave();

    name = L"T1-6";
    xml.AddElement(name);
    name = L"T1-7";
    xml.AddElement(name);
    xml.Leave();

    name = L"T2";
    xml.AddElement(name);

    xml.Access();
    name = L"T2-1";
    xml.AddElement(name);
    name = L"T2-2";
    xml.AddElement(name);
    name = L"T2-3";
    xml.AddElement(name);
    name = L"T2-4";
    xml.AddElement(name);
    name = L"T2-5";
    xml.AddElement(name);
    xml.Leave();

    name = L"T3";
    xml.AddElement(name);
    name = L"T4";
    xml.AddElement(name);
    name = L"T5";
    xml.AddElement(name);

    xml.Access();
    name = L"T5-1";
    xml.AddElement(name);
    name = L"T5-2";
    xml.AddElement(name);
    name = L"T5-3";
    xml.AddElement(name);
    name = L"T5-4";
    xml.AddElement(name);
    name = L"T5-5";
    xml.AddElement(name);
    xml.Leave();

    name = L"T6";
    xml.AddElement(name);

    bool bRet = false;
    xml.Save(L"test.xml");

    xml.Load(L"test.xml");
    xml.DumpXmlDoc();

    name = L"T1";
    bRet = xml.FindElement(name);
    if (bRet)
    {
        std::string value;
        xml.UnicodeToUTF8(xml.GetWText(), &value);
        std::cout << "T1:" << value << std::endl;
    }
    else
    {
        std::string key;
        xml.UnicodeToUTF8(name, &key);
        std::cout << "not find " << key << "!" << std::endl;
    }
    xml.Access();

    name = L"T1-1";
    bRet = xml.FindElement(name);
    if (bRet)
    {
        std::string value;
        xml.UnicodeToUTF8(xml.GetWText(), &value);
        std::cout << "T1-1:" << value << std::endl;
    }
    else
    {
        std::string key;
        xml.UnicodeToUTF8(name, &key);
        std::cout << "not find " << key << "!" << std::endl;
    }

    name = L"T1-5";
    bRet = xml.FindElement(name);
    if (bRet)
    {
        std::string value;
        xml.UnicodeToUTF8(xml.GetWText(), &value);
        std::cout << "T1-5:" << value << std::endl;
    }
    else
    {
        std::string key;
        xml.UnicodeToUTF8(name, &key);
        std::cout << "not find " << key << "!" << std::endl;
    }

    xml.Access();
    name = L"T1-5-1";
    bRet = xml.FindElement(name);
    if (bRet)
    {
        std::string value;
        xml.UnicodeToUTF8(xml.GetWText(), &value);
        std::cout << "T1-5-1:" << value << std::endl;
    }
    else
    {
        std::string key;
        xml.UnicodeToUTF8(name, &key);
        std::cout << "not find " << key << "!" << std::endl;
    }
    xml.Leave();
    xml.Leave();

    name = L"T2";
    bRet = xml.FindElement(name);
    if (bRet)
    {
        std::string value;
        xml.UnicodeToUTF8(xml.GetWText(), &value);
        std::cout << "T2:" << value << std::endl;
    }
    else
    {
        std::string key;
        xml.UnicodeToUTF8(name, &key);
        std::cout << "not find " << key << "!" << std::endl;
    }

    xml.Access();
    name = L"T2-1";
    bRet = xml.FindElement(name);
    if (bRet)
    {
        std::string value;
        xml.UnicodeToUTF8(xml.GetWText(), &value);
        std::cout << "T2-1:" << value << std::endl;
    }
    else
    {
        std::string key;
        xml.UnicodeToUTF8(name, &key);
        std::cout << "not find " << key << "!" << std::endl;
    }
    name = L"T2-5";
    bRet = xml.FindElement(name);
    if (bRet)
    {
        std::string value;
        xml.UnicodeToUTF8(xml.GetWText(), &value);
        std::cout << "T2-5:" << value << std::endl;
    }
    else
    {
        std::string key;
        xml.UnicodeToUTF8(name, &key);
        std::cout << "not find " << key << "!" << std::endl;
    }
    xml.Leave();

    name = L"T3";
    bRet = xml.FindElement(name);
    if (bRet)
    {
        std::string value;
        xml.UnicodeToUTF8(xml.GetWText(), &value);
        std::cout << "T3:" << value << std::endl;
    }
    else
    {
        std::string key;
        xml.UnicodeToUTF8(name, &key);
        std::cout << "not find " << key << "!" << std::endl;
    }

    name = L"T5";
    bRet = xml.FindElement(name);
    if (bRet)
    {
        std::string value;
        xml.UnicodeToUTF8(xml.GetWText(), &value);
        std::cout << "T5:" << value << std::endl;
    }
    else
    {
        std::string key;
        xml.UnicodeToUTF8(name, &key);
        std::cout << "not find " << key << "!" << std::endl;
    }

    xml.Access();
    name = L"T5-1";
    bRet = xml.FindElement(name);
    if (bRet)
    {
        std::string value;
        xml.UnicodeToUTF8(xml.GetWText(), &value);
        std::cout << "T5-1:" << value << std::endl;
    }
    else
    {
        std::string key;
        xml.UnicodeToUTF8(name, &key);
        std::cout << "not find " << key << "!" << std::endl;
    }
    name = L"T5-2";
    bRet = xml.FindElement(name);
    if (bRet)
    {
        std::string value;
        xml.UnicodeToUTF8(xml.GetWText(), &value);
        std::cout << "T5-2:" << value << std::endl;
    }
    else
    {
        std::string key;
        xml.UnicodeToUTF8(name, &key);
        std::cout << "not find " << key << "!" << std::endl;
    }
    name = L"T5-3";
    bRet = xml.FindElement(name);
    if (bRet)
    {
        std::string value;
        xml.UnicodeToUTF8(xml.GetWText(), &value);
        std::cout << "T5-3:" << value << std::endl;
    }
    else
    {
        std::string key;
        xml.UnicodeToUTF8(name, &key);
        std::cout << "not find " << key << "!" << std::endl;
    }
    xml.Access();
    name = L"abc";
    val = L"abc";
    bRet = xml.AddElement(name, val);
    bRet = xml.AddElement(name, val);
    xml.Leave();
    xml.Leave();

    xml.Save(L"test.xml");
    xml.Load(L"test.xml");
    xml.DumpXmlDoc();
}

void testXmlOperator() {
    CXmlOperator xml;
    std::string name;
    std::string val;

    xml.SetDeclaration(L"1.0", L"UTF-8", L"");

    name = "T1";
    xml.AddElement(name);

    xml.Access();
    name = "T1-1";
    xml.AddElement(name);
    name = "T1-2";
    xml.AddElement(name);
    name = "T1-3";
    xml.AddElement(name);
    name = "T1-4";
    xml.AddElement(name);
    name = "T1-5";
    xml.AddElement(name);

    xml.Access();
    name = "T1-5-1";
    val = "1";
    xml.AddElement(name, val);
    name = "T1-5-2";
    val = "2";
    xml.AddElement(name, val);
    name = "T1-5-3";
    val = "3";
    xml.AddElement(name, val);
    name = "T1-5-4";
    val = "4";
    xml.AddElement(name, val);
    name = "T1-5-5";
    val = "5";
    xml.AddElement(name, val);
    xml.Leave();

    name = "T1-6";
    xml.AddElement(name);
    name = "T1-7";
    xml.AddElement(name);
    xml.Leave();

    name = "T2";
    xml.AddElement(name);

    xml.Access();
    name = "T2-1";
    xml.AddElement(name);
    name = "T2-2";
    xml.AddElement(name);
    name = "T2-3";
    xml.AddElement(name);
    name = "T2-4";
    xml.AddElement(name);
    name = "T2-5";
    xml.AddElement(name);
    xml.Leave();

    name = "T3";
    xml.AddElement(name);
    name = "T4";
    xml.AddElement(name);
    name = "T5";
    xml.AddElement(name);

    xml.Access();
    name = "T5-1";
    xml.AddElement(name);
    name = "T5-2";
    xml.AddElement(name);
    name = "T5-3";
    xml.AddElement(name);
    name = "T5-4";
    xml.AddElement(name);
    name = "T5-5";
    xml.AddElement(name);
    xml.Leave();

    name = "T6";
    xml.AddElement(name);

    bool bRet = false;
    xml.Save("test.xml");

    xml.Load("test.xml");
    xml.DumpXmlDoc();

    name = "T1";
    bRet = xml.FindElement(name);
    if (bRet)
        std::cout << name << ":" << xml.GetText() << std::endl;
    else
        std::cout << "not find " << name << "!" << std::endl;
    xml.Access();

    name = "T1-1";
    bRet = xml.FindElement(name);
    if (bRet)
        std::cout << name << ":" << xml.GetText() << std::endl;
    else
        std::cout << "not find " << name << "!" << std::endl;

    name = "T1-5";
    bRet = xml.FindElement(name);
    if (bRet)
        std::cout << name << ":" << xml.GetText() << std::endl;
    else
        std::cout << "not find " << name << "!" << std::endl;

    xml.Access();
    name = "T1-5-1";
    bRet = xml.FindElement(name);
    if (bRet)
        std::cout << name << ":" << xml.GetText() << std::endl;
    else
        std::cout << "not find " << name << "!" << std::endl;
    xml.Leave();
    xml.Leave();

    name = "T2";
    bRet = xml.FindElement(name);
    if (bRet)
        std::cout << name << ":" << xml.GetText() << std::endl;
    else
        std::cout << "not find " << name << "!" << std::endl;

    xml.Access();
    name = "T2-1";
    bRet = xml.FindElement(name);
    if (bRet)
        std::cout << name << ":" << xml.GetText() << std::endl;
    else
        std::cout << "not find " << name << "!" << std::endl;
    name = "T2-5";
    bRet = xml.FindElement(name);
    if (bRet)
        std::cout << name << ":" << xml.GetText() << std::endl;
    else
        std::cout << "not find " << name << "!" << std::endl;
    xml.Leave();

    name = "T3";
    bRet = xml.FindElement(name);
    if (bRet)
        std::cout << name << ":" << xml.GetText() << std::endl;
    else
        std::cout << "not find " << name << "!" << std::endl;
    name = "T5";
    bRet = xml.FindElement(name);
    if (bRet)
        std::cout << name << ":" << xml.GetText() << std::endl;
    else
        std::cout << "not find " << name << "!" << std::endl;

    xml.Access();
    name = "T5-1";
    bRet = xml.FindElement(name);
    if (bRet)
        std::cout << name << ":" << xml.GetText() << std::endl;
    else
        std::cout << "not find " << name << "!" << std::endl;
    name = "T5-2";
    bRet = xml.FindElement(name);
    if (bRet)
        std::cout << name << ":" << xml.GetText() << std::endl;
    else
        std::cout << "not find " << name << "!" << std::endl;
    name = "T5-3";
    bRet = xml.FindElement(name);
    if (bRet)
        std::cout << name << ":" << xml.GetText() << std::endl;
    else
        std::cout << "not find " << name << "!" << std::endl;
    xml.Access();
    name = "abc";
    val = "abc";
    bRet = xml.AddElement(name, val);
    bRet = xml.AddElement(name, val);
    xml.Leave();
    xml.Leave();

    xml.Save("test.xml");
    xml.Load("test.xml");
    xml.DumpXmlDoc();
}

int _tmain(int argc, _TCHAR* argv[])
{
    //printSchoolXml();
    //readSchoolXml();
    //writeSchoolXml();

    testXmlOperator();

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

	return 0;
}

