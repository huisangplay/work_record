#pragma once
#include "model/issue_progress_dict_model.h"
#include <sqlite3/sqlite3.h>
#include <vector>

std::vector<IssueProgressDict> queryAllIssueProgressDict(sqlite3* db) ;

bool insertIssueProgressDict(sqlite3* db, IssueProgressDict& item) ;

bool updateIssueProgressDict(sqlite3* db, const IssueProgressDict& item) ;

bool deleteIssueProgressDict(sqlite3* db, int id) ;