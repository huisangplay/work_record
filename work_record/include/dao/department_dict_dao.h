#pragma once
#include "model/department_dict_model.h"
#include <sqlite3/sqlite3.h>
#include <vector>

#include "util/dao_util.h"

dao_util::DaoResult queryAllDepartmentDict(sqlite3* db, std::vector<DepartmentDict>& list) ;

dao_util::DaoResult insertDepartmentDict(sqlite3* db, DepartmentDict& item) ;

dao_util::DaoResult updateDepartmentDict(sqlite3* db, const DepartmentDict& item) ;

dao_util::DaoResult deleteDepartmentDict(sqlite3* db, int64_t id) ;