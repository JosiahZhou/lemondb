//
//  SumQuery.cpp
//  Project
//
//  Created by Josiah on 18/10/31.
//
//

#include "CountQuery.h"
#include "../../db/Database.h"
#include "../QueryResult.h"

constexpr const char *CountQuery::qname;

QueryResult::Ptr CountQuery::execute() {
    using namespace std;
    Database &db = Database::getInstance();
    Table::ValueType counter = 0;
    try {
        auto &table = db[this->targetTable];
        
        auto result = initCondition(table);
        if (result.second) {
            for (auto it = table.begin(); it != table.end(); ++it) {
                if (this->evalCondition(*it)) {
                    ++counter;
                }
            }
        }
        return make_unique<AnswerResult>(counter);
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

std::string CountQuery::toString() {
    return "QUERY = COUNT " + this->targetTable + "\"";
}

std::string CountQuery::cmdName() {
    return "COUNT";
}

std::string CountQuery::tableName() {
    return this->targetTable;
}

bool CountQuery::isWrite(){
    return false;
}
