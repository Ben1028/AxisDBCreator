#include "AxisDBCreator.h"

// arg[1]: string
// path to spheretables.scp or single file
// arg[2]: string
// Database Name
int main(int argc, char *argv[])
{
	//return 0 = success
	//return 1 = failed
	//return 2 = not enough arguments
	int iReturn = 2;
	string sPath;

	//if (argc >= 3)
	//{
	//	return 0; //block this section
	//	string sMin = HexToInt(argv[1]);
	//	string sMax = HexToInt(argv[2]);
	//	if (!DBCreateData.Open(DBName.c_str()))
	//	{
	//		Table TBData;
	//		if (argc == 4)
	//		{
	//			TBData = DBCreateData.QuerySQL("SELECT * FROM Item WHERE DecID BETWEEN '" + sMin + "' AND '" + sMax + "' AND ID != Itemdef ORDER BY DecID ASC");
	//		}
	//		else
	//		{
	//			TBData = DBCreateData.QuerySQL("SELECT * FROM Item WHERE DecID BETWEEN '" + sMin + "' AND '" + sMax + "' AND ID == Itemdef ORDER BY DecID ASC");
	//		}
	//		TBData.ResetRow();
	//		while (TBData.GoNext())
	//		{
	//			ifstream file(TBData.GetValue("File"));
	//			if (!file.is_open())
	//			{
	//				printf(("ERROR: Unable to open file " + TBData.GetValue("File") + "\n").c_str());
	//				return false;
	//			}
	//			file.seekg(_atoi64(TBData.GetValue("Pos").c_str()));
	//			string sLine;
	//			getline(file, sLine);
	//			printf((sLine + "\n").c_str());
	//			while (getline(file, sLine))
	//			{
	//				if (sLine.find("[") == 0) //End of Block, EXIT
	//				{
	//					break;
	//				}
	//				printf((sLine + "\n").c_str());
	//			}
	//			file.close();
	//		}
	//		DBCreateData.Close();
	//	}
	//	//printf("No path specified. Please enter one now.\n");
	//	//cin >> sPath;
	//}
	//else 

	if (argc > 1)
	{
		sPath = argv[1];
	}
	if (argc > 2)
	{
		DBName = argv[2];
	}


	if (!sPath.empty())
	{
		ReplaceAll(sPath, string("/"), string("\\"));

		if (!DBCreateData.Open(DBName.c_str()))
		{
			logFile.open("AxisDBLog.txt");
			DBCreateData.BeginTransaction();
			if (MakeUpper(GetFilename(sPath)) == "SPHERETABLES.SCP")
			{
				DBCreateData.ExecuteSQL("DROP TABLE IF EXISTS Item");
				DBCreateData.ExecuteSQL("DROP TABLE IF EXISTS Char");
				DBCreateData.ExecuteSQL("DROP TABLE IF EXISTS Map");
				DBCreateData.ExecuteSQL("DROP TABLE IF EXISTS Function");
				DBCreateData.ExecuteSQL("DROP TABLE IF EXISTS Spell");
				DBCreateData.ExecuteSQL("DROP TABLE IF EXISTS Skill");
				DBCreateData.ExecuteSQL("DROP TABLE IF EXISTS Event");
				DBCreateData.ExecuteSQL("DROP TABLE IF EXISTS Type");
				DBCreateData.ExecuteSQL("DROP TABLE IF EXISTS DefMsg");

				DBCreateData.ExecuteSQL("CREATE TABLE 'Item' (Itemdef TEXT PRIMARY KEY NOT NULL COLLATE NOCASE, Def TEXT COLLATE NOCASE, ID TEXT COLLATE NOCASE, Desc TEXT COLLATE NOCASE, Sub TEXT COLLATE NOCASE, Cat TEXT COLLATE NOCASE, Color TEXT COLLATE NOCASE, Dupe TEXT COLLATE NOCASE, File TEXT COLLATE NOCASE, Pos TEXT, DecID INT, Type INT)");
				DBCreateData.ExecuteSQL("CREATE TABLE 'Char' (Chardef TEXT PRIMARY KEY NOT NULL COLLATE NOCASE, Def TEXT COLLATE NOCASE, ID TEXT COLLATE NOCASE, Desc TEXT COLLATE NOCASE, Sub TEXT COLLATE NOCASE, Cat TEXT COLLATE NOCASE, Color TEXT COLLATE NOCASE, File TEXT COLLATE NOCASE, Pos TEXT, DecID INT, Type INT)");
				DBCreateData.ExecuteSQL("CREATE TABLE 'Map' (Areadef TEXT PRIMARY KEY NOT NULL COLLATE NOCASE, Desc TEXT COLLATE NOCASE, Sub TEXT COLLATE NOCASE, Cat TEXT COLLATE NOCASE, Point TEXT COLLATE NOCASE, File TEXT COLLATE NOCASE)");
				DBCreateData.ExecuteSQL("CREATE TABLE 'Function' (Name TEXT PRIMARY KEY NOT NULL COLLATE NOCASE, File TEXT COLLATE NOCASE)");
				DBCreateData.ExecuteSQL("CREATE TABLE 'Spell' (ID TEXT PRIMARY KEY NOT NULL COLLATE NOCASE, Def TEXT COLLATE NOCASE, Name TEXT COLLATE NOCASE, File TEXT COLLATE NOCASE)");
				DBCreateData.ExecuteSQL("CREATE TABLE 'Skill' (ID TEXT PRIMARY KEY NOT NULL COLLATE NOCASE, Def TEXT COLLATE NOCASE, Name TEXT COLLATE NOCASE, File TEXT COLLATE NOCASE)");
				DBCreateData.ExecuteSQL("CREATE TABLE 'Event' (ID TEXT PRIMARY KEY NOT NULL COLLATE NOCASE, File TEXT COLLATE NOCASE)");
				DBCreateData.ExecuteSQL("CREATE TABLE 'Type' (ID TEXT PRIMARY KEY NOT NULL COLLATE NOCASE, File TEXT COLLATE NOCASE)");
				DBCreateData.ExecuteSQL("CREATE TABLE 'DefMsg' (Def TEXT PRIMARY KEY NOT NULL COLLATE NOCASE, Msg TEXT COLLATE NOCASE, File TEXT COLLATE NOCASE)");
				iReturn = !LoadTable(sPath);
			}
			else
			{
				DBCreateData.ExecuteSQL("DELETE FROM Item WHERE File = '" + sPath + "'");
				DBCreateData.ExecuteSQL("DELETE FROM Char WHERE File = '" + sPath + "'");
				DBCreateData.ExecuteSQL("DELETE FROM Map WHERE File = '" + sPath + "'");
				DBCreateData.ExecuteSQL("DELETE FROM Function WHERE File = '" + sPath + "'");
				DBCreateData.ExecuteSQL("DELETE FROM Spell WHERE File = '" + sPath + "'");
				DBCreateData.ExecuteSQL("DELETE FROM Skill WHERE File = '" + sPath + "'");
				DBCreateData.ExecuteSQL("DELETE FROM Event WHERE File = '" + sPath + "'");
				DBCreateData.ExecuteSQL("DELETE FROM Type WHERE File = '" + sPath + "'");
				DBCreateData.ExecuteSQL("DELETE FROM DefMsg WHERE File = '" + sPath + "'");
				iReturn = !LoadFile(sPath);
			}
			if (!iReturn)
			{
				InsertDB();
				DBCreateData.CommitTransaction();
				SortDB();
			}
			else
			{
				DBCreateData.RollbackTransaction();
			}
			DBCreateData.Close();
			logFile.close();
		}
		else
		{
			printf(("ERROR: Unable to create database file " + sPath + "\n").c_str());
			iReturn = 1;
		}
	}
    return iReturn;
}

