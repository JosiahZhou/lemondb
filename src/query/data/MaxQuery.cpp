//
//  MaxQuery.cpp
//  Project
//
//  Created by Josiah on 18/10/31.
//
//

#include "MaxQuery.h"
#include "../../db/Database.h"
#include "../QueryResult.h"

#include <algorithm>
#define int_max 2147483647
#define int_min -2147483648

constexpr const char *MaxQuery::qname;

QueryResult::Ptr MaxQuery::execute() {
    using namespace std;
    if (this->operands.empty())
        return make_unique<ErrorMsgResult>(
                                           qname, this->targetTable.c_str(),
                                           "Invalid number of operands (? operands)."_f % operands.size()
                                           );
    Database &db = Database::getInstance();
    vector<Table::FieldIndex> MaxId;
    vector<int> FieldMax (this->operands.size(),int_min);
    Table::SizeType counter = 0;
    try {
        auto &table = db[this->targetTable];
        
        for (auto it = this->operands.begin(); it != this->operands.end(); ++it) {
            MaxId.push_back(table.getFieldIndex(*it));
        }
        int size = this->operands.size();
        auto result = initCondition(table);
        if (result.second) {
            for (auto it = table.begin(); it != table.end(); ++it) {
                if (this->evalCondition(*it)) {
                    for (auto i = 0; i < size; i++) {
                        if (FieldMax[i] < (*it)[MaxId[i]])
                            FieldMax[i] = (*it)[MaxId[i]];
                    }
                    ++counter;
                }
            }
        }
        if (counter > 0)
            return make_unique<AnswerResult>(FieldMax);
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

std::string MaxQuery::toString() {
    return "QUERY = MAX " + this->targetTable + "\"";
}

std::string MaxQuery::cmdName() {
    return "MAX";
}

std::string MaxQuery::tableName() {
    return this->targetTable;
}

bool MaxQuery::isWrite() {
    return false;
}