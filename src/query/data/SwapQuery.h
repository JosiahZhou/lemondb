//
// Created by aiden on 18-10-31.
//

#ifndef PROJECT_SWAPQUERY_H
#define PROJECT_SWAPQUERY_H


#include "../Query.h"

class SwapQuery : public ComplexQuery {
    static constexpr const char *qname = "SWAP";
    Table::FieldIndex fieldId1;
    Table::FieldIndex fieldId2;
    Table::KeyType keyValue;
public:
    using ComplexQuery::ComplexQuery;

    QueryResult::Ptr execute() override;

    std::string toString() override;

    std::string cmdName() override;

    std::string tableName() override;

    bool isWrite() override;
};
#endif //PROJECT_SWAPQUERY_H
