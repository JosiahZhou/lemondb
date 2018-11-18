//
//  SumQuery.cpp
//  Project
//
//  Created by Josiah on 18/10/31.
//
//

#include "SumQuery.h"
#include "../../db/Database.h"
#include "../QueryResult.h"

#include <algorithm>

constexpr const char *SumQuery::qname;

QueryResult::Ptr SumQuery::execute() {
    using namespace std;
    if (this->operands.empty())
        return make_unique<ErrorMsgResult>(
                                           qname, this->targetTable.c_str(),
                                           "Invalid number of operands (? operands)."_f % operands.size()
                                           );
    Database &db = Database::getInstance();
    vector<Table::FieldIndex> SumId;
    vector<int> FieldSum (this->operands.size(),0);
    Table::SizeType counter = 0;
    try {
        auto &table = db[this->targetTable];
        
        for (auto it = this->operands.begin(); it != this->operands.end(); ++it) {
            SumId.push_back(table.getFieldIndex(*it));
        }
        int size = this->operands.size();
        auto result = initCondition(table);
        if (result.second) {
            for (auto it = table.begin(); it != table.end(); ++it) {
                if (this->evalCondition(*it)) {
                    for (auto i = 0; i < size; i++) {
                        FieldSum[i] += (*it)[SumId[i]];
                    }
                    ++counter;
                }
            }
        }
        return make_unique<AnswerResult>(FieldSum);
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

std::string SumQuery::toString() {
    return "QUERY = SUM " + this->targetTable + "\"";
}

std::string SumQuery::cmdName() {
    return "SUM";
}

std::string SumQuery::tableName() {
    return this->targetTable;
}

bool SumQuery::isWrite() {
    return false;
}