bool LoadTable(string &sPath)
{
	ifstream file(sPath);
	if (!file.is_open())
	{
		printf(("ERROR: Unable to open table file " + sPath + "\n").c_str());
		return false;
	}
	printf(("Openning table file: " + sPath + "\n").c_str());

	bool bProcess = false;
	sPath.erase(sPath.rfind("\\") + 1); //trim end

	string sLine;
	while (getline(file, sLine))
	{
		string sUpper = MakeUpper(sLine);

		if (sUpper.find("[RESOURCES]") != string::npos)
		{
			printf(("Found: " + sLine + " Section\n").c_str());
			bProcess = true;
		}
		else if (sLine.find("[") != string::npos)
		{
			bProcess = false;
		}
		else if (bProcess)
		{
			StripStr(sLine);

			if (!sLine.empty())
			{

				ReplaceAll(sLine, string("/"), string("\\"));
				if (MakeUpper(GetFirstPath(sLine)) == (MakeUpper(GetLastPath(sPath))))
				{
					sLine.erase(0, sLine.find("\\") + 1);
				}
				string sLoad = sPath + sLine;

				if (sUpper.find(".SCP") != string::npos)
				{
					LoadFile(sLoad);
				}
				else
				{
					LoadSingleDirectory(sLoad);
				}
			}
		}
	}
	LoadSingleDirectory(sPath);

	if (file.bad())
	{
		printf(("Error while reading file " + sPath + "\n").c_str());
		file.close();
		return false;
	}

	file.close();
	return true;
}

void LoadSingleDirectory(const string &sPath)
{
	for (const auto & p : fs::directory_iterator(sPath))
	{
		string sUpper = MakeUpper(p.path().string());

		if (sUpper.find(".SCP") != string::npos)
		{
			LoadFile(p.path().string());
		}
	}
}


