//
// Created by zhou on 18-11-17.
//

#include "ListenQuery.h"
#include "../../db/Database.h"

constexpr const char *ListenQuery::qname;

std::string ListenQuery::toString() {
    return "QUERY = LISTEN";
}

QueryResult::Ptr ListenQuery::execute() {
    std::string pathOfFile = this->tableName();
    int pos = pathOfFile.find_last_of('/');
    std::string s(pathOfFile.substr(pos+1));
    return std::make_unique<AnswerResult>(s);
}

std::string ListenQuery::cmdName() {
    return "LISTEN";
}

std::string ListenQuery::tableName() {
    return this->targetTable;
}

bool ListenQuery::isWrite() {
    return true;
}
