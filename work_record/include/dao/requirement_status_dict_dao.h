#pragma once
#include "model/requirement_status_dict_model.h"
#include <sqlite3/sqlite3.h>
#include <vector>

#include "util/dao_util.h"


dao_util::DaoResult queryAllRequirementStatusDict(sqlite3* db, std::vector<RequirementStatusDict>& list) ;

dao_util::DaoResult insertRequirementStatusDict(sqlite3* db, RequirementStatusDict& item) ;

dao_util::DaoResult updateRequirementStatusDict(sqlite3* db, const RequirementStatusDict& item) ;

dao_util::DaoResult deleteRequirementStatusDict(sqlite3* db, int64_t id) ;
