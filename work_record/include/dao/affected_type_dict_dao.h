#pragma once

#include "model/affected_type_dict_model.h"
#include <sqlite3/sqlite3.h>
#include <vector>
#include "util/dao_util.h"
dao_util::DaoResult  queryAllAffectedTypeDict(sqlite3* db, std::vector<AffectedTypeDict>& list);

dao_util::DaoResult insertAffectedTypeDict(sqlite3* db, AffectedTypeDict& item);

dao_util::DaoResult updateAffectedTypeDict(sqlite3* db, const AffectedTypeDict& item);

dao_util::DaoResult deleteAffectedTypeDict(sqlite3* db, int64_t id);

