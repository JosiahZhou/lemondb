//
// Created by liu on 18-10-25.
//

#ifndef PROJECT_LISTENQUERY_H
#define PROJECT_LISTENQUERY_H

#include "../Query.h"

class ListenQuery : public Query {
    static constexpr const char *qname = "LISTEN";
public:
    using Query::Query;

    QueryResult::Ptr execute() override;

    std::string toString() override;

    std::string cmdName() override;

    std::string tableName() override;

    bool isWrite() override;
};


#endif //PROJECT_LISTENQUERY_H
