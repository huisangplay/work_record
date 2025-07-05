#pragma once
#include "model/file_record_model.h"
#include <sqlite3/sqlite3.h>

bool insertFileRecord(sqlite3* db, const FileRecord& fileRec, int& newId) ;

bool deleteFileRecord(sqlite3* db, int fileId) ;
// 根据ID获取FileRecord
bool getFileRecordById(sqlite3* db, int id, FileRecord& record) ;
