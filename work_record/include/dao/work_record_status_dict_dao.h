#pragma once
#include "model/work_record_status_dict_model.h"
#include <sqlite3/sqlite3.h>
#include <vector>

#include "util/dao_util.h"

dao_util::DaoResult queryAllWorkRecordStatusDict(sqlite3 *db, std::vector<WorkRecordStatusDict>& list);

dao_util::DaoResult insertWorkRecordStatusDict(sqlite3 *db, WorkRecordStatusDict &item);

dao_util::DaoResult updateWorkRecordStatusDict(sqlite3 *db, const WorkRecordStatusDict &item);

dao_util::DaoResult deleteWorkRecordStatusDict(sqlite3 *db, int64_t id);
