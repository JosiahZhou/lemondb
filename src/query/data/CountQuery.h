//
// Created by Josiah on 18/10/31.
//

#ifndef PROJECT_COUNTQUERY_H
#define PROJECT_COUNTQUERY_H

#include "../Query.h"

class CountQuery : public ComplexQuery {
    static constexpr const char *qname = "COUNT";
public:
    using ComplexQuery::ComplexQuery;

    QueryResult::Ptr execute() override;

    std::string toString() override;

    std::string cmdName() override;

    std::string tableName() override;

    bool isWrite() override;

};


#endif //PROJECT_COUNTQUERY_H