bool LoadFile(const string &sPath)
{
	ifstream file(sPath);
	if (!file.is_open())
	{
		printf(("ERROR: Unable to open file " + sPath + "\n").c_str());
		return false;
	}

	auto existe = find(begin(_vFiles), end(_vFiles), GetFilename(sPath));
	if (existe != end(_vFiles))
	{
		return false;
	}
	_vFiles.push_back(GetFilename(sPath));

	printf(("Reading file: " + sPath + "\n").c_str());

	string sLine;
	streampos oldpos = file.tellg();
	char buffer[_MAX_U64TOSTR_BASE2_COUNT];
	while (getline(file, sLine))
	{
		StripStr(sLine);
		string sUpper = MakeUpper(sLine);

		if (sUpper.find("[ITEMDEF") == 0)
		{
			CItem * cObject = new CItem;
			cObject->sIndex = GetIndex(sLine);
			cObject->sDefname = cObject->sIndex;
			cObject->sID = cObject->sIndex;
			cObject->sPath = sPath;
			_i64toa_s(oldpos, buffer, _countof(buffer), 10);
			cObject->sPos = buffer;
			ReadItem(file, cObject);
		}
		else if (sUpper.find("[MULTIDEF") == 0)
		{
			CItem * cObject = new CItem;
			cObject->sIndex = GetIndex(sLine);
			cObject->sDefname = cObject->sIndex;
			cObject->sID = cObject->sIndex;
			cObject->sPath = sPath;
			if (!IsStrType(cObject->sIndex))
			{
				cObject->iType = 1;
			}
			else
			{
				cObject->iType = 2;
			}
			_i64toa_s(oldpos, buffer, _countof(buffer), 10);
			cObject->sPos = buffer;
			ReadItem(file, cObject);
		}
		else if (sUpper.find("[CHARDEF") == 0)
		{
			CChar * cObject = new CChar;
			cObject->sIndex = GetIndex(sLine);
			cObject->sDefname = cObject->sIndex;
			cObject->sID = cObject->sIndex;
			cObject->sPath = sPath;
			_i64toa_s(oldpos, buffer, _countof(buffer), 10);
			cObject->sPos = buffer;
			ReadChar(file, cObject);
		}
		else if ((sUpper.find("[AREA") == 0) || (sUpper.find("[ROOM") == 0))
		{
			CMap * cObject = new CMap;
			cObject->sIndex = GetIndex(sLine);
			cObject->sPath = sPath;
			ReadMap(file, cObject);
		}
		else if (sUpper.find("[SPAWN ") == 0)
		{
			CChar * cObject = new CChar;
			cObject->sIndex = GetIndex(sLine);
			cObject->sDefname = cObject->sIndex;
			cObject->sID = cObject->sIndex;
			cObject->sPath = sPath;
			cObject->iType = 1;
			_i64toa_s(oldpos, buffer, _countof(buffer), 10);
			cObject->sPos = buffer;
			ReadSpawn(file, cObject);
		}
		else if (sUpper.find("[TEMPLATE ") == 0)
		{
			CItem * cObject = new CItem;
			cObject->sIndex = GetIndex(sLine);
			cObject->sDefname = cObject->sIndex;
			cObject->sID = cObject->sIndex;
			cObject->sPath = sPath;
			cObject->iType = 3;
			_i64toa_s(oldpos, buffer, _countof(buffer), 10);
			cObject->sPos = buffer;
			ReadTemplate(file, cObject);
		}
		else if (sUpper.find("[TYPEDEF ") == 0)
		{
			CList * cObject = new CList;
			cObject->sIndex = GetIndex(sLine);
			cObject->sPath = sPath;
			_vTypes.push_back(cObject);
		}
		else if (sUpper.find("[TYPEDEFS") == 0)
		{
			ReadTypes(file, sPath);
		}
		else if (sUpper.find("[EVENTS ") == 0)
		{
			CList * cObject = new CList;
			cObject->sIndex = GetIndex(sLine);
			cObject->sPath = sPath;
			_vEvents.push_back(cObject);
		}
		else if (sUpper.find("[FUNCTION ") == 0)
		{
			CList * cObject = new CList;
			cObject->sIndex = GetIndex(sLine);
			cObject->sPath = sPath;
			_vFunctions.push_back(cObject);
		}
		else if (sUpper.find("[SKILL ") == 0)
		{
			CObject * cObject = new CObject;
			cObject->sIndex = GetIndex(sLine);
			cObject->sPath = sPath;
			ReadList(file, cObject, _vSkills);
		}
		else if (sUpper.find("[SPELL ") == 0)
		{
			CObject * cObject = new CObject;
			cObject->sIndex = GetIndex(sLine);
			cObject->sPath = sPath;
			ReadList(file, cObject, _vSpells);
		}
		/*else if ((sUpper.find("[WORLDITEM ") == 0) || (sUpper.find("[WI ") == 0))
		{
		}*/
		else if (sUpper.find("[DEFNAME") == 0)
		{
			ReadDef(file, sPath);
		}
		else if (sUpper.find("[DEFMESSAGE ") == 0)
		{
			ReadMsg(file, sPath);
		}
		oldpos = file.tellg();
	}

	if (file.bad())
	{
		printf(("Error while reading file " + sPath + "\n").c_str());
		file.close();
		return false;
	}

	file.close();
	return true;
}

