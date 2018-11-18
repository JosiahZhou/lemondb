//
// Created by aiden on 18-10-31.
//

#include "DeleteQuery.h"
#include "../../db/Database.h"

constexpr const char *DeleteQuery::qname;

QueryResult::Ptr DeleteQuery::execute() {
    using namespace std;
    Database &db = Database::getInstance();
    Table::SizeType counter = 0;
    try {
        auto &table = db[this->targetTable];
        auto result = initCondition(table);
        if (result.second) {
            int i =0;
            for (auto it = table.begin(); it != table.end(); ++it) {
                if (this->evalCondition(*it)) {
                    auto temp = it;
                    it--;
                    table.deleteKey(temp, i);
                    ++counter;
                } else
                    i++;
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

std::string DeleteQuery::toString() {
    return "QUERY = DELETE QUERY IN" + this->targetTable + "\"";
}

std::string DeleteQuery::cmdName() {
    return "DELETE";
}

std::string DeleteQuery::tableName() {
    return this->targetTable;
}

bool DeleteQuery::isWrite() {
    return true;
}