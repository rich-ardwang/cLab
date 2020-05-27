#include "XmlOperator.h"
#include <windows.h>

bool CXmlOperator::UTF8ToUnicode(const std::string& strUTF8, std::wstring* pStrUnicode)
{
    if (strUTF8.empty() || !pStrUnicode)
        return false;

    int nLength = MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, NULL, 0);
    int nWideLength = nLength - 1;

    pStrUnicode->resize(nWideLength, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, &pStrUnicode->at(0), nWideLength);

    return true;
}

bool CXmlOperator::UnicodeToUTF8(const std::wstring& strUnicode, std::string* pStrUTF8)
{
    if (strUnicode.empty() || !pStrUTF8)
        return false;

    int nLength = WideCharToMultiByte(CP_UTF8, 0, strUnicode.c_str(), -1, NULL, 0, NULL, NULL);

    pStrUTF8->resize(nLength - 1, '\0');
    WideCharToMultiByte(CP_UTF8, 0, strUnicode.c_str(), -1, &pStrUTF8->at(0), nLength - 1, NULL, NULL);

    return true;
}

bool CXmlOperator::UnicodeToASCII(const std::wstring& strUnicode, std::string* pStrASCII)
{
    if (strUnicode.empty() || !pStrASCII)
        return false;

    int nLength = WideCharToMultiByte(CP_ACP, 0, strUnicode.c_str(), -1, NULL, 0, NULL, NULL);

    pStrASCII->resize(nLength - 1, '\0');
    WideCharToMultiByte(CP_ACP, 0, strUnicode.c_str(), -1, &pStrASCII->at(0), nLength - 1, NULL, NULL);

    return true;
}

CXmlOperator::CXmlOperator() : m_pCurrentElement(nullptr),
                               m_pChildElement(nullptr),
                               m_pXmlDoc(nullptr)
{
    m_pXmlDoc = new TiXmlDocument;
}

CXmlOperator::~CXmlOperator()
{
    if (m_pXmlDoc)
    {
        delete m_pXmlDoc;
        m_pXmlDoc = nullptr;
    }
}

bool CXmlOperator::SetDeclaration(const std::wstring& strVersion, const std::wstring& strEncoding,
    const std::wstring& strStandalone)
{
    bool bRet = false;

    try
    {
        if (!m_pXmlDoc)
        {
            bRet = false;
            throw bRet;
        }

        TiXmlNode *pNode = nullptr;
        TiXmlDeclaration *pDecl = nullptr;

        pNode = m_pXmlDoc->FirstChild();
        if (pNode)
        {
            pDecl = pNode->ToDeclaration();
        }

        std::string version;
        std::string encoding;
        std::string standalone;

        UnicodeToUTF8(strVersion, &version);
        UnicodeToUTF8(strEncoding, &encoding);
        UnicodeToUTF8(strStandalone, &standalone);

        TiXmlDeclaration decl(version, encoding, standalone);
        if (pDecl)
        {
            *pDecl = decl;
        }
        else
        {
            if (pNode)
            {
                m_pXmlDoc->InsertBeforeChild(pNode, decl);
            }
            else
            {
                pDecl = new TiXmlDeclaration(decl);
                m_pXmlDoc->LinkEndChild(pDecl);
            }
        }
        bRet = true;
    }
    catch (bool)
    {
    }

    return bRet;
}

bool CXmlOperator::SetDeclaration(const std::string& strVersion, const std::string& strEncoding,
    const std::string& strStandalone)
{
    bool bRet = false;

    try
    {
        if (!m_pXmlDoc)
        {
            bRet = false;
            throw bRet;
        }

        TiXmlNode *pNode = nullptr;
        TiXmlDeclaration *pDecl = nullptr;

        pNode = m_pXmlDoc->FirstChild();
        if (pNode)
        {
            pDecl = pNode->ToDeclaration();
        }

        TiXmlDeclaration decl(strVersion, strEncoding, strStandalone);
        if (pDecl)
        {
            *pDecl = decl;
        }
        else
        {
            if (pNode)
            {
                m_pXmlDoc->InsertBeforeChild(pNode, decl);
            }
            else
            {
                pDecl = new TiXmlDeclaration(decl);
                m_pXmlDoc->LinkEndChild(pDecl);
            }
        }
        bRet = true;
    }
    catch (bool)
    {
    }

    return bRet;
}