void ReadItem(ifstream &file, CItem *cObject)
{
	string sKey, sValue, sValueRaw;
	bool bIgnore = false;

	string sLine;
	streampos oldpos = file.tellg();
	while (getline(file, sLine))
	{
		//printf((sLine + "\n").c_str());
		StripStr(sLine);
		string sUpper = MakeUpper(sLine);

		if (sUpper.find("[") == 0) //End of Block, EXIT
		{
			file.seekg(oldpos);//return to last position so we don't skip the next block
			break;
		}
		oldpos = file.tellg();//save file position
		if ((sUpper.find("=") == string::npos) || bIgnore) //Skip these
		{
			continue;
		}

		if (sUpper.find("ON=@CREATE") == 0) // Allow reading of @Create Block
		{
			bIgnore = false;
			continue;
		}
		else if (sUpper.find("ON=@") == 0) //Ignore all other triggers
		{
			bIgnore = true;
			continue;
		}

		sKey = GetKey(sUpper);
		sValue = GetValue(sLine);
		sValueRaw = GetValueRaw(sLine);
		StripStr(sKey);
		StripStr(sValue);
		StripStr(sValueRaw);

		if (sKey == "DEFNAME")
		{
			cObject->sDefname = sValue;
		}
		else if (sKey == "CATEGORY")
		{
			cObject->sCat = sValueRaw;
		}
		else if (sKey == "SUBSECTION")
		{
			cObject->sSub = sValueRaw;
		}
		else if (sKey == "DESCRIPTION")
		{
			cObject->sDesc = sValueRaw;
		}
		else if ((sKey == "COLOR") && (!bIgnore))
		{
			cObject->sColor = sValue;
		}
		else if (sKey == "DUPEITEM")
		{
			cObject->sDupe = sValue;
		}
		else if ((sKey == "ID") && (!bIgnore))
		{
			cObject->sID = sValue;
		}
		else if ((sKey == "TYPE") && (MakeUpper(sValue).find("T_MULTI") == 0) && (cObject->iType == 0) && (!bIgnore))
		{

			cObject->iType = 1;
		}
	}
	_mDefs[cObject->sDefname] = cObject->sID;

	//Replace all @ in description with the subsection
	if (cObject->sDesc.find("@") != string::npos)
	{
		ReplaceAll(cObject->sDesc, string("@"), cObject->sSub);
	}
	//Check for invalid IDs
	if ((IsStrType(cObject->sID) == 0) && (cObject->sID == cObject->sIndex))
	{
		logData("ID '" + cObject->sID + "' Refers to itself (No valid ID) file:" + cObject->sPath + "\n");
		cObject->sID = "0";
	}
	if (cObject->iType == 2)
	{
		_vMultis.push_back(cObject);
	}
	else
	{
		_vItems.push_back(cObject);
	}
}

void ReadChar(ifstream &file, CChar *cObject)
{
	string sKey, sValue, sValueRaw;
	bool bIgnore = false;

	string sLine;
	streampos oldpos = file.tellg();
	while (getline(file, sLine))
	{
		StripStr(sLine);
		string sUpper = MakeUpper(sLine);

		if (sUpper.find("[") == 0) //End of Block, EXIT
		{
			file.seekg(oldpos);//return to last position so we don't skip the next block
			break;
		}
		oldpos = file.tellg();//save file position
		if ((sUpper.find("=") == string::npos) || bIgnore) //Skip these
		{
			continue;
		}

		if (sUpper.find("ON=@CREATE") == 0) // Allow reading of @Create Block
		{
			bIgnore = false;
			continue;
		}
		else if (sUpper.find("ON=@") == 0) //Ignore all other triggers
		{
			bIgnore = true;
			continue;
		}
		else if (sUpper.find("ITEMNEWBIE") == 0)//Ignore all after an item
		{
			bIgnore = true;
		}

		sKey = GetKey(sUpper);
		sValue = GetValue(sLine);
		sValueRaw = GetValueRaw(sLine);
		StripStr(sKey);
		StripStr(sValue);
		StripStr(sValueRaw);

		if (sKey == "DEFNAME")
		{
			cObject->sDefname = sValue;
		}
		else if (sKey == "CATEGORY")
		{
			cObject->sCat = sValueRaw;
		}
		else if (sKey == "SUBSECTION")
		{
			cObject->sSub = sValueRaw;
		}
		else if (sKey == "DESCRIPTION")
		{
			cObject->sDesc = sValueRaw;
		}
		else if ((sKey == "COLOR") && (!bIgnore))
		{
			cObject->sColor = sValue;
		}
		else if ((sKey == "ID") && (!bIgnore))
		{
			cObject->sID = sValue;
		}
	}
	_mDefs[cObject->sDefname] = cObject->sID;

	//Replace all @ in description with the subsection
	if (cObject->sDesc.find("@") != string::npos)
	{
		ReplaceAll(cObject->sDesc, string("@"), cObject->sSub);
	}
	//Check for invalid IDs
	if ((IsStrType(cObject->sID) == 0) && (cObject->sID == cObject->sIndex))
	{
		logData("ID '" + cObject->sID + "' Refers to itself (No valid ID) file:" + cObject->sPath + "\n");
		cObject->sID = "0";
	}

	_vChars.push_back(cObject);
}

