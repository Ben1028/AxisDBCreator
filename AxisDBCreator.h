#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <sstream>
#include <map>
using namespace std;
#include "SQLite.h"
#include "Objects.h"
using namespace SQLite;

string DBName = "Axis.db";
Database DBCreateData;

struct ci_less
{
	struct nocase_compare
	{
		bool operator() (const unsigned char& c1, const unsigned char& c2) const {
			return tolower(c1) < tolower(c2);
		}
	};
	bool operator() (const string & s1, const string & s2) const {
		return lexicographical_compare
		(s1.begin(), s1.end(),
			s2.begin(), s2.end(),
			nocase_compare());
	}
};
map<string, string, ci_less> _mDefs;
vector<string> _vFiles;
vector<CItem*> _vItems;
vector<CItem*> _vMultis;
vector<CChar*> _vChars;
vector<CMap*> _vMap;
vector<CTemplate*> _vSpawns;
vector<CTemplate*> _vTemplates;
vector<CList*> _vFunctions;
vector<CObject*> _vSpells;
vector<CObject*> _vSkills;
vector<CList*> _vEvents;
vector<CList*> _vTypes;
vector<CMsg*> _vMsgs;

namespace fs = std::experimental::filesystem;

ofstream logFile;
bool SortDB();
void InsertDB();
void LoadSingleDirectory(string &sPath);
bool LoadTable(string &sPath);
bool LoadFile(string &sPath);
void ReadItem(ifstream &file, CItem *cObject, bool bMulti = false);
void ReadChar(ifstream &file, CChar *cObject);
void ReadMap(ifstream &file, CMap *cObject);
void ReadSpawn(ifstream &file, CTemplate *cObject);
void ReadTemplate(ifstream &file, CTemplate *cObject);
void ReadList(ifstream &file, CObject *cObject, vector<CObject*> &vList);
void ReadTypes(ifstream &file, string &sPath);
void ReadDef(ifstream &file, string &sPath);
void ReadMsg(ifstream &file, string &sPath);

//Replace all instances of 'from' by 'to' in 'str'
void ReplaceAll(string &str, const string& from, const string& to)
{
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != string::npos) 
	{
		str.replace(start_pos, from.length(), to);
		start_pos += to.length();
	}
}

//Strip text line from unwanted data
void StripStr(string& s)
{
	const char* t = " \t\n\r";
	//remove white space from begining and end
	s.erase(0, s.find_first_not_of(t));
	s.erase(s.find_last_not_of(t) + 1);
	//Remove comments
	size_t pos = s.find("//");
	if (pos != string::npos)
	{
		s.erase(pos);
	}
}

string MakeUpper(string s)
{
	transform(s.begin(), s.end(), s.begin(), ::toupper);
	return s;
}

string GetFilename(string s)
{
	//remove all before the last '\'
	size_t pos = s.find_last_of("\\");
	if (pos != string::npos)
	{
		s.erase(0, pos + 1);
	}
	return s;
}

//Get the resource index inside []
string GetIndex(string s)
{
	size_t pos = s.find(" ");
	if (pos != string::npos)
	{
		s.erase(0, pos + 1);
	}
	pos = s.find("]");
	if (pos != string::npos)
	{
		s.erase(pos);
	}
	return s;
}

//Get script key (value before the '=' sign or blank space)
string GetKey(string s)
{
	size_t pos = s.find_first_of("= \t\n\r");
	if (pos != string::npos)
	{
		s.erase(pos);
	}
	return s;
}

//Get only first argument after the '=' sign or blank space
string GetValue(string s)
{
	//remove all before the '=' sign
	size_t pos = s.find_first_of("=");
	if (pos != string::npos)
	{
		s.erase(0, pos + 1);
	}
	//remove white space at begining or '{'
	pos = s.find_first_not_of("{ \t\n\r");
	if (pos != string::npos)
	{
		s.erase(0, pos);
	}
	//remove anything after next white space or ',' or '|'
	pos = s.find_first_of("|, \t");
	if (pos != string::npos)
	{
		s.erase(pos);
	}
	return s;
}

//Get all after the '=' sign or blank space
string GetValueRaw(string s)
{
	//remove all before the '=' sign or blank space
	size_t pos = s.find_first_of("=");
	if (pos != string::npos)
	{
		s.erase(0, pos + 1);
	}
	//escape ' for SQLite
	ReplaceAll(s, "'", "''");
	return s;
}

//Get only first argument after the blank space
string GetDef(string s)
{
	//remove all before the space
	size_t pos = s.find_first_of(" \t\n\r");
	if (pos != string::npos)
	{
		s.erase(0, pos + 1);
	}
	//remove white space at begining or '{'
	pos = s.find_first_not_of("{ \t\n\r");
	if (pos != string::npos)
	{
		s.erase(0, pos);
	}
	//remove anything after next white space or ',' or '|'
	pos = s.find_first_of("|, \t");
	if (pos != string::npos)
	{
		s.erase(pos);
	}
	return s;
}

//Get all after the blank space
string GetDefRaw(string s)
{
	//remove all before the blank space
	size_t pos = s.find_first_of(" \t\n\r");
	if (pos != string::npos)
	{
		s.erase(0, pos + 1);
	}
	//escape ' for SQLite
	ReplaceAll(s, "'", "''");
	return s;
}

//Insert line in log file (only last log file is stored)
void logData(string data)
{
	logFile << data;
}

//Return string type
//0 = string
//1 = hex number
//2 = decimal number
UINT IsStrType(string s)
{
	//hex or string
	if (s.find("0") == 0)
	{
		if (s.find_first_not_of("0123456789abcdefADBDEF") == string::npos)
		{
			return 1;
		}
		else
		{
			return 0;
		}
		
	}
	//decimal
	else if (s.find_first_not_of("0123456789") == string::npos)
	{
		return 2;
	}
	//string
	else
	{
		return 0;
	}
}