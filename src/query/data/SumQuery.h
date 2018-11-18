//
// Created by Josiah on 18/10/31.
//

#ifndef PROJECT_SUMQUERY_H
#define PROJECT_SUMQUERY_H

#include "../Query.h"

class SumQuery : public ComplexQuery {
    static constexpr const char *qname = "SUM";
public:
    using ComplexQuery::ComplexQuery;

    QueryResult::Ptr execute() override;

    std::string toString() override;

    std::string cmdName() override;

    std::string tableName() override;

    bool isWrite() override;
};


#endif //PROJECT_SUMQUERY_H
