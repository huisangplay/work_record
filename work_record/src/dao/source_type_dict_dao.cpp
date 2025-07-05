#include "dao/source_type_dict_dao.h"
#include "util/dao_util.h"
#include "constants/constants_sql.h"

using namespace dao_util;

// 查询所有来源类型字典
DaoResult queryAllSourceTypeDict(sqlite3 *db, std::vector<SourceTypeDict> &list) {
    list.clear();
    sqlite3_stmt *stmt = nullptr;

    DAO_SAFE_PREPARE(db, constants_sql::SQL_SELECT_ALL_SOURCE_TYPE_DICT, stmt, "queryAllSourceTypeDict");

    DAO_SAFE_EXEC_SELECT(db, stmt, [&](sqlite3_stmt* s) {
                         SourceTypeDict d;
                         d.id = sqlite3_column_int(s, 0);
                         d.type = reinterpret_cast<const char*>(sqlite3_column_text(s, 1));
                         d.comment = reinterpret_cast<const char*>(sqlite3_column_text(s, 2));
                         list.push_back(d);
                         }, "queryAllSourceTypeDict");

    return DaoResult::SUCCESS;
}

// 插入来源类型字典
DaoResult insertSourceTypeDict(sqlite3 *db, SourceTypeDict &item) {
    sqlite3_stmt *stmt = nullptr;

    DAO_SAFE_PREPARE(db, constants_sql::SQL_INSERT_SOURCE_TYPE_DICT, stmt, "insertSourceTypeDict");

    sqlite3_bind_text(stmt, 1, item.type.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, item.comment.c_str(), -1, SQLITE_STATIC);

    DAO_SAFE_EXEC_DONE(db, stmt, "insertSourceTypeDict");

    item.id = sqlite3_last_insert_rowid(db);
    return DaoResult::SUCCESS;
}

// 更新来源类型字典
DaoResult updateSourceTypeDict(sqlite3 *db, const SourceTypeDict &item) {
    sqlite3_stmt *stmt = nullptr;

    DAO_SAFE_PREPARE(db, constants_sql::SQL_UPDATE_SOURCE_TYPE_DICT, stmt, "updateSourceTypeDict");

    sqlite3_bind_text(stmt, 1, item.type.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, item.comment.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, item.id);

    DAO_SAFE_EXEC_DONE(db, stmt, "updateSourceTypeDict");

    return DaoResult::SUCCESS;
}

// 删除来源类型字典
DaoResult deleteSourceTypeDict(sqlite3 *db, int64_t id) {
    sqlite3_stmt *stmt = nullptr;

    DAO_SAFE_PREPARE(db, constants_sql::SQL_DELETE_SOURCE_TYPE_DICT, stmt, "deleteSourceTypeDict");

    sqlite3_bind_int(stmt, 1, id);

    DAO_SAFE_EXEC_DONE(db, stmt, "deleteSourceTypeDict");

    return DaoResult::SUCCESS;
}
