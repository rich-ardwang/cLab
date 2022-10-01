#include "xmlParser/iniparser.h"

#include <fstream>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

namespace windcom
{

	INIParser::INIParser(const char *file):m_szBuf(NULL),m_iBufLen(0),m_bGood(false)
	{
		ifstream ofile(file, std::ios::binary);

		if(ofile)
		{
			ofile.seekg (0, ofile.end);
			m_iBufLen = ofile.tellg();
			ofile.seekg (0, ofile.beg);

			m_szBuf = new char [m_iBufLen + 1];

			// read data as a block:
			ofile.read (m_szBuf, m_iBufLen);

			if (!ofile)
			{
				m_bGood = false;
				delete []m_szBuf;
				m_szBuf = NULL;
				m_iBufLen = 0;
			}
			else
			{
				m_bGood = true;
				m_szBuf[m_iBufLen] = '\0';
			}
		}
		else
		{
			m_bGood = false;
		}

		ofile.close();
	}

	INIParser::~INIParser()
	{
		if(m_szBuf)
			delete [] m_szBuf;
	}

	bool INIParser::GetValue(const string &strSection, const string &strKey, string &strValue)
	{
		if(strSection.empty() || strKey.empty())
			return false;

		strValue.clear();
		int sec_s,sec_e,key_s,key_e, value_s, value_e;
		if(parse(strSection.c_str(), strKey.c_str(), m_szBuf, &sec_s,&sec_e,&key_s,&key_e,&value_s,&value_e))
		{

			int cpcount = value_e - value_s;
			strValue.assign(m_szBuf + value_s, cpcount);
			return true;
		}
		return false;
	}

	bool INIParser::GetValue(const string& strSection, const string& strKey, int& iValue)
	{
		string strValue;
		if(GetValue(strSection, strKey, strValue))
		{
			iValue = atoi(strValue.c_str());
			return true;
		}
		return false;
	}

	bool INIParser::parse(const char *section, const char *key, const char *buf, int *sec_s, int *sec_e,
		int *key_s, int *key_e, int *value_s, int *value_e)
	{
		const char *p = buf;
		int i=0;

		assert(buf!=NULL);
		assert(section != NULL && strlen(section));
		assert(key != NULL && strlen(key));

		*sec_e = *sec_s = *key_e = *key_s = *value_s = *value_e = -1;

		while( !isend(p[i]) ) 
		{
			//find the section
			if( ( 0==i ||  isnewline(p[i-1]) ) && isleft_barce(p[i]) )
			{
				int section_start=i+1;

				//find the ']'
				do {
					i++;
				} while( !isright_brace(p[i]) && !isend(p[i]));

				if( 0 == strncmp(p+section_start,section, i-section_start)) {
					int newline_start=0;

					i++;

					//Skip over space char after ']'
					while(isspace(p[i])) {
						i++;
					}

					//find the section
					*sec_s = section_start;
					*sec_e = i;

					while( ! (isnewline(p[i-1]) && isleft_barce(p[i]))
						&& !isend(p[i]) ) {
							int j=0;
							//get a new line
							newline_start = i;

							while( !isnewline(p[i]) &&  !isend(p[i]) ) {
								i++;
							}

							//now i  is equal to end of the line
							j = newline_start;

							if(';' != p[j]) //skip over comment
							{
								while(j < i && p[j]!='=') {
									j++;
									if('=' == p[j]) {
										if(strncmp(key,p+newline_start,j-newline_start)==0)
										{
											//find the key ok
											*key_s = newline_start;
											*key_e = j-1;

											*value_s = j+1;
											*value_e = i;

											return true;
										}
									}
								}
							}

							i++;
					}
				}
			}
			else
			{
				i++;
			}
		}

		return false;
	}
}
