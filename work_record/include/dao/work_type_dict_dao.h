#pragma once
#include "model/work_type_dict_model.h"
#include <sqlite3/sqlite3.h>
#include <vector>

#include "util/dao_util.h"

dao_util::DaoResult queryAllWorkTypeDict(sqlite3 *db, std::vector<WorkTypeDict>& list);

dao_util::DaoResult insertWorkTypeDict(sqlite3 *db, WorkTypeDict &item);

dao_util::DaoResult updateWorkTypeDict(sqlite3 *db, const WorkTypeDict &item);

dao_util::DaoResult deleteWorkTypeDict(sqlite3 *db, int64_t id);
