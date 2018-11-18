//
// Created by aiden on 18-10-31.
//

#include "SwapQuery.h"
#include "../../db/Database.h"

constexpr const char *SwapQuery::qname;

QueryResult::Ptr SwapQuery::execute() {
    using namespace std;
    if (this->operands.size() != 2)
        return make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                "Invalid number of operands (? operands)."_f % operands.size()
        );
    Database &db = Database::getInstance();
    Table::SizeType counter = 0;
    try {
        auto &table = db[this->targetTable];
        if (this->operands[0] == "KEY") {
            this->keyValue = this->operands[1];
        } else {
            this->fieldId1 = table.getFieldIndex(this->operands[0]);
            this->fieldId2 = table.getFieldIndex(this->operands[1]);
        }
        auto result = initCondition(table);
        if (result.second) {
            for (auto it = table.begin(); it != table.end(); ++it) {
                if (this->evalCondition(*it)) {
                    if (this->keyValue.empty()) {
                        Table::ValueType temp = (*it)[this->fieldId1];
                        (*it)[this->fieldId1] = (*it)[this->fieldId2];
                        (*it)[this->fieldId2] = temp;
                    } else {
                        it->setKey(this->keyValue);
                    }
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

std::string SwapQuery::toString() {
    return "QUERY = SWAP " + this->targetTable + "\"";
}

std::string SwapQuery::cmdName() {
    return "SWAP";
}

std::string SwapQuery::tableName() {
    return this->targetTable;
}

bool SwapQuery::isWrite() {
    return true;
}