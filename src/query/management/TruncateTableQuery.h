//
// Created by aiden on 18-10-30.
//

#ifndef PROJECT_TRUNCATETABLEQUERY_H
#define PROJECT_TRUNCATETABLEQUERY_H

#include "../Query.h"

class TruncateTableQuery : public Query {
    static constexpr const char *qname = "TRUNCATE";
public:

    using Query::Query;

    QueryResult::Ptr execute() override;

    std::string toString() override;

    std::string cmdName() override;

    std::string tableName() override;

    bool isWrite() override;
};

#endif //PROJECT_TRUNCATETABLEQUERY_H
