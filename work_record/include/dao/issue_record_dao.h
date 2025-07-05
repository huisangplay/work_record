#pragma once
#include "model/issue_record_model.h"
#include <sqlite3/sqlite3.h>
#include <vector>

#include "util/dao_util.h"

// 查询所有问题记录
dao_util::DaoResult queryAllIssues(sqlite3 *dbqueryAllIssues, std::vector<IssueRecord> &records);

// 新增问题记录
dao_util::DaoResult insertIssue(sqlite3 *db, const IssueRecord &r, int64_t &newId);

// 修改问题记录
dao_util::DaoResult updateIssue(sqlite3 *db, const IssueRecord &r);

// 删除问题记录
dao_util::DaoResult deleteIssue(sqlite3 *db, int64_t id);

// 分页查询问题记录
dao_util::DaoResult queryIssuesPaged(sqlite3 *db, int page, int page_size, int &total,
                                     const std::string &source_type_id, const std::string &affected_id,
                                     std::vector<IssueRecord> &records);
