//
// Created by aiden on 18-10-30.
//


#include "TruncateTableQuery.h"
#include "../../db/Database.h"

constexpr const char *TruncateTableQuery::qname;

QueryResult::Ptr TruncateTableQuery::execute() {
    using namespace std;
    Database &db = Database::getInstance();
    try {
        db.truncateTable(this->targetTable);
        return make_unique<SuccessMsgResult>(qname);
    } catch (const TableNameNotFound &e) {
        return make_unique<ErrorMsgResult>(qname, targetTable, "No such table."s);
    } catch (const exception &e) {
        return make_unique<ErrorMsgResult>(qname, e.what());
    }
}

std::string TruncateTableQuery::toString() {
    return "QUERY = TRUNCATE, Table = \"" + targetTable + "\"";
}

std::string TruncateTableQuery::cmdName() {
    return "TRUNCATE";
}

std::string TruncateTableQuery::tableName() {
    return this->targetTable;
}

bool TruncateTableQuery::isWrite() {
    return true;
}