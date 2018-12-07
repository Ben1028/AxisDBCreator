#include "SQLite.h"

namespace SQLite
{
	Database::Database(void)
	{
		m_sqlite3 = 0;
		m_iLastError = SQLITE_OK;
	}

	Database::~Database(void)
	{
		Close();
	}

	int Database::Open(std::string strFileName)
	{
		Close();

		int iErr = sqlite3_open(strFileName.c_str(), &m_sqlite3);

		if (iErr != SQLITE_OK) m_iLastError = iErr;

		return iErr;
	}

	void Database::Close()
	{
		if (m_sqlite3)
		{
			sqlite3_close(m_sqlite3);
			m_sqlite3 = 0;
		}
	}

	bool Database::IsOpen()
	{
		if (this == 0) return false;
		return m_sqlite3 != 0;
	}

	Table Database::QuerySQL(std::string strSQL)
	{
		if (!IsOpen()) {
			m_iLastError = SQLITE_ERROR;
			return Table();
		}

		char ** retStrings = 0;
		char * errmsg = 0;
		int iRows = 0, iCols = 0;

		int iErr = sqlite3_get_table(m_sqlite3, strSQL.c_str(), &retStrings,
			&iRows, &iCols, &errmsg);

		if (iErr != SQLITE_OK) m_iLastError = iErr;

		sqlite3_free(errmsg);

		Table retTable;

		if (iRows > 0) retTable.m_iPos = 0;

		retTable.m_iCols = iCols;
		retTable.m_iRows = iRows;

		retTable.m_strlstCols.reserve(iCols);

		int iPos = 0;

		for (; iPos < iCols; iPos++)
		{
			retTable.m_strlstCols.push_back(stdvstring());

			if (retStrings[iPos])
				ConvertUTF8ToString(retStrings[iPos], retTable.m_strlstCols.back());
			else retTable.m_strlstCols.back().push_back('\0');
		}

		retTable.m_lstRows.resize(iRows);
		for (int iRow = 0; iRow < iRows; iRow++)
		{
			retTable.m_lstRows[iRow].reserve(iCols);
			for (int iCol = 0; iCol < iCols; iCol++)
			{
				retTable.m_lstRows[iRow].push_back(stdvstring());

				if (retStrings[iPos])
					ConvertUTF8ToString(retStrings[iPos], retTable.m_lstRows[iRow].back());
				else retTable.m_lstRows[iRow].back().push_back('\0');

				iPos++;
			}
		}

		sqlite3_free_table(retStrings);

		return retTable;
	}

	void Database::ConvertUTF8ToString(char * strInUTF8MB, stdvstring & strOut)
	{
		int len = (int)strlen(strInUTF8MB) + 1;
		strOut.resize(len, 0);


		WCHAR * wChar = new WCHAR[len];
		wChar[0] = 0;
		MultiByteToWideChar(CP_UTF8, 0, strInUTF8MB, len, wChar, len);
		WideCharToMultiByte(CP_ACP, 0, wChar, len, &strOut[0], len, 0, 0);
		delete[] wChar;

	}

	int Database::ExecuteSQL(std::string strSQL)
	{
		if (!IsOpen()) {
			m_iLastError = SQLITE_ERROR;
			return SQLITE_ERROR;
		}

		char * errmsg = 0;

		int iErr = sqlite3_exec(m_sqlite3, strSQL.c_str(), 0, 0, &errmsg);

		if (iErr != SQLITE_OK) m_iLastError = iErr;

		sqlite3_free(errmsg);

		return iErr;
	}

	int Database::IsSQLComplete(std::string strSQL)
	{
		return sqlite3_complete(strSQL.c_str());
	}

	int Database::GetLastChangesCount()
	{
		return sqlite3_changes(m_sqlite3);
	}

	sqlite_int64 Database::GetLastInsertRowID()
	{
		if (m_sqlite3 == 0) return 0;

		return sqlite3_last_insert_rowid(m_sqlite3);
	}

	bool Database::BeginTransaction()
	{
		if (!IsOpen())
		{
			m_iLastError = SQLITE_ERROR;
			return false;
		}
		m_iLastError = ExecuteSQL("BEGIN TRANSACTION");
		return m_iLastError == SQLITE_OK;
	}

	bool Database::CommitTransaction()
	{
		if (!IsOpen())
		{
			m_iLastError = SQLITE_ERROR;
			return false;
		}
		m_iLastError = ExecuteSQL("COMMIT TRANSACTION");
		return m_iLastError == SQLITE_OK;
	}

	bool Database::RollbackTransaction()
	{
		if (!IsOpen())
		{
			m_iLastError = SQLITE_ERROR;
			return false;
		}
		m_iLastError = ExecuteSQL("ROLLBACK TRANSACTION");
		return m_iLastError == SQLITE_OK;
	}

	std::string Table::GetColName(int iCol)
	{
		if (iCol >= 0 && iCol < m_iCols)
		{
			return &m_strlstCols[iCol][0];
		}
		return 0;
	}

	bool Table::GoFirst()
	{
		if (this == 0) return false;
		if (m_lstRows.size())
		{
			m_iPos = 0;
			return true;
		}
		return false;
	}

	bool Table::GoLast()
	{
		if (m_lstRows.size())
		{
			m_iPos = (int)m_lstRows.size() - 1;
			return true;
		}
		return false;
	}

	bool Table::GoNext()
	{
		if (m_iPos + 1 < (int)m_lstRows.size())
		{
			m_iPos++;
			return true;
		}
		return false;
	}

	bool Table::GoPrev()
	{
		if (m_iPos > 0)
		{
			m_iPos--;
			return true;
		}
		return false;
	}

	bool Table::GoRow(unsigned int iRow)
	{
		if (this == 0) return false;
		if (iRow < m_lstRows.size())
		{
			m_iPos = iRow;
			return true;
		}
		return false;
	}

	std::string Table::GetValue(std::string lpColName)
	{
		if (lpColName.empty()) return 0;
		if (m_iPos < 0) return 0;
		for (int i = 0; i < m_iCols; i++)
		{
			if (!_stricmp(&m_strlstCols[i][0], lpColName.c_str()))
			{
				return &m_lstRows[m_iPos][i][0];
			}
		}
		return 0;
	}

	std::string Table::GetValue(int iColIndex)
	{
		if (iColIndex < 0 || iColIndex >= m_iCols) return 0;
		if (m_iPos < 0) return 0;
		return &m_lstRows[m_iPos][iColIndex][0];
	}

	std::string Table::operator [] (std::string lpColName)
	{
		if (lpColName.empty()) return 0;
		if (m_iPos < 0) return 0;
		for (int i = 0; i < m_iCols; i++)
		{
			if (!_stricmp(&m_strlstCols[i][0], lpColName.c_str()))
			{
				return &m_lstRows[m_iPos][i][0];
			}
		}
		return 0;
	}

	std::string Table::operator [] (int iColIndex)
	{
		if (iColIndex < 0 || iColIndex >= m_iCols) return 0;
		if (m_iPos < 0) return 0;
		return &m_lstRows[m_iPos][iColIndex][0];
	}

	void Table::JoinTable(Table & tblJoin)
	{
		if (m_iCols == 0) {
			*this = tblJoin;
			return;
		}
		if (m_iCols != tblJoin.m_iCols) return;

		if (tblJoin.m_iRows > 0)
		{
			m_iRows += tblJoin.m_iRows;

			for (std::vector<row>::iterator it = tblJoin.m_lstRows.begin();
				it != tblJoin.m_lstRows.end(); it++)
			{
				m_lstRows.push_back(*it);
			}
		}
	}
}