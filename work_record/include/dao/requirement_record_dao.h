#pragma once
#include "model/requirement_record_model.h"
#include <sqlite3/sqlite3.h>
#include <vector>

#include "util/dao_util.h"

// 查询所有需求记录
dao_util::DaoResult queryAllRequirementRecords(sqlite3* db, std::vector<RequirementRecord>& records) ;

// 根据ID获取RequirementRecord
dao_util::DaoResult getRequirementById(sqlite3* db, int64_t id, RequirementRecord& record) ;

// requirement_dao.h 内联实现，解决链接错误
dao_util::DaoResult insertRequirementRecord(sqlite3* db, const RequirementRecord& record, int64_t& newId);

dao_util::DaoResult updateRequirementRecord(sqlite3* db, const RequirementRecord& record) ;

// 统计某需求下工作记录数量
dao_util::DaoResult countWorkRecordByRequirement(sqlite3* db, int64_t requirementId, int& count) ;

// 删除需求记录
dao_util::DaoResult deleteRequirementRecord(sqlite3* db, int64_t id) ;

// 分页查询需求记录
dao_util::DaoResult queryRequirementRecordsPaged(sqlite3* db, int page, int page_size, int& total, const std::string& status_id, const std::string& affected_id, const std::string& source_type_id, std::vector<RequirementRecord>& records) ;

// 获取需求相关的所有文件路径
 std::vector<std::string> getAllFilesByRequirement(sqlite3* db, int64_t requirement_id) ;

