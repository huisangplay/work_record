#pragma once
#include "model/file_record_model.h"
#include <sqlite3/sqlite3.h>

#include "util/dao_util.h"

dao_util::DaoResult insertFileRecord(sqlite3* db, const FileRecord& fileRec, int64_t& newId) ;

dao_util::DaoResult deleteFileRecord(sqlite3* db, int64_t fileId) ;
// 根据ID获取FileRecord
dao_util::DaoResult getFileRecordById(sqlite3* db, int64_t id, FileRecord& record) ;
