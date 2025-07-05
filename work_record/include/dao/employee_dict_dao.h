#pragma once
#include "model/employee_dict_model.h"
#include <sqlite3/sqlite3.h>

std::vector<EmployeeDict> queryAllEmployeeDict(sqlite3* db) ;

bool insertEmployeeDict(sqlite3* db, EmployeeDict& item) ;

bool updateEmployeeDict(sqlite3* db, const EmployeeDict& item) ;

bool deleteEmployeeDict(sqlite3* db, int id) ;

std::vector<EmployeeDict> queryEmployeeByDepartment(sqlite3* db, int department_id) ;