#pragma once
#include "model/department_dict_model.h"
#include <sqlite3/sqlite3.h>
#include <vector>

std::vector<DepartmentDict> queryAllDepartmentDict(sqlite3* db) ;

bool insertDepartmentDict(sqlite3* db, DepartmentDict& item) ;

bool updateDepartmentDict(sqlite3* db, const DepartmentDict& item) ;

bool deleteDepartmentDict(sqlite3* db, int id) ;