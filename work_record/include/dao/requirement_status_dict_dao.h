#pragma once
#include "model/requirement_status_dict_model.h"
#include <sqlite3/sqlite3.h>
#include <vector>


std::vector<RequirementStatusDict> queryAllRequirementStatusDict(sqlite3* db) ;

bool insertRequirementStatusDict(sqlite3* db, RequirementStatusDict& item) ;

bool updateRequirementStatusDict(sqlite3* db, const RequirementStatusDict& item) ;

bool deleteRequirementStatusDict(sqlite3* db, int id) ;