void ReadMap(ifstream &file, CMap *cObject)
{
	string sKey, sValue, sValueRaw;

	string sLine;
	streampos oldpos = file.tellg();
	while (getline(file, sLine))
	{
		StripStr(sLine);
		string sUpper = MakeUpper(sLine);

		if ((sUpper.find("[") == 0) || (sUpper.find("ON=@") == 0)) //End of Block, EXIT
		{
			file.seekg(oldpos);//return to last position so we don't skip the next block
			break;
		}
		oldpos = file.tellg();//save file position
		if (sUpper.find("=") == string::npos) //Skip these
		{
			continue;
		}

		sKey = GetKey(sUpper);
		sValue = GetValue(sLine);
		sValueRaw = GetValueRaw(sLine);
		StripStr(sKey);
		StripStr(sValue);
		StripStr(sValueRaw);

		if (sKey == "NAME")
		{
			cObject->sName = sValueRaw;
		}
		else if (sKey == "GROUP")
		{
			cObject->sGroup = sValueRaw;
		}
		else if (sKey == "P")
		{
			cObject->sPos = sValueRaw;
			cObject->sCat = "Map - " + sValueRaw.substr(sValueRaw.find_last_of(",") + 1, 1);
		}
	}

	_vMap.push_back(cObject);
}

void ReadSpawn(ifstream &file, CChar *cObject)
{
	string sKey, sValue, sValueRaw;

	string sLine;
	streampos oldpos = file.tellg();
	while (getline(file, sLine))
	{
		StripStr(sLine);
		string sUpper = MakeUpper(sLine);

		if ((sUpper.find("[") == 0) || (sUpper.find("ON=@") == 0)) //End of Block, EXIT
		{
			file.seekg(oldpos);//return to last position so we don't skip the next block
			break;
		}
		oldpos = file.tellg();//save file position
		if (sUpper.find("=") == string::npos) //Skip these
		{
			continue;
		}

		sKey = GetKey(sUpper);
		sValue = GetValue(sLine);
		sValueRaw = GetValueRaw(sLine);
		StripStr(sKey);
		StripStr(sValue);
		StripStr(sValueRaw);

		 if (sKey == "CATEGORY")
		{
			cObject->sCat = sValueRaw;
		}
		else if (sKey == "SUBSECTION")
		{
			cObject->sSub = sValueRaw;
		}
		else if (sKey == "DESCRIPTION")
		{
			cObject->sDesc = sValueRaw;
		}
		else if ((sKey == "ID") && (cObject->sID == cObject->sIndex))
		{
			cObject->sID = sValue;
		}
	}
	_mDefs[cObject->sIndex] = cObject->sID;

	//Replace all @ in description with the subsection
	if (cObject->sDesc.find("@") != string::npos)
	{
		ReplaceAll(cObject->sDesc, string("@"), cObject->sSub);
	}
	//Check for invalid IDs
	if ((IsStrType(cObject->sID) == 0) && (cObject->sID == cObject->sIndex))
	{
		logData("Spawn group '" + cObject->sID + "' is empty! (No valid ID) file:" + cObject->sPath + "\n");
		cObject->sID = "0";
	}

	_vSpawns.push_back(cObject);
}

