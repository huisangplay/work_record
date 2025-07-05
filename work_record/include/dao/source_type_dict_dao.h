#pragma once
#include "model/source_type_dict_model.h"
#include <sqlite3/sqlite3.h>
#include <vector>
std::vector<SourceTypeDict> queryAllSourceTypeDict(sqlite3* db) ;

bool insertSourceTypeDict(sqlite3* db, SourceTypeDict& item) ;

bool updateSourceTypeDict(sqlite3* db, const SourceTypeDict& item) ;

bool deleteSourceTypeDict(sqlite3* db, int id) ;
