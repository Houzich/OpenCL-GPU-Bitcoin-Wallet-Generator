#pragma once
//#include <cstdint>
#include <sqlite3.h>
#include <string>
#include <stdexcept>
/**************************** DATA TYPES ****************************/
namespace sqlite_db
{
	/*
	  Sqlite3 database, not used currently
	*/

	class Sqlite3Exception : public std::runtime_error
	{
	public:
		Sqlite3Exception(char* msg, bool free_msg_after_cons = false) : std::runtime_error(std::string(msg)) { if (free_msg_after_cons) sqlite3_free(msg); }
		Sqlite3Exception(int code) : std::runtime_error(sqlite3_errstr(code)) { }
	};

	class Sqlite3
	{
	public:
		Sqlite3() noexcept : db(nullptr) {}

		explicit Sqlite3(const std::string& db_filepath)
			: db(nullptr)
		{
			try
			{
				open(db_filepath);
			}
			catch (const Sqlite3Exception& err)
			{
				throw;
			}
		}

		void open(const std::string& db_filepath)
		{
			if (db != nullptr)
			{
				sqlite3_close(db);
			}
			int rc;

			rc = sqlite3_enable_shared_cache(1);
			if (rc != SQLITE_OK)
			{
				throw Sqlite3Exception(rc);
			}
	
			//rc = sqlite3_open(db_filepath.c_str(), &db);
			rc = sqlite3_open_v2(db_filepath.c_str(), &db, SQLITE_OPEN_READONLY | SQLITE_OPEN_NOMUTEX | SQLITE_OPEN_SHAREDCACHE, NULL);


			if (rc != SQLITE_OK)
			{
				throw Sqlite3Exception(rc);
			}

			char* errmsg = nullptr;
			rc = sqlite3_exec(db, "PRAGMA synchronous = OFF", NULL, NULL, &errmsg);
			if (rc != SQLITE_OK)
			{
				throw Sqlite3Exception(errmsg, true);
			}

			rc = sqlite3_exec(db, "PRAGMA journal_mode = DELETE", NULL, NULL, &errmsg);
			if (rc != SQLITE_OK)
			{
				throw Sqlite3Exception(errmsg, true);
			}

			rc = sqlite3_exec(db, "PRAGMA read_uncommitted = ON", NULL, NULL, &errmsg);
			if (rc != SQLITE_OK)
			{
				throw Sqlite3Exception(errmsg, true);
			}
			
			rc = sqlite3_exec(db, "PRAGMA locking_mode = DEFERRED", NULL, NULL, &errmsg);
			if (rc != SQLITE_OK)
			{
				throw Sqlite3Exception(errmsg, true);
			}
		
		}

		void exec(const std::string& stmt, int(*callback)(void*, int, char**, char**) = nullptr, void* first_arg = nullptr)
		{
			char* errmsg = nullptr;
			int rc = sqlite3_exec(db, stmt.c_str(), callback, first_arg, &errmsg);
			if (rc != SQLITE_OK)
			{
				throw Sqlite3Exception(errmsg, true);
			}
		}

		int get_rows(const std::string& table_name)
		{
			int rows = -1;
			sqlite3_stmt* stmt;
			std::string sql_stmt = "SELECT COUNT(*) from " + table_name + ";";
			int rc = sqlite3_prepare_v2(db, sql_stmt.c_str(), -1, &stmt, nullptr);
			if (rc != SQLITE_OK)
			{
				throw Sqlite3Exception(rc);
			}

			rc = sqlite3_step(stmt);
			if (rc != SQLITE_ROW)
			{
				sqlite3_finalize(stmt);
				throw Sqlite3Exception(rc);
			}

			rows = sqlite3_column_int(stmt, 0);

			sqlite3_finalize(stmt);

			return rows;
		}


		void insert_address(const std::string& table_name, const std::string& address)
		{
			sqlite3_stmt* stmt;
			std::string sql_stmt = "INSERT or IGNORE INTO " + table_name + " (ADDRESS) VALUES (?);";
			int rc = sqlite3_prepare_v2(db, sql_stmt.c_str(), -1, &stmt, nullptr);
			if (rc != SQLITE_OK)
			{
				throw Sqlite3Exception(rc);
			}

			rc = sqlite3_bind_text(stmt, 1, address.c_str(), -1, SQLITE_STATIC);

			rc = sqlite3_step(stmt);
			if (rc != SQLITE_DONE)
			{
				sqlite3_finalize(stmt);
				throw Sqlite3Exception(rc);
			}

			sqlite3_finalize(stmt);
		}


		int search_address(const std::string table_name, const char* address)
		{
			int ret = 0;
			sqlite3_stmt* stmt;
			std::string sql_stmt = "SELECT ADDRESS FROM " + table_name + " WHERE ADDRESS = ? LIMIT 1;";

			int rc = SQLITE_OK;
			do {
				if ((rc != SQLITE_BUSY)&&(rc != SQLITE_OK)) {
					sqlite3_reset(stmt);
				}
				rc = sqlite3_prepare_v2(db, sql_stmt.c_str(), -1, &stmt, nullptr);
			} while (rc != SQLITE_OK);

			if (rc != SQLITE_OK)
			{
				throw Sqlite3Exception(rc);
			}


			do {
				rc = sqlite3_bind_text(stmt, 1, address, -1, SQLITE_STATIC);
			} while (rc == SQLITE_BUSY);

			if (rc != SQLITE_OK)
			{
				sqlite3_finalize(stmt);
				throw Sqlite3Exception(rc);
			}


			do {
				rc = sqlite3_step(stmt);
			} while (rc == SQLITE_BUSY);

			if (rc != SQLITE_DONE && rc != SQLITE_ROW)
			{
				sqlite3_finalize(stmt);
				throw Sqlite3Exception(rc);
			}
			if (rc == SQLITE_DONE)
			{
				ret = 0;
			}
			if (rc == SQLITE_ROW)
			{
				ret = 1;
			}

			sqlite3_finalize(stmt);
			return ret;
		}

		~Sqlite3() noexcept
		{
			if (db != nullptr)
			{
				sqlite3_close(db);
			}
		}

	private:
		::sqlite3* db;
	};


}

/****************************** MACROS ******************************/


/*********************** FUNCTION DECLARATIONS **********************/

