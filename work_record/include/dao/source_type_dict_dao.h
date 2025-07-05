#pragma once
#include "model/source_type_dict_model.h"
#include <sqlite3/sqlite3.h>
#include <vector>

#include "util/dao_util.h"

dao_util::DaoResult queryAllSourceTypeDict(sqlite3* db, std::vector<SourceTypeDict> &list) ;

dao_util::DaoResult insertSourceTypeDict(sqlite3* db, SourceTypeDict& item) ;

dao_util::DaoResult updateSourceTypeDict(sqlite3* db, const SourceTypeDict& item) ;

dao_util::DaoResult deleteSourceTypeDict(sqlite3* db, int64_t id) ;
