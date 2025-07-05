#pragma once
#include "model/work_type_dict_model.h"
#include <sqlite3/sqlite3.h>
#include <vector>
std::vector<WorkTypeDict> queryAllWorkTypeDict(sqlite3* db) ;

bool insertWorkTypeDict(sqlite3* db, WorkTypeDict& item) ;

bool updateWorkTypeDict(sqlite3* db, const WorkTypeDict& item) ;

bool deleteWorkTypeDict(sqlite3* db, int id) ;
