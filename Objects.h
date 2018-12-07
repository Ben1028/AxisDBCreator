#pragma once
#include <string>
using namespace std;

class CItem
{
public:
	string sIndex;
	string sCat = "Uncategorized";
	string sSub = "Uncategorized";
	string sDesc = "Undefined";
	string sColor = "0";
	string sDupe = "0";
	string sDefname;
	string sID;
	string sPath;
};

class CChar
{
public:
	string sIndex;
	string sCat = "Uncategorized";
	string sSub = "Uncategorized";
	string sDesc = "Undefined";
	string sColor = "0";
	string sDefname;
	string sID;
	string sPath;
};

class CMap
{
public:
	string sIndex;
	string sCat = "Uncategorized";
	string sName = "Unknown";
	string sGroup = "Undefined";
	string sPos = "0";
	string sPath;
};

class CTemplate
{
public:
	string sIndex;
	string sCat = "Uncategorized";
	string sSub = "Uncategorized";
	string sDesc = "Undefined";
	string sID;
	string sPath;
};

class CObject
{
public:
	string sIndex;
	string sDefname;
	string sName;
	string sPath;
};

class CList
{
public:
	string sIndex;
	string sPath;
};

class CMsg
{
public:
	string sIndex;
	string sMsg;
	string sPath;
};