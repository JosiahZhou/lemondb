//
// Created by aiden on 18-10-31.
//

#include "DuplicateQuery.h"
#include "../../db/Database.h"

constexpr const char *DuplicateQuery::qname;

QueryResult::Ptr DuplicateQuery::execute() {
    using namespace std;
    Database &db = Database::getInstance();
    Table::SizeType counter = 0;

    std::vector<vector<Table::ValueType>> dataVector;
    std::vector<Table::KeyType> keyVector;

    try {
        auto &table = db[this->targetTable];
        auto result = initCondition(table);
        if (result.second) {
            size_t tableSize = table.size();
            auto it = table.begin();
            for (size_t i = 0; i < tableSize; i++) {
                if (this->evalCondition(*it)) {
                    Table::KeyType key = it->key();
                    key += "_copy";
                    if (table.existKey(key)) {
                        it++;
                        continue;
                    }
                    int fieldCount = table.getfieldSize();
                    vector<Table::ValueType> data;
                    data.reserve(fieldCount);
                    for (int i = 0; i < fieldCount; i++) {
                        data.emplace_back((*it)[i]);
                    }
//                    table.insertByIndex(key, move(data));
                    dataVector.emplace_back(move(data));
                    keyVector.emplace_back(key);
                    ++counter;
                }
                it++;
            }
            for (size_t j = 0; j < keyVector.size(); ++j) {
                table.insertByIndex(keyVector[j], move(dataVector[j]));
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

std::string DuplicateQuery::toString() {
    return "QUERY = DUPLICATE " + this->targetTable + "\"";
}

std::string DuplicateQuery::cmdName() {
    return "DUPLICATE";
}

std::string DuplicateQuery::tableName() {
    return this->targetTable;
}

bool DuplicateQuery::isWrite() {
    return true;
}
