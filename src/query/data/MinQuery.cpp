//
//  MinQuery.cpp
//  Project
//
//  Created by Josiah on 18/10/31.
//
//

#include "MinQuery.h"
#include "../../db/Database.h"
#include "../QueryResult.h"

#include <algorithm>
#include <iostream>
#define int_max 2147483647
#define int_min -2147483648

constexpr const char *MinQuery::qname;

QueryResult::Ptr MinQuery::execute() {
    using namespace std;
    if (this->operands.empty())
        return make_unique<ErrorMsgResult>(
                                           qname, this->targetTable.c_str(),
                                           "Invalid number of operands (? operands)."_f % operands.size()
                                           );
    Database &db = Database::getInstance();
    vector<Table::FieldIndex> MinId;
    vector<int> FieldMin (this->operands.size(),int_max);
    Table::SizeType counter = 0;
    try {
        auto &table = db[this->targetTable];
        
        for (auto it = this->operands.begin(); it != this->operands.end(); ++it) {
            MinId.push_back(table.getFieldIndex(*it));
        }
        int size = this->operands.size();
        auto result = initCondition(table);
        if (result.second) {
            for (auto it = table.begin(); it != table.end(); ++it) {
                if (this->evalCondition(*it)) {
                    for (auto i = 0; i < size; i++) {
                        if (FieldMin[i] > (*it)[MinId[i]])
                            FieldMin[i] = (*it)[MinId[i]];
                    }
                    ++counter;
                }
            }
        }
        if (counter > 0)
            return make_unique<AnswerResult>(FieldMin);
        else return make_unique<NullQueryResult>();
    } catch (const TableNameNotFound &e) {
        return make_unique<ErrorMsgResult>(qname, this->targetTable, "No such table."s);
    } catch (const IllFormedQueryCondition &e) {
        return make_unique<ErrorMsgResult>(qname, this->targetTable, e.what());
    } catch (const invalid_argument &e) {
        // Cannot convert operand to string
        return make_unique<ErrorMsgResult>(qname, this->targetTable, "Unknown error '?'"_f % e.what());
    } catch (const exception &e) {
        return make_unique<ErrorMsgResult>(qname, this->targetTable, "Unkonwn error '?'."_f % e.what());
    }
}

std::string MinQuery::toString() {
    return "QUERY = MIN " + this->targetTable + "\"";
}

std::string MinQuery::cmdName() {
    return "MIN";
}

std::string MinQuery::tableName() {
    return this->targetTable;
}

bool MinQuery::isWrite() {
    return false;
}