//
// Created by liu on 18-10-25.
//

#ifndef PROJECT_LISTTABLEQUERY_H
#define PROJECT_LISTTABLEQUERY_H

#include "../Query.h"

class ListTableQuery : public Query {
    static constexpr const char *qname = "LIST";
public:
    QueryResult::Ptr execute() override;

    std::string toString() override;

    std::string cmdName() override;

    std::string tableName() override;

    bool isWrite() override;
};


#endif //PROJECT_LISTTABLEQUERY_H
