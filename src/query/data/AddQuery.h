//
// Created by Josiah on 18/10/31.
//

#ifndef PROJECT_ADDQUERY_H
#define PROJECT_ADDQUERY_H

#include "../Query.h"

class AddQuery : public ComplexQuery {
    static constexpr const char *qname = "ADD";
public:
    using ComplexQuery::ComplexQuery;

    QueryResult::Ptr execute() override;

    std::string toString() override;

    std::string cmdName() override;

    std::string tableName() override;

    bool isWrite() override;
};


#endif //PROJECT_ADDQUERY_H
