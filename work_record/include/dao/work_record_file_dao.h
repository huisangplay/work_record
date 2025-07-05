#pragma once
#include <sqlite3/sqlite3.h>

bool insertWorkFileRel(sqlite3* db, int workId, int fileId) ;

 bool deleteWorkFileRelByWork(sqlite3* db, int workId) ;
