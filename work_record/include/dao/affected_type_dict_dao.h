#pragma once

#include "model/affected_type_dict_model.h"
#include <sqlite3/sqlite3.h>
#include <vector>

std::vector<AffectedTypeDict> queryAllAffectedTypeDict(sqlite3* db);

bool insertAffectedTypeDict(sqlite3* db, AffectedTypeDict& item);

bool updateAffectedTypeDict(sqlite3* db, const AffectedTypeDict& item);

bool deleteAffectedTypeDict(sqlite3* db, int id);

