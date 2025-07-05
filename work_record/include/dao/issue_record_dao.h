#pragma once
#include "model/issue_record_model.h"
#include <sqlite3/sqlite3.h>
#include <vector>
// 查询所有问题记录
std::vector<IssueRecord> queryAllIssuesDao(sqlite3* db) ;

// 新增问题记录
void insertIssueDao(sqlite3* db, const IssueRecord& r, int& newId) ;

// 修改问题记录
void updateIssueDao(sqlite3* db, const IssueRecord& r) ;

// 删除问题记录
bool deleteIssueDao(sqlite3* db, int id) ;

// 分页查询问题记录
std::vector<IssueRecord> queryIssuesPagedDao(sqlite3* db, int page, int page_size, int& total, const std::string& source_type_id, const std::string& affected_id);
