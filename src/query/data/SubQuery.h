//
// Created by Josiah on 18/10/31.
//

#ifndef PROJECT_SUBQUERY_H
#define PROJECT_SUBQUERY_H

#include "../Query.h"

class SubQuery : public ComplexQuery {
    static constexpr const char *qname = "SUB";
public:
    using ComplexQuery::ComplexQuery;

    QueryResult::Ptr execute() override;

    std::string toString() override;

    std::string cmdName() override;

    std::string tableName() override;

    bool isWrite() override;
};


#endif //PROJECT_SUBQUERY_H
