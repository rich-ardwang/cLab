#pragma once
#include <string>
#include "tinyxml.h"

class TiXmlDocument;
class TiXmlElement;
class TiXmlDeclaration;

class CXmlOperator
{
public:
    /*----------------------------------------------------------------------*/
    /*  Operation Type:
    /*      OperatorTypeNone£¬None type
    /*      OperatorTypeFirstChild£¬First child element
    /*      OperatorTypeNextChild£¬Next child element
    /*----------------------------------------------------------------------*/
    enum OperatorType
    {
        OperatorTypeNone = 0,
        OperatorTypeFirstChild,
        OperatorTypeNextChild,
    };

    // static functions
    static bool UTF8ToUnicode(const std::string& strUTF8, std::wstring* pStrUnicode);
    static bool UnicodeToUTF8(const std::wstring& strUnicode, std::string* pStrUTF8);
    static bool UnicodeToASCII(const std::wstring& strUnicode, std::string* pStrASCII);

public:
    CXmlOperator();
    ~CXmlOperator();

public:
    /*----------------------------------------------------------------------*/
    /*  Function:
    /*      set the xml declaration
    /*  Input Params:
    /*      strVersion£¬Version("1.0")
    /*      strEncoding£¬Econde("UTF-8")
    /*      strStandalone£¬("yes" or "no")
    /*  Return Params:
    /*      true£¬Success£»false£¬Failed
    /*----------------------------------------------------------------------*/
    bool SetDeclaration(const std::wstring& strVersion, const std::wstring& strEncoding,
        const std::wstring& strStandalone);

    /*----------------------------------------------------------------------*/
    /*  Function:
    /*      set the xml declaration
    /*  Input Params:
    /*      strVersion£¬Version("1.0")
    /*      strEncoding£¬Econde("UTF-8")
    /*      strStandalone£¬("yes" or "no")
    /*  Return Params:
    /*      true£¬Success£»false£¬Failed
    /*----------------------------------------------------------------------*/
    bool SetDeclaration(const std::string& strVersion, const std::string& strEncoding,
        const std::string& strStandalone);

    /*----------------------------------------------------------------------*/
    /*  Function:
    /*      get the xml declaration
    /*  Return Params:
    /*      the pointer to class TiXmlDeclaration
    /*----------------------------------------------------------------------*/
    TiXmlDeclaration* GetDeclaration();

    /*----------------------------------------------------------------------*/
    /*  Function:
    /*      add element to xml document
    /*  Input Params:
    /*      strName£¬The tag name of the element
    /*  Return Params:
    /*      true£¬Success£»false£¬Failed
    /*----------------------------------------------------------------------*/
    bool AddElement(const std::wstring& strName);

    /*----------------------------------------------------------------------*/
    /*  Function:
    /*      add element to xml document
    /*  Input Params:
    /*      strName£¬The tag name of the element
    /*  Return Params:
    /*      true£¬Success£»false£¬Failed
    /*----------------------------------------------------------------------*/
    bool AddElement(const std::string& strName);

    /*----------------------------------------------------------------------*/
    /*  Function:
    /*      add element to xml document
    /*  Input Params:
    /*      strName£¬The tag name of the element
    /*      strText£¬The value of the element
    /*  Return Params:
    /*      true£¬Success£»false£¬Failed
    /*----------------------------------------------------------------------*/
    bool AddElement(const std::wstring& strName, const std::wstring& strText);

    /*----------------------------------------------------------------------*/
    /*  Function:
    /*      add element to xml document
    /*  Input Params:
    /*      strName£¬The tag name of the element
    /*      strText£¬The value of the element
    /*  Return Params:
    /*      true£¬Success£»false£¬Failed
    /*----------------------------------------------------------------------*/
    bool AddElement(const std::string& strName, const std::string& strText);

    /*----------------------------------------------------------------------*/
    /*  Function:
    /*      find the element from xml document
    /*  Input Params:
    /*      strName£¬The tag name of the element
    /*  Return Params:
    /*      true£¬Success£»false£¬Failed
    /*----------------------------------------------------------------------*/
    bool FindElement(const std::wstring& strName) const;

    /*----------------------------------------------------------------------*/
    /*  Function:
    /*      find the element from xml document
    /*  Input Params:
    /*      strName£¬The tag name of the element
    /*  Return Params:
    /*      true£¬Success£»false£¬Failed
    /*----------------------------------------------------------------------*/
    bool FindElement(const std::string& strName) const;

    /*----------------------------------------------------------------------*/
    /*  Function:
    /*      get the value of the element
    /*  Return Params:
    /*      the value of the element
    /*----------------------------------------------------------------------*/
    std::wstring GetWText() const;

    /*----------------------------------------------------------------------*/
    /*  Function:
    /*      get the value of the element
    /*  Return Params:
    /*      the value of the element
    /*----------------------------------------------------------------------*/
    std::string GetText() const;

    /*----------------------------------------------------------------------*/
    /*  Function:
    /*      load xml document
    /*  Input Params:
    /*      strFileName£¬The path name of xml document
    /*  Return Params:
    /*      true£¬Success£»false£¬Failed
    /*----------------------------------------------------------------------*/
    bool Load(const std::wstring& strFileName);

    /*----------------------------------------------------------------------*/
    /*  Function:
    /*      load xml document
    /*  Input Params:
    /*      strFileName£¬The path name of xml document
    /*  Return Params:
    /*      true£¬Success£»false£¬Failed
    /*----------------------------------------------------------------------*/
    bool Load(const std::string& strFileName);

    /*----------------------------------------------------------------------*/
    /*  Function:
    /*      save xml document
    /*  Input Params:
    /*      strFileName£¬The path name of xml document
    /*  Return Params:
    /*      true£¬Success£»false£¬Failed
    /*----------------------------------------------------------------------*/
    bool Save(const std::wstring& strFileName);

    /*----------------------------------------------------------------------*/
    /*  Function:
    /*      save xml document
    /*  Input Params:
    /*      strFileName£¬The path name of xml document
    /*  Return Params:
    /*      true£¬Success£»false£¬Failed
    /*----------------------------------------------------------------------*/
    bool Save(const std::string& strFileName);

    /*----------------------------------------------------------------------*/
    /*  Function:
    /*      get access to element
    /*----------------------------------------------------------------------*/
    void Access();

    /*----------------------------------------------------------------------*/
    /*  Function:
    /*      leave the element
    /*----------------------------------------------------------------------*/
    void Leave();

    /*----------------------------------------------------------------------*/
    /*  Function:
    /*      output the contents of the xml doc
    /*----------------------------------------------------------------------*/
    inline void DumpXmlDoc()
    {
        if (m_pXmlDoc) { m_pXmlDoc->Print(); }
    }

private:
    // no copyable
    CXmlOperator(const CXmlOperator&);
    CXmlOperator& operator =(const CXmlOperator&);

private:
    mutable TiXmlElement     *m_pCurrentElement;   // the pointer to current element
    mutable TiXmlElement     *m_pChildElement;     // the pointer to child element
    TiXmlDocument            *m_pXmlDoc;           // the pointer to xml doc
    mutable OperatorType     m_emOptType;          // operation type
};
