#include "common/strex.h"
#include "common/mempool.h"
#include <iomanip>
#include <cstring>
#include <sstream>

namespace windcom
{
    bool Str2Short(const std::string& str, short& value, int base)
    {
        short ret;
        std::stringstream s(str);
        s>>ret;
        if(s.fail())
            return false;
        return true;
    }

    int StrCmp(const char* src, const char* dst)
    {
        return strcmp(src, dst);
    }

    int StrCmp(const std::string& src, const std::string& dst)
    {
        return strcmp(src.c_str(), dst.c_str());
    }

    int StrCmpNoCase(const char* src, const char* dst)
    {
        return StrCmpNoCase(std::string(src), std::string(dst));
    }

    int StrCmpNoCase(const std::string& src, const std::string& dst)
    {
#ifdef _WIN32
        return _stricmp(src.c_str(), dst.c_str());
#else
        unsigned int srcLen = src.length();
        unsigned int dstLen = dst.length();

        for (unsigned int i=0; i<srcLen && i<dstLen; i++)
        {
            if (tolower(src[i]) != tolower(dst[i]))
            {
                if (src[i] > dst[i])
                {
                    return 1;
                }
                else
                {
                    return -1;
                }
            }
        }
        if (srcLen>dstLen)
        {
            return 1;
        }
        else if (srcLen < dstLen)
        {
            return -1;
        }
        else
        {
            return 0;
        }
#endif
    }


    void Split(const std::string &str, char delim, std::vector<std::string> &elems)
    {
        elems.clear();
        
        std::stringstream ss(str);
        std::string item;
        while (std::getline(ss, item, delim))
        {
            if (!item.empty())//²»°üº¬¿ÕÖµ
                elems.push_back(item);
        }
    }

    void Split(const std::string& src, const std::string& separator, std::vector<std::string>& dest)
    {
        dest.clear();
        
        if (separator.empty())
        {
            dest.push_back(src);
            return;
        }

        std::string str = src;
        std::string substring;
        std::string::size_type start = 0, index;

        do
        {
            index = str.find_first_of(separator,start);
            if (index != std::string::npos)
            {    
                substring = str.substr(start,index-start);
                dest.push_back(substring);
                start = str.find_first_not_of(separator,index);
                if (start == std::string::npos) return;
            }
        }while(index != std::string::npos);

        //the last token
        substring = str.substr(start);
        dest.push_back(substring);
    }

    void SplitAbso(const std::string& src, const std::string& separator, std::vector<std::string>& dest)
    {
        dest.clear();

        if (src.empty())
            return;

        if (separator.empty())
        {
            dest.push_back(src);
            return;
        }

        size_t nFirstPos = src.find_first_of(separator);
        size_t nLastPos = 0;

        if (nFirstPos == std::string::npos)
        {
            dest.push_back(src);
            return;
        }

        while(nFirstPos != std::string::npos)
        {
            dest.push_back(src.substr(nLastPos, nFirstPos - nLastPos));
            nLastPos = nFirstPos + 1;
            nFirstPos = src.find_first_of(separator, nLastPos);
        }

        dest.push_back(src.substr(nLastPos));       
    }

    int Split(const std::string &str, char delim, std::string& first, std::string& second)
    {
        size_t index = str.find_first_of(delim);
        if (index == std::string::npos)
        {
            return -1;
        }
        first = str.substr(0, index);
        second = str.substr(index+1);

        return 0;
    }


	std::string Between(const std::string &str, char left, char right)
	{
		std::string::size_type pos1 = str.find_first_of(left);
		std::string::size_type pos2 = str.find_first_of(right);

		if (std::string::npos == pos1 || std::string::npos == pos2 )
			return "";
		else
			return str.substr(pos1+1, pos2-pos1-1);
	}

	std::string BetweenL(const std::string &str, char first)
	{
		std::string::size_type pos = str.find_first_of(first);

		if (std::string::npos == pos)
			return "";
		else
			return str.substr(0, pos);
	}


	std::string BetweenR(const std::string &str, char first)
	{
		std::string::size_type pos = str.find_last_of(first);

		if (std::string::npos == pos || pos == str.length()-1)
			return "";
		else
			return str.substr(pos+1);
	}

