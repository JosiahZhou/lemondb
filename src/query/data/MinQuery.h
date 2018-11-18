//
// Created by Josiah on 18/10/31.
//

#ifndef PROJECT_MINQUERY_H
#define PROJECT_MINQUERY_H

#include "../Query.h"

class MinQuery : public ComplexQuery {
    static constexpr const char *qname = "MIN";
public:
    using ComplexQuery::ComplexQuery;
    
    QueryResult::Ptr execute() override;
    
    std::string toString() override;

    std::string cmdName() override;

    std::string tableName() override;

    bool isWrite() override;
};


#endif //PROJECT_MINQUERY_H
