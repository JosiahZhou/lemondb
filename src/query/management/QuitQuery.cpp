//
// Created by liu on 18-10-25.
//

#include "QuitQuery.h"
#include "../../db/Database.h"

constexpr const char *QuitQuery::qname;

std::string QuitQuery::toString() {
    return "QUERY = Quit";
}

QueryResult::Ptr QuitQuery::execute() {
    auto &db = Database::getInstance();
    db.exit();
    // might not reach here, but we want to keep the consistency of queries
    return std::make_unique<SuccessMsgResult>(qname);
}

std::string QuitQuery::cmdName() {
    return "QUIT";
}

std::string QuitQuery::tableName() {
    return this->targetTable;
}

bool QuitQuery::isWrite() {
    return true;
}