void ReadTemplate(ifstream &file, CItem *cObject)
{
	string sKey, sValue, sValueRaw;

	string sLine;
	streampos oldpos = file.tellg();
	while (getline(file, sLine))
	{
		StripStr(sLine);
		string sUpper = MakeUpper(sLine);

		if ((sUpper.find("[") == 0) || (sUpper.find("ON=@") == 0)) //End of Block, EXIT
		{
			file.seekg(oldpos);//return to last position so we don't skip the next block
			break;
		}
		oldpos = file.tellg();//save file position
		if (sUpper.find("=") == string::npos) //Skip these
		{
			continue;
		}

		sKey = GetKey(sUpper);
		sValue = GetValue(sLine);
		sValueRaw = GetValueRaw(sLine);
		StripStr(sKey);
		StripStr(sValue);
		StripStr(sValueRaw);

		if (sKey == "CATEGORY")
		{
			cObject->sCat = sValueRaw;
		}
		else if (sKey == "SUBSECTION")
		{
			cObject->sSub = sValueRaw;
		}
		else if (sKey == "DESCRIPTION")
		{
			cObject->sDesc = sValueRaw;
		}
		else if (((sKey == "CONTAINER") || (sKey == "ITEM") || (sKey == "SELL") || (sKey == "BUY")) && (cObject->sID == cObject->sIndex))
		{
			cObject->sID = sValue;
		}
	}
	_mDefs[cObject->sIndex] = cObject->sID;

	//Replace all @ in description with the subsection
	if (cObject->sDesc.find("@") != string::npos)
	{
		ReplaceAll(cObject->sDesc, string("@"), cObject->sSub);
	}
	//Check for invalid IDs
	if ((IsStrType(cObject->sID) == 0) && (cObject->sID == cObject->sIndex))
	{
		logData("Template '" + cObject->sID + "' is empty! (No valid ID) file:" + cObject->sPath + "\n");
		cObject->sID = "0";
	}

	_vTemplates.push_back(cObject);
}

void ReadList(ifstream &file, CObject *cObject, vector<CObject*> &vList)
{
	string sKey, sValue;

	string sLine;
	streampos oldpos = file.tellg();
	while (getline(file, sLine))
	{
		StripStr(sLine);
		string sUpper = MakeUpper(sLine);

		if ((sUpper.find("[") == 0) || (sUpper.find("ON=@") == 0)) //End of Block, EXIT
		{
			file.seekg(oldpos);//return to last position so we don't skip the next block
			break;
		}
		oldpos = file.tellg();//save file position
		if (sUpper.find("=") == string::npos) //Skip these
		{
			continue;
		}

		sKey = GetKey(sUpper);
		sValue = GetValue(sLine);
		StripStr(sKey);
		StripStr(sValue);

		if (sKey == "DEFNAME")
		{
			cObject->sDefname = sValue;
		}
		else if ((sKey == "NAME") || (sKey == "KEY"))
		{
			cObject->sName = sValue;
		}
	}

	vList.push_back(cObject);
}

void ReadTypes(ifstream &file, const string &sPath)
{
	string sKey;

	string sLine;
	streampos oldpos = file.tellg();
	while (getline(file, sLine))
	{
		StripStr(sLine);

		if (sLine.find("[") == 0) //End of Block, EXIT
		{
			file.seekg(oldpos);//return to last position so we don't skip the next block
			break;
		}
		oldpos = file.tellg();//save file position

		sKey = GetKey(sLine);
		StripStr(sKey);

		if (!sKey.empty())
		{
			CList * cObject = new CList;
			cObject->sIndex = sKey;
			cObject->sPath = sPath;
			_vTypes.push_back(cObject);
		}
	}
}

void ReadDef(ifstream &file, const string &sPath)
{
	string sKey, sValue;

	string sLine;
	streampos oldpos = file.tellg();
	while (getline(file, sLine))
	{
		StripStr(sLine);

		if (sLine.find("[") == 0) //End of Block, EXIT
		{
			file.seekg(oldpos);//return to last position so we don't skip the next block
			break;
		}
		oldpos = file.tellg();//save file position

		sKey = GetKey(sLine);
		sValue = GetDef(sLine);
		StripStr(sKey);
		StripStr(sValue);

		if (!sKey.empty())
		{
			_mDefs[sKey] = sValue;
		}
	}
}

void ReadMsg(ifstream &file, const string &sPath)
{
	string sKey, sValue;

	string sLine;
	streampos oldpos = file.tellg();
	while (getline(file, sLine))
	{
		StripStr(sLine);

		if (sLine.find("[") == 0) //End of Block, EXIT
		{
			file.seekg(oldpos);//return to last position so we don't skip the next block
			break;
		}
		oldpos = file.tellg();//save file position

		sKey = GetKey(sLine);
		sValue = GetDefRaw(sLine);
		StripStr(sKey);
		StripStr(sValue);

		if (!sKey.empty())
		{
			CMsg * cObject = new CMsg;
			cObject->sIndex = sKey;
			cObject->sMsg = sValue;
			cObject->sPath = sPath;
			_vMsgs.push_back(cObject);
		}
	}
}

