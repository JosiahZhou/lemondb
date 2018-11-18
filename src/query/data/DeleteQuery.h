//
// Created by aiden on 18-10-31.
//

#ifndef PROJECT_DELETEQUERY_H
#define PROJECT_DELETEQUERY_H

#include "../Query.h"

class DeleteQuery : public ComplexQuery {
    static constexpr const char *qname = "DELETE";
    Table::KeyType keyValue;
public:
    using ComplexQuery::ComplexQuery;

    QueryResult::Ptr execute() override;

    std::string toString() override;

    std::string cmdName() override;

    std::string tableName() override;

    bool isWrite() override;
};

#endif //PROJECT_DELETEQUERY_H
