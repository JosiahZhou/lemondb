//
// Created by aiden on 18-10-31.
//

#ifndef PROJECT_DUPLICATEQUERY_H
#define PROJECT_DUPLICATEQUERY_H

#include "../Query.h"

class DuplicateQuery : public ComplexQuery {
    static constexpr const char *qname = "DUPLICATE";
    Table::KeyType keyValue;
public:
    using ComplexQuery::ComplexQuery;

    QueryResult::Ptr execute() override;

    std::string toString() override;

    std::string cmdName() override;

    std::string tableName() override;

    bool isWrite() override;
};

#endif //PROJECT_DUPLICATEQUERY_H