void InsertDB()
{
	printf("Creating Database\n");
	//Items
	for (auto pData : _vItems)
	{
		while (IsStrType(pData->sID) == 0)
		{
			auto search = _mDefs.find(pData->sID);
			if (search == _mDefs.end())
			{
				logData("Item '" + pData->sIndex + "' has an invalid ID: " + pData->sID +" file:" + pData->sPath + "\n");
				break;
			}
			pData->sID = search->second;
		}
		while (IsStrType(pData->sColor) == 0)
		{
			auto search = _mDefs.find(pData->sColor);
			if (search == _mDefs.end())
			{
				break;
			}
			pData->sColor = search->second;
		}
		DBCreateData.ExecuteSQL("INSERT OR REPLACE INTO Item VALUES('" + pData->sIndex + "','" + pData->sDefname + "','" + MakeHex(pData->sID) + "','" + pData->sDesc + "','" + pData->sSub + "','" + pData->sCat + "','" + MakeHex(pData->sColor) + "','" + MakeHex(pData->sDupe) + "','" + pData->sPath + "','" + pData->sPos + "','" + HexToInt(pData->sID) + "','" + to_string(pData->iType) + "')");
		delete pData;
		pData = NULL;
	}

	//Multis
	for (auto pData : _vMultis)
	{
		while (IsStrType(pData->sID) == 0)
		{
			auto search = _mDefs.find(pData->sID);
			if (search == _mDefs.end())
			{
				logData("Multi '" + pData->sIndex + "' has an invalid ID: " + pData->sID + " file:" + pData->sPath + "\n");
				break;
			}
			pData->sID = search->second;
		}
		while (IsStrType(pData->sColor) == 0)
		{
			auto search = _mDefs.find(pData->sColor);
			if (search == _mDefs.end())
			{
				break;
			}
			pData->sColor = search->second;
		}
		DBCreateData.ExecuteSQL("INSERT OR REPLACE INTO Item VALUES('" + pData->sIndex + "','" + pData->sDefname + "','" + MakeHex(pData->sID) + "','" + pData->sDesc + "','" + pData->sSub + "','" + pData->sCat + "','" + MakeHex(pData->sColor) + "','" + MakeHex(pData->sDupe) + "','" + pData->sPath + "','" + pData->sPos + "','" + HexToInt(pData->sID) + "','" + to_string(pData->iType) + "')");
		delete pData;
		pData = NULL;
	}

	//Chars
	for (auto pData : _vChars)
	{
		while (IsStrType(pData->sID) == 0)
		{
			auto search = _mDefs.find(pData->sID);
			if (search == _mDefs.end())
			{
				logData("Char '" + pData->sIndex + "' has an invalid ID: " + pData->sID + " file:" + pData->sPath + "\n");
				break;
			}
			pData->sID = search->second;
		}
		while (IsStrType(pData->sColor) == 0)
		{
			auto search = _mDefs.find(pData->sColor);
			if (search == _mDefs.end())
			{
				break;
			}
			pData->sColor = search->second;
		}
		DBCreateData.ExecuteSQL("INSERT OR REPLACE INTO Char VALUES('" + pData->sIndex + "','" + pData->sDefname + "','" + MakeHex(pData->sID) + "','" + pData->sDesc + "','" + pData->sSub + "','" + pData->sCat + "','" + MakeHex(pData->sColor) + "','" + pData->sPath + "','" + pData->sPos + "','" + HexToInt(pData->sID) + "','" + to_string(pData->iType) + "')");
		delete pData;
		pData = NULL;
	}

	//Map
	for (auto pData : _vMap)
	{
		DBCreateData.ExecuteSQL("INSERT OR REPLACE INTO Map VALUES('" + pData->sIndex + "','" + pData->sName + "','" + pData->sGroup + "','" + pData->sCat + "','" + pData->sPos + "','" + pData->sPath + "')");
		delete pData;
		pData = NULL;
	}

	//Spawns
	for (auto pData : _vSpawns)
	{
		while (IsStrType(pData->sID) == 0)
		{
			auto search = _mDefs.find(pData->sID);
			if (search == _mDefs.end())
			{
				logData("Spawn '" + pData->sIndex + "' has an invalid ID: " + pData->sID + " file:" + pData->sPath + "\n");
				break;
			}
			pData->sID = search->second;
		}
		DBCreateData.ExecuteSQL("INSERT OR REPLACE INTO Char VALUES('" + pData->sIndex + "','" + pData->sDefname + "','" + MakeHex(pData->sID) + "','" + pData->sDesc + "','" + pData->sSub + "','" + pData->sCat + "','" + MakeHex(pData->sColor) + "','" + pData->sPath + "','" + pData->sPos + "','" + HexToInt(pData->sID) + "','" + to_string(pData->iType) + "')");
		delete pData;
		pData = NULL;
	}

	//Templates
	for (auto pData : _vTemplates)
	{
		while (IsStrType(pData->sID) == 0)
		{
			auto search = _mDefs.find(pData->sID);
			if (search == _mDefs.end())
			{
				logData("Template '" + pData->sIndex + "' has an invalid ID: " + pData->sID + " file:" + pData->sPath + "\n");
				break;
			}
			pData->sID = search->second;
		}
		DBCreateData.ExecuteSQL("INSERT OR REPLACE INTO Item VALUES('" + pData->sIndex + "','" + pData->sDefname + "','" + MakeHex(pData->sID) + "','" + pData->sDesc + "','" + pData->sSub + "','" + pData->sCat + "','" + MakeHex(pData->sColor) + "','" + MakeHex(pData->sDupe) + "','" + pData->sPath + "','" + pData->sPos + "','" + HexToInt(pData->sID) + "','" + to_string(pData->iType) + "')");
		delete pData;
		pData = NULL;
	}

	//Functions
	for (auto pData : _vFunctions)
	{
		DBCreateData.ExecuteSQL("INSERT OR REPLACE INTO Function VALUES('" + pData->sIndex + "','" + pData->sPath + "')");
		delete pData;
		pData = NULL;
	}

	//Spells
	for (auto pData : _vSpells)
	{
		DBCreateData.ExecuteSQL("INSERT OR REPLACE INTO Spell VALUES('" + pData->sIndex + "','" + pData->sDefname + "','" + pData->sName + "','" + pData->sPath + "')");
		delete pData;
		pData = NULL;
	}

	//Skills
	for (auto pData : _vSkills)
	{
		DBCreateData.ExecuteSQL("INSERT OR REPLACE INTO Skill VALUES('" + pData->sIndex + "','" + pData->sDefname + "','" + pData->sName + "','" + pData->sPath + "')");
		delete pData;
		pData = NULL;
	}

	//Events
	for (auto pData : _vEvents)
	{
		DBCreateData.ExecuteSQL("INSERT OR REPLACE INTO Event VALUES('" + pData->sIndex + "','" + pData->sPath + "')");
		delete pData;
		pData = NULL;
	}

	//Types
	for (auto pData : _vTypes)
	{
		DBCreateData.ExecuteSQL("INSERT OR REPLACE INTO Type VALUES('" + pData->sIndex + "','" + pData->sPath + "')");
		delete pData;
		pData = NULL;
	}

	//DefMessages
	for (auto pData : _vMsgs)
	{
		DBCreateData.ExecuteSQL("INSERT OR REPLACE INTO DefMsg VALUES('" + pData->sIndex + "','" + pData->sMsg + "','" + pData->sPath + "')");
		delete pData;
		pData = NULL;
	}
}


