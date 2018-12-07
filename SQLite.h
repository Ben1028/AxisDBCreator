#pragma once

#include "sqlite3.h"
#include <string>
#include <vector>

namespace SQLite
{
	//////////////////////////////////////////////////////////////////////////
	// Typedefs
	//////////////////////////////////////////////////////////////////////////

	// I use this one for a sequential array in memory,
	//   So I can write directly to it
	typedef std::vector<char> stdvstring;
	typedef std::vector<stdvstring> vstrlist;
	typedef vstrlist row;

	//////////////////////////////////////////////////////////////////////////
	// Classes
	//////////////////////////////////////////////////////////////////////////

	class Table; // Forward declaration
	class TablePtr; // Forward declaration

	// Main wrapper
	class Database
	{
	public:
		Database(void);
		virtual ~Database(void);

		int Open(std::string strFileName );
		void Close();
		bool IsOpen();

		sqlite3 * GetPtr(){ return m_sqlite3; };
		int GetLastError(){ return m_iLastError; };
		void ClearError() { m_iLastError=SQLITE_OK; };

		Table QuerySQL(std::string strSQL );
		int ExecuteSQL(std::string strSQL );

		int IsSQLComplete(std::string strSQL );
		int GetLastChangesCount();

		sqlite_int64 GetLastInsertRowID();

		bool BeginTransaction();
		bool CommitTransaction();
		bool RollbackTransaction();

	private:
		sqlite3 * m_sqlite3;
		int m_iLastError;

		void ConvertUTF8ToString( char * strInUTF8MB, stdvstring & strOut );
	};


	class Table
	{
		friend class Database;
	public:
		Table(void){ m_iRows=m_iCols=0;	m_iPos=-1; };
		virtual ~Table() {};

		int GetColCount(){ if (this==0) return 0; return m_iCols; };
		int GetRowCount(){ if (this==0) return 0; return m_iRows; };
		int GetCurRow(){ if (this==0) return -1; return m_iPos; };

		std::string GetColName( int iCol );

		void ResetRow(){ m_iPos = -1; };

		bool GoFirst();
		bool GoLast();
		bool GoNext();
		bool GoPrev();
		bool GoRow(unsigned int iRow);

		std::string GetValue(std::string lpColName);
		std::string GetValue(int iColIndex);
		std::string operator [] (std::string lpColName);
		std::string operator [] (int iColIndex);

		void JoinTable(Table & tblJoin);

	private:
		int m_iRows, m_iCols;

		row m_strlstCols;
		std::vector<row> m_lstRows;

		int m_iPos;
	};
}