TiXmlDeclaration* CXmlOperator::GetDeclaration()
{
    TiXmlDeclaration *pDecl = nullptr;
    if (m_pXmlDoc)
    {
        TiXmlElement *pRoot = m_pXmlDoc->RootElement();
        if (pRoot)
        {
            pDecl = pRoot->ToDeclaration();
        }
    }
    return pDecl;
}

bool CXmlOperator::AddElement(const std::wstring& strName)
{
    bool bRet = false;

    try
    {
        if (strName.empty() || !m_pXmlDoc || !isalpha(strName[0]))
        {
            bRet = false;
            throw bRet;
        }

        std::string strUTF8;
        TiXmlElement *pElem = nullptr;

        UnicodeToUTF8(strName, &strUTF8);
        pElem = new TiXmlElement(strUTF8);
        if (m_pCurrentElement)
        {
            m_pCurrentElement->LinkEndChild(pElem);
        }
        else if (m_pXmlDoc)
        {
            m_pXmlDoc->LinkEndChild(pElem);
        }
        else
        {
            bRet = false;
            throw bRet;
        }
        m_pChildElement = pElem;
        m_emOptType = OperatorTypeNextChild;
        bRet = true;
    }
    catch (bool)
    {
    }

    return bRet;
}

bool CXmlOperator::AddElement(const std::string& strName)
{
    bool bRet = false;

    try
    {
        if (strName.empty() || !m_pXmlDoc || !isalpha(strName[0]))
        {
            bRet = false;
            throw bRet;
        }

        TiXmlElement *pElem = new TiXmlElement(strName);
        if (m_pCurrentElement)
        {
            m_pCurrentElement->LinkEndChild(pElem);
        }
        else if (m_pXmlDoc)
        {
            m_pXmlDoc->LinkEndChild(pElem);
        }
        else
        {
            bRet = false;
            throw bRet;
        }
        m_pChildElement = pElem;
        m_emOptType = OperatorTypeNextChild;
        bRet = true;
    }
    catch (bool)
    {
    }

    return bRet;
}

bool CXmlOperator::AddElement(const std::wstring& strName, const std::wstring& strText)
{
    bool bRet = AddElement(strName);
    if (bRet)
    {
        std::string strUTF8;
        TiXmlText *pText = nullptr;

        UnicodeToUTF8(strText, &strUTF8);
        pText = new TiXmlText(strUTF8);
        if (m_pChildElement)
        {
            m_pChildElement->LinkEndChild(pText);
        }
        else
            bRet = false;

        m_emOptType = OperatorTypeNextChild;
    }
    return bRet;
}

bool CXmlOperator::AddElement(const std::string& strName, const std::string& strText)
{
    bool bRet = AddElement(strName);
    if (bRet)
    {
        TiXmlText *pText = new TiXmlText(strText);
        if (m_pChildElement)
        {
            m_pChildElement->LinkEndChild(pText);
        }
        else
            bRet = false;

        m_emOptType = OperatorTypeNextChild;
    }
    return bRet;
}

bool CXmlOperator::FindElement(const std::wstring& strName) const
{
    bool bRet = false;

    try
    {
        if (strName.empty() || !m_pXmlDoc || !isalpha(strName[0]))
        {
            bRet = false;
            throw bRet;
        }

        std::string strUTF8;

        UnicodeToUTF8(strName, &strUTF8);

        if (m_emOptType == OperatorTypeFirstChild)
        {
            if (m_pCurrentElement)
            {
                m_pChildElement = m_pCurrentElement->FirstChildElement(strUTF8);
            }
            else
            {
                m_pChildElement = m_pXmlDoc->FirstChildElement(strUTF8);
            }
            if (m_pChildElement)
            {
                bRet = true;
                m_emOptType = OperatorTypeNextChild;
            }
        }
        // find the next child element
        else if (m_emOptType == OperatorTypeNextChild)
        {
            if (m_pChildElement)
            {
                m_pChildElement = m_pChildElement->NextSiblingElement(strUTF8);
            }
            if (m_pChildElement)
            {
                bRet = true;
            }
        }
    }
    catch (bool)
    {
    }

    return bRet;
}

