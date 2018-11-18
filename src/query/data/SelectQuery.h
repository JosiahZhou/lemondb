//
// Created by aiden on 18-10-31.
//

#ifndef PROJECT_SELECTQUERY_H
#define PROJECT_SELECTQUERY_H

#include "../Query.h"

class SelectQuery : public ComplexQuery {
    static constexpr const char *qname = "SELECT";
public:
    using ComplexQuery::ComplexQuery;

    QueryResult::Ptr execute() override;

    std::string toString() override;

    std::string cmdName() override;

    std::string tableName() override;

    bool isWrite() override;
};

#endif //PROJECT_SELECTQUERY_H
