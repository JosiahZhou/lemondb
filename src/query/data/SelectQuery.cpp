//
// Created by aiden on 18-10-31.
//

#include <queue>
#include "SelectQuery.h"
#include "../../db/Database.h"
#define int_max 2147483647
#define int_min -2147483648
constexpr const char *SelectQuery::qname;

QueryResult::Ptr SelectQuery::execute() {
    using namespace std;
    Database &db = Database::getInstance();
    try {
        auto &table = db[this->targetTable];
        auto result = initCondition(table);
        vector<Table::FieldIndex> SelectId;
        for (auto it = ++this->operands.begin(); it != this->operands.end(); ++it) {
            SelectId.push_back(table.getFieldIndex(*it));
        }
        priority_queue<string, vector<string>, greater<string>>selected;
        if (result.second) {
            int size = this->operands.size();
            for (auto it = table.begin(); it != table.end(); ++it) {
                if (this->evalCondition(*it)) {
                    string out = "";
                    out +=  "( " + it->key();
                    for (int i = 0; i < size-1; i++) {
                        out += " " + to_string((*it)[SelectId[i]]);
                    }
                    out += " )\n";
                    selected.push(out);
                }
            }
        }
        string out = "";
        int queue_size = selected.size();
        for (int i = 0; i < queue_size; i++) {
            out = out + selected.top();
            selected.pop();
        }
        return std::make_unique<SelectResult>(out);
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

std::string SelectQuery::toString() {
    return "QUERY = SELECT IN" + this->targetTable + "\"";
}

std::string SelectQuery::cmdName() {
    return "SELECT";
}

std::string SelectQuery::tableName() {
    return this->targetTable;
}

bool SelectQuery::isWrite() {
    return false;
}