#include "dao/work_type_dict_dao.h"
#include "util/dao_util.h"
#include "constants/constants_sql.h"

using namespace dao_util;

// 查询所有工作类型字典
DaoResult queryAllWorkTypeDict(sqlite3* db, std::vector<WorkTypeDict>& list) {
    list.clear();
    sqlite3_stmt* stmt = nullptr;
    
    DAO_SAFE_PREPARE(db, constants_sql::SQL_SELECT_ALL_WORK_TYPE_DICT, stmt, "queryAllWorkTypeDict");
    
    DAO_SAFE_EXEC_SELECT(db, stmt, [&](sqlite3_stmt* s) {
        WorkTypeDict d;
        d.id = sqlite3_column_int(s, 0);
        d.type = reinterpret_cast<const char*>(sqlite3_column_text(s, 1));
        d.comment = reinterpret_cast<const char*>(sqlite3_column_text(s, 2));
        list.push_back(d);
    }, "queryAllWorkTypeDict");
    
    return DaoResult::SUCCESS;
}

// 插入工作类型字典
DaoResult insertWorkTypeDict(sqlite3* db, WorkTypeDict& item) {
    sqlite3_stmt* stmt = nullptr;
    
    DAO_SAFE_PREPARE(db, constants_sql::SQL_INSERT_WORK_TYPE_DICT, stmt, "insertWorkTypeDict");
    
    sqlite3_bind_text(stmt, 1, item.type.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, item.comment.c_str(), -1, SQLITE_STATIC);
    
    DAO_SAFE_EXEC_DONE(db, stmt, "insertWorkTypeDict");
    
    item.id = sqlite3_last_insert_rowid(db);
    return DaoResult::SUCCESS;
}

// 更新工作类型字典
DaoResult updateWorkTypeDict(sqlite3* db, const WorkTypeDict& item) {
    sqlite3_stmt* stmt = nullptr;
    
    DAO_SAFE_PREPARE(db, constants_sql::SQL_UPDATE_WORK_TYPE_DICT, stmt, "updateWorkTypeDict");
    
    sqlite3_bind_text(stmt, 1, item.type.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, item.comment.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, item.id);
    
    DAO_SAFE_EXEC_DONE(db, stmt, "updateWorkTypeDict");
    
    return DaoResult::SUCCESS;
}

// 删除工作类型字典
DaoResult deleteWorkTypeDict(sqlite3* db, int64_t id) {
    sqlite3_stmt* stmt = nullptr;
    
    DAO_SAFE_PREPARE(db, constants_sql::SQL_DELETE_WORK_TYPE_DICT, stmt, "deleteWorkTypeDict");
    
    sqlite3_bind_int64(stmt, 1, id);
    
    DAO_SAFE_EXEC_DONE(db, stmt, "deleteWorkTypeDict");
    
    return DaoResult::SUCCESS;
} 