bool CXmlOperator::FindElement(const std::string& strName) const
{
    bool bRet = false;

    try
    {
        if (strName.empty() || !m_pXmlDoc || !isalpha(strName[0]))
        {
            bRet = false;
            throw bRet;
        }

        if (m_emOptType == OperatorTypeFirstChild)
        {
            if (m_pCurrentElement)
            {
                m_pChildElement = m_pCurrentElement->FirstChildElement(strName);
            }
            else
            {
                m_pChildElement = m_pXmlDoc->FirstChildElement(strName);
            }
            if (m_pChildElement)
            {
                bRet = true;
                m_emOptType = OperatorTypeNextChild;
            }
        }
        // find the next child element
        else if (m_emOptType == OperatorTypeNextChild)
        {
            if (m_pChildElement)
            {
                m_pChildElement = m_pChildElement->NextSiblingElement(strName);
            }
            if (m_pChildElement)
            {
                bRet = true;
            }
        }
    }
    catch (bool)
    {
    }

    return bRet;
}

std::wstring CXmlOperator::GetWText() const
{
    std::wstring strText;
    const char *pszText = nullptr;

    if (m_pChildElement)
    {
        pszText = m_pChildElement->GetText();
        if (pszText)
        {
            UTF8ToUnicode(pszText, &strText);
        }
    }

    return strText;
}

std::string CXmlOperator::GetText() const
{
    std::string strText;
    const char *pszText = nullptr;

    if (m_pChildElement)
    {
        pszText = m_pChildElement->GetText();
        if (pszText)
        {
            std::string tmp(pszText);
            strText = tmp;
        }
    }

    return strText;
}

bool CXmlOperator::Load(const std::wstring& strFileName)
{
    bool bRet = false;

    try
    {
        if (strFileName.empty())
        {
            bRet = false;
            throw bRet;
        }

        std::string name;

        UnicodeToUTF8(strFileName, &name);
        if (m_pXmlDoc)
        {
            delete m_pXmlDoc;
            m_pXmlDoc = nullptr;
        }
        m_pXmlDoc = new TiXmlDocument(name);
        bRet = m_pXmlDoc->LoadFile();
        m_emOptType = OperatorTypeFirstChild;
        m_pCurrentElement = nullptr;
        m_pChildElement = nullptr;
    }
    catch (bool)
    {
    }

    return bRet;
}

bool CXmlOperator::Load(const std::string& strFileName)
{
    bool bRet = false;

    try
    {
        if (strFileName.empty())
        {
            bRet = false;
            throw bRet;
        }

        if (m_pXmlDoc)
        {
            delete m_pXmlDoc;
            m_pXmlDoc = nullptr;
        }
        m_pXmlDoc = new TiXmlDocument(strFileName);
        bRet = m_pXmlDoc->LoadFile();
        m_emOptType = OperatorTypeFirstChild;
        m_pCurrentElement = nullptr;
        m_pChildElement = nullptr;
    }
    catch (bool)
    {
    }

    return bRet;
}

bool CXmlOperator::Save(const std::wstring& strFileName)
{
    bool bRet = false;

    try
    {
        if (strFileName.empty() || !m_pXmlDoc)
        {
            bRet = false;
            throw bRet;
        }

        std::string name;
        TiXmlDeclaration *pDecl = GetDeclaration();

        if (!pDecl)
        {
            SetDeclaration(L"1.0", L"UTF-8", L"");
        }
        UnicodeToUTF8(strFileName, &name);
        bRet = m_pXmlDoc->SaveFile(name);
    }
    catch (bool)
    {
    }

    return bRet;
}

bool CXmlOperator::Save(const std::string& strFileName)
{
    bool bRet = false;

    try
    {
        if (strFileName.empty() || !m_pXmlDoc)
        {
            bRet = false;
            throw bRet;
        }

        TiXmlDeclaration *pDecl = GetDeclaration();
        if (!pDecl)
        {
            SetDeclaration(L"1.0", L"UTF-8", L"");
        }
        bRet = m_pXmlDoc->SaveFile(strFileName);
    }
    catch (bool)
    {
    }

    return bRet;
}

void CXmlOperator::Access()
{
    m_pCurrentElement = m_pChildElement;
    m_pChildElement = nullptr;
    m_emOptType = OperatorTypeFirstChild;
}

void CXmlOperator::Leave()
{
    if (m_pCurrentElement)
    {
        TiXmlNode *pNode = m_pCurrentElement->Parent();

        m_pChildElement = m_pCurrentElement;
        if (pNode)
        {
            m_pCurrentElement = pNode->ToElement();
        }
        else
        {
            m_pCurrentElement = nullptr;
        }

        m_emOptType = OperatorTypeNextChild;
    }
}
