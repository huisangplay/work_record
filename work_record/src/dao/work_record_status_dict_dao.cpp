#include "dao/work_record_status_dict_dao.h"
#include "util/dao_util.h"
#include "constants/constants_sql.h"

using namespace dao_util;

// 查询所工作记录状态字典
DaoResult queryAllWorkRecordStatusDict(sqlite3* db, std::vector<WorkRecordStatusDict>& list) {
    list.clear();
    sqlite3_stmt* stmt = nullptr;
    
    DAO_SAFE_PREPARE(db, constants_sql::SQL_SELECT_ALL_WORK_RECORD_STATUS_DICT, stmt, "queryAllWorkRecordStatusDict");
    
    DAO_SAFE_EXEC_SELECT(db, stmt, [&](sqlite3_stmt* s) {
        WorkRecordStatusDict d;
        d.id = sqlite3_column_int(s, 0);
        d.status_name = reinterpret_cast<const char*>(sqlite3_column_text(s, 1));
        d.status_class = reinterpret_cast<const char*>(sqlite3_column_text(s, 2));
        list.push_back(d);
    }, "queryAllWorkRecordStatusDict");
    
    return DaoResult::SUCCESS;
}

// 插入工作记录状态字典
DaoResult insertWorkRecordStatusDict(sqlite3* db, WorkRecordStatusDict& item) {
    sqlite3_stmt* stmt = nullptr;
    
    DAO_SAFE_PREPARE(db, constants_sql::SQL_INSERT_WORK_RECORD_STATUS_DICT, stmt, "insertWorkRecordStatusDict");
    
    sqlite3_bind_text(stmt, 1, item.status_name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, item.status_class.c_str(), -1, SQLITE_STATIC);
    
    DAO_SAFE_EXEC_DONE(db, stmt, "insertWorkRecordStatusDict");
    
    item.id = sqlite3_last_insert_rowid(db);
    return DaoResult::SUCCESS;
}

// 更新工作记录状态字典
DaoResult updateWorkRecordStatusDict(sqlite3* db, const WorkRecordStatusDict& item) {
    sqlite3_stmt* stmt = nullptr;
    
    DAO_SAFE_PREPARE(db, constants_sql::SQL_UPDATE_WORK_RECORD_STATUS_DICT, stmt, "updateWorkRecordStatusDict");
    
    sqlite3_bind_text(stmt, 1, item.status_name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, item.status_class.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, item.id);
    
    DAO_SAFE_EXEC_DONE(db, stmt, "updateWorkRecordStatusDict");
    
    return DaoResult::SUCCESS;
}

// 删除工作记录状态字典
DaoResult deleteWorkRecordStatusDict(sqlite3* db, int64_t id) {
    sqlite3_stmt* stmt = nullptr;
    
    DAO_SAFE_PREPARE(db, constants_sql::SQL_DELETE_WORK_RECORD_STATUS_DICT, stmt, "deleteWorkRecordStatusDict");
    
    sqlite3_bind_int(stmt, 1, id);
    
    DAO_SAFE_EXEC_DONE(db, stmt, "deleteWorkRecordStatusDict");
    
    return DaoResult::SUCCESS;
} 