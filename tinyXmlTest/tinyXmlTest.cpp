// tinyXmlTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "tinyxml.h"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

void printSchoolXml() {
    using namespace std;
    TiXmlDocument doc;
    const char * xmlFile = "school.xml";
    if (doc.LoadFile(xmlFile)) {
        doc.Print();
    }
    else {
        std::cout << "can not parse xml : school.xml" << std::endl;
    }
}

void readSchoolXml() {
    using namespace std;
    const char * xmlFile = "school.xml";
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

int _tmain(int argc, _TCHAR* argv[])
{
    //printSchoolXml();
    readSchoolXml();
    //writeSchoolXml();

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

	return 0;
}

