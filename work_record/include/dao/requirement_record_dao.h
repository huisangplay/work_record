#pragma once
#include "model/requirement_record_model.h"
#include <sqlite3/sqlite3.h>
#include <vector> 
// 查询所有需求记录
std::vector<RequirementRecord> queryAllRequirementRecordsDao(sqlite3* db) ;

// 根据ID获取RequirementRecord
bool getRequirementById(sqlite3* db, int id, RequirementRecord& record) ;

// requirement_dao.h 内联实现，解决链接错误
bool insertRequirementRecord(sqlite3* db, const RequirementRecord& record, int& newId);

void updateRequirementRecord(sqlite3* db, const RequirementRecord& record) ;

// 统计某需求下工单数量
int countWorkRecordByRequirement(sqlite3* db, int requirementId) ;

// 删除需求记录
bool deleteRequirementRecord(sqlite3* db, int id) ;

// 分页查询需求记录
std::vector<RequirementRecord> queryRequirementRecordsPagedDao(sqlite3* db, int page, int page_size, int& total, const std::string& status_id, const std::string& affected_id, const std::string& source_type_id) ;

// 获取需求相关的所有文件路径
 std::vector<std::string> getAllFilesByRequirement(sqlite3* db, int requirement_id) ;

