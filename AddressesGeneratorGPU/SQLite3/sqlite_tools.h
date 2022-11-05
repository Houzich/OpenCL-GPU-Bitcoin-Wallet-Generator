#pragma once
#include <cstdint>
#include <vector>
//#ifndef __SQLITE_TOOLS_H
//#define __SQLITE_TOOLS_H
#include "sqlite_db.h"
/**************************** DATA TYPES ****************************/


/****************************** MACROS ******************************/


/*********************** FUNCTION DECLARATIONS **********************/
extern const std::string table_name_header_H;
extern const std::string table_name_header_L;
extern std::vector<std::string> table_name;

std::string get_table_name_from_char(char chr);
int create_db(std::string db_filename);
int get_lines_from_txt(void);
int get_all_lines_from_txt(std::string filename, std::string db_filename_bip44, std::string db_filename_bip84, int num_lines);
int print_info(std::string db_filename);
//#endif	//__SQLITE_TOOLS_H