//
// Created by aiden on 18-10-30.
//

#ifndef PROJECT_COPYTABLEQUERY_H
#define PROJECT_COPYTABLEQUERY_H

#include "../Query.h"

class CopyTableQuery : public Query {
    static constexpr const char *qname = "COPYTABLE";
    const std::string table;
    const std::string newtable;

public:

    explicit CopyTableQuery(std::string table, std::string newtable) :
            table(std::move(table)), newtable(std::move(newtable)) {}

    QueryResult::Ptr execute() override;

    std::string toString() override;

    std::string cmdName() override;

    std::string tableName() override;
    
    std::string getNewTableName() override;

    bool isWrite() override;
};

#endif //PROJECT_COPYTABLEQUERY_H
