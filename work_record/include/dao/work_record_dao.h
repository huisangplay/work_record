#pragma once
#include "model/work_record_model.h"
#include <sqlite3/sqlite3.h>

bool insertWorkRecord(sqlite3* db, const WorkRecord& record, int& newId) ;

// 根据ID获取WorkRecord
bool getWorkRecordById(sqlite3* db, int id, WorkRecord& record) ;

// 更新工单
bool updateWorkRecord(sqlite3* db, const WorkRecord& record) ;

// 分页+过滤查询工单
std::vector<WorkRecord> queryWorkRecordsPagedDao(sqlite3* db, const std::string& scope, int page, int page_size, int& total, const std::string& status_id, const std::string& affected_id, const std::string& source_type_id, const std::string& requirement_id, const std::string& work_type_id);