bool SortDB()
{
	printf("Sorting Items\n");
	//Fix Dupeitems
	Table TBData = DBCreateData.QuerySQL("SELECT * FROM Item WHERE Dupe != '0'");
	TBData.ResetRow();
	DBCreateData.BeginTransaction();
	while (TBData.GoNext())
	{
		Table TBTemp = DBCreateData.QuerySQL("SELECT * FROM Item WHERE Itemdef = '" + TBData.GetValue("Dupe") + "'");
		if (TBTemp.GetRowCount() != 0)
		{
			DBCreateData.ExecuteSQL("UPDATE Item SET Desc = '" + TBTemp.GetValue("Desc") + " - (Dupe)', Sub = '" + TBTemp.GetValue("Sub") + "', Cat = '" + TBTemp.GetValue("Cat") + "' WHERE Itemdef = '" + TBData.GetValue("Itemdef") + "'");
		}
		else
		{
			logData("DupeItem '" + TBData.GetValue("Dupe") + "' used but not defined for Item '" + TBData.GetValue("Itemdef") + "' file:" + TBData.GetValue("File") + "\n");
		}
	}
	DBCreateData.CommitTransaction();

	printf("Sorting Multis\n");
	//Fix Dupeitems
	TBData = DBCreateData.QuerySQL("SELECT * FROM Multi WHERE Dupe != '0'");
	TBData.ResetRow();
	DBCreateData.BeginTransaction();
	while (TBData.GoNext())
	{
		Table TBTemp = DBCreateData.QuerySQL("SELECT * FROM Multi WHERE Multidef = '" + TBData.GetValue("Dupe") + "'");
		if (TBTemp.GetRowCount() != 0)
		{
			DBCreateData.ExecuteSQL("UPDATE Multi SET Desc = '" + TBTemp.GetValue("Desc") + " - (Dupe)', Sub = '" + TBTemp.GetValue("Sub") + "', Cat = '" + TBTemp.GetValue("Cat") + "' WHERE Multidef = '" + TBData.GetValue("Multidef") + "'");
		}
		else
		{
			logData("DupeItem '" + TBData.GetValue("Dupe") + "' used but not defined for Multi '" + TBData.GetValue("Multidef") + "' file:" + TBData.GetValue("File") + "\n");
		}
	}
	DBCreateData.CommitTransaction();
	return true;
}