    std::string Trim(const std::string &src, const std::string &trim)
    {
        std::string ret;
        std::string::size_type pos1 = src.find_first_not_of(trim);
        std::string::size_type pos2 = src.find_last_not_of(trim);

        if (std::string::npos == pos1 || std::string::npos == pos2 )
            ret = "";
        else
            ret = src.substr(pos1, pos2 - pos1 + 1);

        return ret;
    }
    
    std::string TrimHead(const std::string &src, const std::string &trim)
    {
        std::string ret;
        std::string::size_type pos = src.find_first_not_of(trim);

        if (std::string::npos == pos)
            ret = "";
        else
            ret = src.substr(pos);

        return ret;
    }

    std::string TrimTail(const std::string &src, const std::string &trim)
    {
        std::string ret;
        std::string::size_type pos = src.find_last_not_of(trim);

        if (std::string::npos == pos)
            ret = "";
        else
            ret = src.substr(0, pos+1);

        return ret;
    }

    std::string LowerStr(const std::string &src)
    {
        char* pBuf = (char*)src.c_str();
        size_t len = src.length();

        while(len)
        {
            pBuf[len-1] = tolower(pBuf[len-1]);
            len--;
        }

        return std::string(pBuf);
    }

    std::string UpperStr(const std::string &src)
    {
        char* pBuf = (char*)src.c_str();
        size_t len = src.length();

        while(len)
        {
            pBuf[len-1] = toupper(pBuf[len-1]);
            len--;
        }

        return std::string(pBuf);
    }

    std::string Buffer2HexStr(const char* szBuf, unsigned int uiLen)
    {
        std::stringstream str;

        for(unsigned int i=0; i<uiLen; i++)
            str<<std::setw(2)<<std::hex<<std::uppercase<<std::setfill('0')<<(int)(unsigned char)szBuf[i] <<" ";
        return str.str();
    }



	sstring::sstring() : m_pBuffer(NULL), m_nSize(0){}

	sstring::sstring(const char *p, size_t len) : 
		m_pBuffer(NULL), m_nSize(0)
	{
		Clone(p, len);
	}

	sstring::sstring(const char *p) : 
		m_pBuffer(NULL), m_nSize(0)
	{
		Clone(p, (p == NULL)?0:strlen(p));
	}

	sstring::sstring(const sstring& rhs) :
		m_pBuffer(NULL), m_nSize(0)
	{
		Clone(rhs.Ptr(), rhs.Size());
	}

	sstring::sstring(const std::string& rhs) :
		m_pBuffer(NULL), m_nSize(0)
	{
		Clone(rhs.data(), rhs.size());
	}

	sstring& sstring::operator =(const sstring& rhs)
	{
		if (this != &rhs)
		{
			CheckFree();
			Clone(rhs.Ptr(), rhs.Size());
		}
		return *this;
	}

	sstring& sstring::assign(const char* p, int n)
	{
		CheckFree();
		if (p != NULL)
			Clone(p, n);
		return *this;
	}

	sstring& sstring::operator =(const char* p)
	{
		CheckFree();
		if (p != NULL)
			Clone(p, strlen(p));
		return *this;
	}

	sstring& sstring::operator =(const std::string& rhs)
	{
		CheckFree();
		Clone(rhs.data(), rhs.size());
		return *this;
	}

	bool sstring::operator ==(const sstring& rhs) const
	{
		if (m_nSize != rhs.Size()) return false;
		return memcmp(m_pBuffer, rhs.Ptr(), m_nSize) == 0;
	}

	bool sstring::operator !=(const sstring& rhs) const
	{
		return !(operator ==(rhs));
	}

	char& sstring::operator [](int i) const
	{
		return m_pBuffer[i];
	}

	sstring::~sstring()
	{
		CheckFree();
	}

	bool sstring::Clone(const char *p, size_t len)
	{
		CheckFree();
		if (p == NULL) return false;
		m_pBuffer = StdMem::Malloc(len);
		if (m_pBuffer == NULL) return false;
		memcpy(m_pBuffer, p, len);
		m_nSize = len;
		return true;
	}

	size_t sstring::FindFirstOf(const sstring &trim)
	{
		for (size_t i=0; i<m_nSize; ++i)
		{
			if (CharIn(m_pBuffer[i], trim))
				return i;
		}
		return -1;
	}

