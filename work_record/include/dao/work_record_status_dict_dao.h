#pragma once
#include "model/work_record_status_dict_model.h"
#include <sqlite3/sqlite3.h>
#include <vector>
std::vector<WorkRecordStatusDict> queryAllWorkRecordStatusDict(sqlite3* db) ;

bool insertWorkRecordStatusDict(sqlite3* db, WorkRecordStatusDict& item) ;

bool updateWorkRecordStatusDict(sqlite3* db, const WorkRecordStatusDict& item) ;

bool deleteWorkRecordStatusDict(sqlite3* db, int id) ;
