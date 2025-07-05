#include "dao/affected_type_dict_dao.h"
#include "util/dao_util.h"
#include "constants/constants_sql.h"

using namespace dao_util;

// 查询所有影响类型字典
DaoResult queryAllAffectedTypeDict(sqlite3* db, std::vector<AffectedTypeDict>& list) {
    list.clear();
    sqlite3_stmt* stmt = nullptr;
    
    DAO_SAFE_PREPARE(db, constants_sql::SQL_SELECT_ALL_AFFECTED_TYPE_DICT, stmt, "queryAllAffectedTypeDict");
    
    DAO_SAFE_EXEC_SELECT(db, stmt, [&](sqlite3_stmt* s) {
        AffectedTypeDict d;
        d.id = sqlite3_column_int(s, 0);
        d.affected = reinterpret_cast<const char*>(sqlite3_column_text(s, 1));
        d.comment = reinterpret_cast<const char*>(sqlite3_column_text(s, 2));
        list.push_back(d);
    }, "queryAllAffectedTypeDict");
    
    return DaoResult::SUCCESS;
}

// 插入影响类型字典
DaoResult insertAffectedTypeDict(sqlite3* db, AffectedTypeDict& item) {
    sqlite3_stmt* stmt = nullptr;
    
    DAO_SAFE_PREPARE(db, constants_sql::SQL_INSERT_AFFECTED_TYPE_DICT, stmt, "insertAffectedTypeDict");
    
    sqlite3_bind_text(stmt, 1, item.affected.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, item.comment.c_str(), -1, SQLITE_STATIC);
    
    DAO_SAFE_EXEC_DONE(db, stmt, "insertAffectedTypeDict");
    
    item.id = sqlite3_last_insert_rowid(db);
    return DaoResult::SUCCESS;
}

// 更新影响类型字典
DaoResult updateAffectedTypeDict(sqlite3* db, const AffectedTypeDict& item) {
    sqlite3_stmt* stmt = nullptr;
    
    DAO_SAFE_PREPARE(db, constants_sql::SQL_UPDATE_AFFECTED_TYPE_DICT, stmt, "updateAffectedTypeDict");
    
    sqlite3_bind_text(stmt, 1, item.affected.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, item.comment.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, item.id);
    
    DAO_SAFE_EXEC_DONE(db, stmt, "updateAffectedTypeDict");
    
    return DaoResult::SUCCESS;
}

// 删除影响类型字典
DaoResult deleteAffectedTypeDict(sqlite3* db, int64_t id) {
    sqlite3_stmt* stmt = nullptr;
    
    DAO_SAFE_PREPARE(db, constants_sql::SQL_DELETE_AFFECTED_TYPE_DICT, stmt, "deleteAffectedTypeDict");
    
    sqlite3_bind_int(stmt, 1, id);
    
    DAO_SAFE_EXEC_DONE(db, stmt, "deleteAffectedTypeDict");
    
    return DaoResult::SUCCESS;
} 