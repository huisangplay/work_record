#include "dao/requirement_status_dict_dao.h"
#include "util/dao_util.h"
#include "constants/constants_sql.h"

using namespace dao_util;

// 查询所有需求状态字典
DaoResult queryAllRequirementStatusDict(sqlite3* db, std::vector<RequirementStatusDict>& list) {
    list.clear();
    sqlite3_stmt* stmt = nullptr;
    
    DAO_SAFE_PREPARE(db, constants_sql::SQL_SELECT_ALL_REQUIREMENT_STATUS_DICT, stmt, "queryAllRequirementStatusDict");
    
    DAO_SAFE_EXEC_SELECT(db, stmt, [&](sqlite3_stmt* s) {
        RequirementStatusDict d;
        d.id = sqlite3_column_int(s, 0);
        d.status = reinterpret_cast<const char*>(sqlite3_column_text(s, 1));
        d.comment = reinterpret_cast<const char*>(sqlite3_column_text(s, 2));
        d.requirement_status_class = reinterpret_cast<const char*>(sqlite3_column_text(s, 3));
        list.push_back(d);
    }, "queryAllRequirementStatusDict");
    
    return DaoResult::SUCCESS;
}

// 插入需求状态字典
DaoResult insertRequirementStatusDict(sqlite3* db, RequirementStatusDict& item) {
    sqlite3_stmt* stmt = nullptr;
    
    DAO_SAFE_PREPARE(db, constants_sql::SQL_INSERT_REQUIREMENT_STATUS_DICT, stmt, "insertRequirementStatusDict");
    
    sqlite3_bind_text(stmt, 1, item.status.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, item.comment.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, item.requirement_status_class.c_str(), -1, SQLITE_STATIC);
    
    DAO_SAFE_EXEC_DONE(db, stmt, "insertRequirementStatusDict");
    
    item.id = sqlite3_last_insert_rowid(db);
    return DaoResult::SUCCESS;
}

// 更新需求状态字典
DaoResult updateRequirementStatusDict(sqlite3* db, const RequirementStatusDict& item) {
    sqlite3_stmt* stmt = nullptr;
    
    DAO_SAFE_PREPARE(db, constants_sql::SQL_UPDATE_REQUIREMENT_STATUS_DICT, stmt, "updateRequirementStatusDict");
    
    sqlite3_bind_text(stmt, 1, item.status.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, item.comment.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, item.requirement_status_class.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, item.id);
    
    DAO_SAFE_EXEC_DONE(db, stmt, "updateRequirementStatusDict");
    
    return DaoResult::SUCCESS;
}

// 删除需求状态字典
DaoResult deleteRequirementStatusDict(sqlite3* db, int64_t id) {
    sqlite3_stmt* stmt = nullptr;
    
    DAO_SAFE_PREPARE(db, constants_sql::SQL_DELETE_REQUIREMENT_STATUS_DICT, stmt, "deleteRequirementStatusDict");
    
    sqlite3_bind_int(stmt, 1, id);
    
    DAO_SAFE_EXEC_DONE(db, stmt, "deleteRequirementStatusDict");
    
    return DaoResult::SUCCESS;
} 