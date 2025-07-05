#pragma once
#include "model/issue_progress_dict_model.h"
#include <sqlite3/sqlite3.h>
#include <vector>

#include "util/dao_util.h"

dao_util::DaoResult queryAllIssueProgressDict(sqlite3* db, std::vector<IssueProgressDict>& list) ;

dao_util::DaoResult insertIssueProgressDict(sqlite3* db, IssueProgressDict& item) ;

dao_util::DaoResult updateIssueProgressDict(sqlite3* db, const IssueProgressDict& item) ;

dao_util::DaoResult deleteIssueProgressDict(sqlite3* db, int64_t id) ;