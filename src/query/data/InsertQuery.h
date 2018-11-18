//
// Created by liu on 18-10-25.
//

#ifndef PROJECT_INSERTQUERY_H
#define PROJECT_INSERTQUERY_H

#include "../Query.h"

class InsertQuery : public ComplexQuery {
    static constexpr const char *qname = "INSERT";
public:
    using ComplexQuery::ComplexQuery;

    QueryResult::Ptr execute() override;

    std::string toString() override;

    std::string cmdName() override;

    std::string tableName() override;

    bool isWrite() override;
};


#endif //PROJECT_INSERTQUERY_H
