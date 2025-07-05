#pragma once
#include <sqlite3/sqlite3.h>

#include "util/dao_util.h"

dao_util::DaoResult insertWorkFileRel(sqlite3 *db, int workId, int fileId);

dao_util::DaoResult deleteWorkFileRelByWork(sqlite3 *db, int workId);
