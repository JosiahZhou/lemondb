//
// Created by aiden on 18-10-30.
//


#include "CopyTableQuery.h"
#include "../../db/Database.h"

#include <fstream>

constexpr const char *CopyTableQuery::qname;

QueryResult::Ptr CopyTableQuery::execute() {
    using namespace std;
    Database &db = Database::getInstance();
    try {
        auto &target = db[this->table];
        auto newTable = std::make_unique<Table>(this->newtable, target);
        db.registerTable(std::move(newTable));
        return make_unique<NullQueryResult>();
    } catch (const exception &e) {
        return make_unique<ErrorMsgResult>(qname, e.what());
    }
}

std::string CopyTableQuery::toString() {
    return "QUERY = COPY TABLE, TABLE = \"" + this->table + "\"";
}

std::string CopyTableQuery::cmdName() {
    return "COPYTABLE";
}

std::string CopyTableQuery::tableName() {
    return this->table;
}

std::string CopyTableQuery::getNewTableName() {
    return this->newtable;
}

bool CopyTableQuery::isWrite() {
    return true;
}
