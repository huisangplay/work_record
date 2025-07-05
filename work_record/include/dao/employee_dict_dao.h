#pragma once
#include "model/employee_dict_model.h"
#include <sqlite3/sqlite3.h>

#include "util/dao_util.h"

dao_util::DaoResult queryAllEmployeeDict(sqlite3* db, std::vector<EmployeeDict>& list) ;

dao_util::DaoResult insertEmployeeDict(sqlite3* db, EmployeeDict& item) ;

dao_util::DaoResult updateEmployeeDict(sqlite3* db, const EmployeeDict& item) ;

dao_util::DaoResult deleteEmployeeDict(sqlite3* db, int64_t id) ;

std::vector<EmployeeDict> queryEmployeeByDepartment(sqlite3* db, int64_t department_id) ;