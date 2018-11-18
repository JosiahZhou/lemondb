//
//  AddQuery.cpp
//  Project
//
//  Created by Josiah on 18/10/31.
//
//

#include "AddQuery.h"
#include "../../db/Database.h"
#include "../QueryResult.h"

#include <algorithm>

constexpr const char *AddQuery::qname;

QueryResult::Ptr AddQuery::execute() {
    using namespace std;
    if (this->operands.size() < 2)
        return make_unique<ErrorMsgResult>(
                                           qname, this->targetTable.c_str(),
                                           "Invalid number of operands (? operands)."_f % operands.size()
                                           );
    Database &db = Database::getInstance();
    vector<Table::FieldIndex> AddId;
    int AddResult = 0;
    Table::SizeType counter = 0;
    try {
        auto &table = db[this->targetTable];
        
        for (auto it = this->operands.begin(); it != this->operands.end(); ++it) {
            AddId.push_back(table.getFieldIndex(*it));
        }
        auto result = initCondition(table);
        if (result.second) {
            for (auto it = table.begin(); it != table.end(); ++it) {
                if (this->evalCondition(*it)) {
                    AddResult = 0;
                    auto it2_end = AddId.end();
                    it2_end--;
                    for (auto it2 = AddId.begin(); it2 != it2_end; ++it2) {
                        AddResult += (*it)[*it2];
                    }
                    (*it)[*it2_end] = AddResult;
                    ++counter;
                }
            }
        }
        return make_unique<RecordCountResult>(counter);
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

std::string AddQuery::toString() {
    return "QUERY = ADD " + this->targetTable + "\"";
}

std::string AddQuery::cmdName() {
    return "ADD";
}

std::string AddQuery::tableName() {
    return this->targetTable;
}

bool AddQuery::isWrite(){
    return true;
}