	size_t sstring::FindLastOf(const sstring &trim)
	{
		for (size_t i=m_nSize-1; i>=0; --i)
		{
			if (CharIn(m_pBuffer[i], trim))
				return i;
		}
		return -1;
	}

	size_t sstring::FindFirstNotOf(const sstring &trim)
	{
		for (size_t i=0; i<m_nSize; ++i)
		{
			if (!CharIn(m_pBuffer[i], trim))
				return i;
		}
		return -1;
	}

	size_t sstring::FindLastNotOf(const sstring &trim)
	{
		for (size_t i=m_nSize-1; i>=0; --i)
		{
			if (!CharIn(m_pBuffer[i], trim))
				return i;
		}
		return -1;
	}

	sstring sstring::TrimLeft(const sstring &trim)
	{
		sstring s("");

		size_t p = FindFirstNotOf(trim);
		if (p == -1)
			return s;

		s.Clone(m_pBuffer + p, m_nSize - p);
		return s;
	}

	sstring sstring::TrimRight(const sstring &trim)
	{
		sstring s("");

		size_t p = FindLastNotOf(trim);
		if (p == -1)
			return s;

		s.Clone(m_pBuffer, p + 1);
		return s;
	}

	sstring sstring::Trim(const sstring &trim)
	{
		sstring s("");

		size_t p1 = FindFirstNotOf(trim);
		if (p1 == -1)
			return s;

		size_t p2 = FindLastNotOf(trim);
		if (p2 == -1)
			return s;

		if (p2 > p1)
			s.Clone(m_pBuffer + p1, p2 - p1 + 1);

		return s;
	}

	sstring sstring::ToUpper()
	{
		sstring strUpper(m_pBuffer, m_nSize);
		for (size_t i=0; i<m_nSize; ++i)
		{
			if (strUpper[i] >= 'a' && strUpper[i] <= 'z')
				strUpper[i] = strUpper[i] - 32;
		}
		return strUpper;
	}

	sstring sstring::ToLower()
	{
		sstring strLower(m_pBuffer, m_nSize);
		for (size_t i=0; i<m_nSize; ++i)
		{
			if (strLower[i] >= 'A' && strLower[i] <= 'Z')
				strLower[i] += 32;
		}
		return strLower;
	}

	void sstring::CheckFree()
	{ 
		if (m_pBuffer != NULL)
		{
			StdMem::Free(m_pBuffer);
		}
		m_pBuffer = NULL;
		m_nSize = 0;
	}

	bool sstring::CharIn(char c, const sstring &s)
	{
		for (size_t i=0; i<s.Size(); ++i)
		{
			if (s[i] == c)
				return true;
		}
		return false;
	}


	spointer::spointer() : m_pBuffer(NULL), m_nSize(0) {}

	spointer::spointer(const char *p, size_t len) : m_pBuffer((char*)p), m_nSize(len) {}

	spointer::spointer(const spointer& rhs) : m_pBuffer(rhs.m_pBuffer), m_nSize(rhs.m_nSize) {}

	spointer& spointer::operator =(const spointer& rhs) 
	{ 
		if (this != &rhs) 
		{ 
			m_pBuffer = rhs.m_pBuffer; 
			m_nSize = rhs.m_nSize; 
		} 
		return *this; 
	}

	spointer::~spointer() {}

	void spointer::Attach(const char *p, size_t len)
	{
		m_pBuffer = (char*)p;
		m_nSize = len;
	}

	std::string bpointer::rhex() const
	{
		std::string print;
		char* p = m_buf;
		size_t len = m_size;

		while(len>0)
		{
			int n = len>=16 ? 16 : len;
			char t1[16], t2[16*3];

			int i = 0;
			while(i<n)
			{
				unsigned char c = *p++;
				t1[i] = isprint(c) ? c : '.';

				unsigned char b1 = c>>4;
				unsigned char b2 = c&0x0F;

				int j = (i++)*3;
				t2[j] = HEX(b1);
				t2[j+1] = HEX(b2);
				t2[j+2] = ' ';
			}

			if(n<16)
			{
				memset(t1+i, ' ', 16-n);
				memset(t2+i*3, ' ', 48-3*n);
			}

			print.append("   ").append(t1, sizeof(t1)).append(";  ").append(t2, sizeof(t2)).append("\n");
			len -= n;
		}
		return print;
	}
}
