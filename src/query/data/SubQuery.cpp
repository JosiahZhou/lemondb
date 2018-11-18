//
//  SubQuery.cpp
//  Project
//
//  Created by Josiah on 18/10/31.
//
//

#include "SubQuery.h"
#include "../../db/Database.h"
#include "../QueryResult.h"

#include <algorithm>

constexpr const char *SubQuery::qname;

QueryResult::Ptr SubQuery::execute() {
    using namespace std;
    if (this->operands.size() < 2)
        return make_unique<ErrorMsgResult>(
                                           qname, this->targetTable.c_str(),
                                           "Invalid number of operands (? operands)."_f % operands.size()
                                           );
    Database &db = Database::getInstance();
    vector<Table::FieldIndex> SubId;
    int SubResult = 0;
    Table::SizeType counter = 0;
    try {
        auto &table = db[this->targetTable];
        
        for (auto it = this->operands.begin(); it != this->operands.end(); ++it) {
            SubId.push_back(table.getFieldIndex(*it));
        }
        auto result = initCondition(table);
        if (result.second) {
            for (auto it = table.begin(); it != table.end(); ++it) {
                if (this->evalCondition(*it)) {
                    SubResult = (*it)[*SubId.begin()];
                    auto it2_end = SubId.end();
                    it2_end--;
                    for (auto it2 = ++SubId.begin(); it2 != it2_end; ++it2) {
                        SubResult -= (*it)[*it2];
                    }
                    (*it)[*it2_end] = SubResult;
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

std::string SubQuery::toString() {
    return "QUERY = SUB " + this->targetTable + "\"";
}

std::string SubQuery::cmdName() {
    return "SUB";
}

std::string SubQuery::tableName() {
    return this->targetTable;
}

bool SubQuery::isWrite() {
    return true;
}