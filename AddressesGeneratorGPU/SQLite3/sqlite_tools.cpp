// TODO: Add OpenCL kernel code here.
#include "sqlite_tools.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
using namespace sqlite_db;

using std::cout; using std::cerr;
using std::endl; using std::string;
using std::ifstream; using std::vector;

const std::string table_name_header_H = "H";
const std::string table_name_header_L = "L";
std::vector<std::string> table_name = { "0","1","2","3","4","5","6","7","8","9","A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z","a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w","x","y","z"};

std::string get_table_name_from_char(char chr)
{
    std::string name;
    if (islower(chr)) {
        name = table_name_header_L;
        /*  Converting lowercase characters to uppercase  */
        name += chr - 32;
    }
    else
    {
        name = table_name_header_H;
        name += chr;
    }
    return name;
}

int create_db(std::string db_filename) {
    Sqlite3 db;
    try
    {
        db.open(db_filename);
        for (int i = 0; i < table_name.size(); i++) {
            std::string cmd;
            std::string name = get_table_name_from_char(table_name[i][0]);
            cmd = "CREATE TABLE IF NOT EXISTS ";
            cmd += name;
            cmd += "(ADDRESS TEXT NOT NULL);";
            db.exec(cmd);

            cmd = "CREATE UNIQUE INDEX IF NOT EXISTS IDX";
            cmd += name;
            cmd += " on ";
            cmd += name;
            cmd += "(ADDRESS);";
            db.exec(cmd);
        }
    }
    catch (const Sqlite3Exception& err)
    {
        std::cerr << err.what() << std::endl;
        //std::exit(1);
        return -1;
    }
    return 0;
}


int add_to_db(std::string db_filename_bip44, std::string db_filename_bip84, std::vector<std::string> lines) {
    Sqlite3 db_bip44;
    Sqlite3 db_bip84;
    try
    {
        db_bip44.open(db_filename_bip44);
        db_bip84.open(db_filename_bip84);
    }
    catch (const Sqlite3Exception& err)
    {
        std::cerr << err.what() << std::endl;
        //std::exit(1);
        return -1;
    }
    for each (string line in lines) {
        std::string table_name;
        if (line[0] == '1')
        {
            table_name = get_table_name_from_char(line[1]);
            db_bip44.insert_address(table_name, line);
        }
        else if (line[0] == 'b')
        {
            table_name = get_table_name_from_char(line[4]);
            db_bip84.insert_address(table_name, line);
        }
        else
        {
            std::cerr << "\n!!!ERROR line: " << line << std::endl;
            //std::exit(1);
            return -1;
        }
    }
    return 0;
}

int add_to_db(Sqlite3& db_bip44, Sqlite3& db_bip84, std::vector<std::string>& lines) {
    for each (string line in lines) {
        std::string table_name;
        if (line[0] == '1')
        {
            table_name = get_table_name_from_char(line[1]);
            db_bip44.insert_address(table_name, line);
        }
        else if (line[0] == 'b')
        {
            table_name = get_table_name_from_char(line[4]);
            db_bip84.insert_address(table_name, line);
        }
        else
        {
            std::cerr << "\n!!!ERROR line: " << line << std::endl;
            //std::exit(1);
            return -1;
        }
    }
    return 0;
}

int get_lines_from_txt(void) {
    std::string filename = "111.txt";
    std::ifstream input_file(filename, std::ios::app);
    string line;
    int count = 2000;
    std::vector<std::string> lines;
	if (input_file.is_open())
	{
        while ((getline(input_file, line))&&((count--) > 0)) {
            lines.push_back(line);
        }
	}
	else
	{
        std::cerr << "\n!!!ERROR open file: " << filename << std::endl;
        //std::exit(1);
        return -1;
    }
    input_file.close();
    return 0;
}

int get_all_lines_from_txt(std::string filename, std::string db_filename_bip44, std::string db_filename_bip84, int num_lines) {
    std::ifstream input_file;
    Sqlite3 db_bip44;
    Sqlite3 db_bip84;

    input_file.open(filename, std::ifstream::in);

    try
    {
        db_bip44.open(db_filename_bip44);
        db_bip84.open(db_filename_bip84);
    }
    catch (const Sqlite3Exception& err)
    {
        std::cerr << err.what() << std::endl;
        //std::exit(1);
        return -1;
    }
    string line;

    if (input_file.is_open())
    {
        int all_count = 0;
        std::cout << "PROCESSED 0 ROWS";
        while (1) {
            int count = num_lines;
            std::vector<std::string> lines;
            while ((getline(input_file, line)) && ((count--) > 0)) {
                lines.push_back(line);
            }
            add_to_db(db_bip44, db_bip84, lines);
            all_count += lines.size();
            std::cout << "\r";
            std::cout << "PROCESSED " << all_count << " ROWS";
            if (count != -1) break;
        }
    }
    else
    {
        std::cerr << "\n!!!ERROR open file: " << filename << std::endl;
        //std::exit(1);
        return -1;
    }
    std::cout << "\n";
    input_file.close();
    return 0;
}

int print_info(std::string db_filename) {
    Sqlite3 db;
    int all_rows = 0;
    try
    {
        db.open(db_filename);
        for (int i = 0; i < table_name.size(); i++) {
            std::string tbl_name = get_table_name_from_char(table_name[i][0]);
            int rows = db.get_rows(tbl_name);
            std::cout << "in " + tbl_name + " is " << rows << " rows" << std::endl;
            all_rows += rows;
        }
        std::cout << "NUM ALL ROWS IS: " << all_rows << std::endl;
    }
    catch (const Sqlite3Exception& err)
    {
        std::cerr << err.what() << std::endl;
        //std::exit(1);
        return -1;
    }

    return 0;
}





