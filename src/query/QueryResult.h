//
// Created by liu on 18-10-25.
//

#ifndef PROJECT_QUERYRESULT_H
#define PROJECT_QUERYRESULT_H

#include "../utils/formatter.h"

#include <memory>
#include <sstream>
#include <string>
#include <vector>

class QueryResult {
public:
    typedef std::unique_ptr<QueryResult> Ptr;

    virtual bool success() = 0;

    virtual bool display() = 0;

    virtual ~QueryResult() = default;

    friend std::ostream &operator<<(std::ostream &os, const QueryResult &table);
    
    virtual std::string getString() = 0;

protected:
    virtual std::ostream &output(std::ostream &os) const = 0;
};

class FailedQueryResult : public QueryResult {
    const std::string data;
public:
    bool success() override { return false; }

    bool display() override { return false; }
    
    std::string getString() override { return this->data; }
};

class SuceededQueryResult : public QueryResult {
public:
    bool success() override { return true; }
};

class NullQueryResult : public SuceededQueryResult {
public:
    bool display() override { return false; }
    
    std::string getString() override {
        std::string r = "\n";
        return r;
    }

protected:
    std::ostream &output(std::ostream &os) const override {
        return os << "\n";
    }
};

class ErrorMsgResult : public FailedQueryResult {
    std::string msg;
public:
    bool display() override { return false; }

    ErrorMsgResult(const char *qname,
                   const std::string &msg) {
        this->msg = R"(Query "?" failed : ?)"_f % qname % msg;
    }

    ErrorMsgResult(const char *qname,
                   const std::string &table,
                   const std::string &msg) {
        this->msg =
                R"(Query "?" failed in Table "?" : ?)"_f % qname % table % msg;
    }

    std::string getString() override {
        std::string r = this->msg;
        r += "\n";
        return r;
    }

protected:
    std::ostream &output(std::ostream &os) const override {
        return os << msg << "\n";
    }
};

class SuccessMsgResult : public SuceededQueryResult {
    std::string msg;
public:
    bool display() override { return false; }
    
    std::string getString() override {
        std::string r = this->msg;
        r += "\n";
        return r;
    }

    explicit SuccessMsgResult(const int number) {
        this->msg = R"(Answer = "?".)"_f % number;
    }

    explicit SuccessMsgResult(std::vector<int> results) {
        std::stringstream ss;
        ss << "Answer = ( ";
        for (auto result : results) {
            ss << result << " ";
        }
        ss << ")";
        this->msg = ss.str();
    }

    explicit SuccessMsgResult(const char *qname) {
        this->msg = R"(Query "?" success.)"_f % qname;
    }

    SuccessMsgResult(const char *qname, const std::string &msg) {
        this->msg = R"(Query "?" success : ?)"_f % qname % msg;
    }

    SuccessMsgResult(const char *qname,
                     const std::string &table,
                     const std::string &msg) {
        this->msg = R"(Query "?" success in Table "?" : ?)"_f
                    % qname % table % msg;
    }

protected:
    std::ostream &output(std::ostream &os) const override {
        return os << msg << "\n";
    }
};
        
class AnswerResult : public SuceededQueryResult {
    std::string msg;
public:
    bool display() override { return true; }
    
    std::string getString() override {
        std::string r = this->msg;
        r += "\n";
        return r;
    }
    
    explicit AnswerResult(int number) {
        std::stringstream ss;
        ss << "ANSWER = ";
        ss << number;
        this->msg = ss.str();
    }
    
    explicit AnswerResult(std::string filename) {
        std::stringstream ss;
        ss << "ANSWER = ( listening from ";
        ss << filename;
        ss << " ) ";
        this->msg = ss.str();
    }
    
    explicit AnswerResult(std::vector<int> results) {
        std::stringstream ss;
        ss << "ANSWER = ( ";
        for (auto result : results) {
            ss << result << " ";
        }
        ss << ") ";
        this->msg = ss.str();
    }
    
protected:
    std::ostream &output(std::ostream &os) const override {
        return os << msg << "\n";
    }
};

class RecordCountResult : public SuceededQueryResult {
    const int affectedRows;
public:
    bool display() override { return true; }
    
    std::string getString() override {
        std::string r = "Affected ? rows."_f % affectedRows + "\n";
        return r;
    }

    explicit RecordCountResult(int count) : affectedRows(count) {}

protected:
    std::ostream &output(std::ostream &os) const override {
        return os << "Affected ? rows."_f % affectedRows << "\n";
    }
};

class SelectResult : public SuceededQueryResult {
    const std::string out;
public:
    bool display() override { return true; }
    
    std::string getString() override { return this->out; }

    explicit SelectResult(std::string outr) : out(outr) {}

protected:
    std::ostream &output(std::ostream &os) const override {
        return os << out;
    }
};

#endif //PROJECT_QUERYRESULT_H
