//
// Created by liu on 18-10-21.
//

#include "query/QueryParser.h"
#include "query/QueryBuilders.h"

#include <getopt.h>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <list>
#include <queue>
#include <mutex>
#include <unordered_map>

std::unordered_map<std::string, std::list<Query::Ptr> > queryMap;
std::unordered_map<std::string, std::mutex> tableMutex;
std::unordered_map<std::string, std::mutex> queryListMutex;
std::vector<std::thread *> threadList;
std::list<std::string> resultList;
std::mutex threadCountLock;
std::mutex threadLock;
int threadCounter;

bool queryListEmpty = false;

struct {
    std::string listen;
    long threads = 0;
} parsedArgs;

void parseArgs(int argc, char *argv[]) {
    const option longOpts[] = {
        {"listen",  required_argument, nullptr, 'l'},
        {"threads", required_argument, nullptr, 't'},
        {nullptr,   no_argument,       nullptr, 0}
    };
    const char *shortOpts = "l:t:";
    int opt, longIndex;
    while ((opt = getopt_long(argc, argv, shortOpts, longOpts, &longIndex)) != -1) {
        if (opt == 'l') {
            parsedArgs.listen = optarg;
        } else if (opt == 't') {
            parsedArgs.threads = std::strtol(optarg, nullptr, 10);
        } else {
            std::cerr << "lemondb: warning: unknown argument " << longOpts[longIndex].name << std::endl;
        }
    }
    
}

std::string extractQueryString(std::istream &is) {
    std::string buf;
    do {
        int ch = is.get();
        if (ch == ';') return buf;
        if (ch == EOF) throw std::ios_base::failure("End of input");
        buf.push_back((char) ch);
    } while (true);
}


void tableThread(std::string tableName) {
    tableMutex[tableName].lock();
    
    threadCountLock.lock();
    if(threadCounter == 0){
        threadCountLock.unlock();
        threadLock.lock();
        threadLock.unlock();
        threadCountLock.lock();
    }
    if(--threadCounter == 0){
        threadLock.lock();
    }
    threadCountLock.unlock();
    
    while (1) {
        if (queryMap[tableName].empty() && queryListEmpty) {
            break;
        }
        else if (queryMap[tableName].empty()){
            std::this_thread::yield();
        }
        else {
            Query::Ptr *query = &queryMap[tableName].front();
            QueryResult::Ptr result = (*query)->execute();
            std::string resultString = result->getString();
            if (result->success()) {
                if (result->display()) {
                    *((*query)->resultString) = resultString;
//                    std::cout << tableName << " : " << *result;
                }
            }
//                } else {
//#ifndef NDEBUG
//                    std::cout.flush();
//                    std::cerr << *result;
//#endif
//                }
//            } else {
//                std::cout.flush();
//                std::cerr << "QUERY FAILED:\n\t" << *result;
//            }
            if ((*query)->cmdName() == "DROP") break;
            if ((*query)->cmdName() == "COPYTABLE") tableMutex[(*query)->getNewTableName()].unlock();
            
            queryListMutex[tableName].lock();
            queryMap[tableName].pop_front();
            queryListMutex[tableName].unlock();
        }
    }
    threadCountLock.lock();
    threadCounter++;
    threadLock.unlock();
    threadCountLock.unlock();
    tableMutex[tableName].unlock();
}

void pushQuery(std::string fileaddress, QueryParser &p){
    
    std::fstream fin;
    fin.open(fileaddress);
    if (!fin.is_open()) {
        std::cerr << "lemondb: error: " << parsedArgs.listen << ": no such file or directory" << std::endl;
        exit(-1);
    }
    std::istream is(fin.rdbuf());
    while (is) {
        try {
            // A very standard REPL
            // REPL: Read-Evaluate-Print-Loop
            std::string queryStr = extractQueryString(is);
            Query::Ptr query = p.parseQuery(queryStr);
            QueryResult::Ptr result;
            std::string tname = query->tableName();
            
            if (query->cmdName() != "QUIT") {
                resultList.emplace_back(std::string());
                (*query).resultString = &(*(--resultList.end()));
            }
            else {
                break;
            }
            if (query->cmdName() == "LISTEN") {
                result = query->execute();
                std::string resultString = result->getString();
                
                pushQuery(tname, p);
                
                if (result->success()) {
                    if (result->display()) {
                        *((*query).resultString) = resultString;
                    }
                }
            }
            if (query->cmdName() == "LOAD") {
                std::thread *currentThread = new std::thread(&tableThread, tname);
                threadList.push_back(currentThread);
            }
            else if (query->cmdName() == "COPYTABLE") {
                tableMutex[query->getNewTableName()].lock();
                std::thread *currentThread = new std::thread(&tableThread, query->getNewTableName());
                threadList.push_back(currentThread);
            }
            queryListMutex[tname].lock();
            queryMap[tname].emplace_back(move(query));
            queryListMutex[tname].unlock();
            }
            catch (const std::ios_base::failure& e) {
            break;
        } catch (const std::exception& e) {
            std::cout.flush();
            std::cerr << e.what() << std::endl;
        }
    }
}

void threadsCase (std::istream &is, QueryParser &p) {
    size_t counter = 0;
    
    while (is) {
        try {
            // A very standard REPL
            // REPL: Read-Evaluate-Print-Loop
            std::string queryStr = extractQueryString(is);
            Query::Ptr query = p.parseQuery(queryStr);
            QueryResult::Ptr result = query->execute();
            std::cout << ++counter << "\n";
            if (result->success()) {
                if (result->display()) {
                    std::cout << *result;
                } else {
#ifndef NDEBUG
                    std::cout.flush();
                    std::cerr << *result;
#endif
                }
            } else {
                std::cout.flush();
                std::cerr << "QUERY FAILED:\n\t" << *result;
            }
        }  catch (const std::ios_base::failure& e) {
            // End of input
            break;
        } catch (const std::exception& e) {
            std::cout.flush();
            std::cerr << e.what() << std::endl;
        }
    }
}

int main(int argc, char *argv[]) {
    // Assume only C++ style I/O is used in lemondb
    // Do not use printf/fprintf in <cstdio> with this line
    std::ios_base::sync_with_stdio(false);
    
    parseArgs(argc, argv);
    int threadNum=0;
    
    std::fstream fin;
    if (!parsedArgs.listen.empty()) {
        fin.open(parsedArgs.listen);
        if (!fin.is_open()) {
            std::cerr << "lemondb: error: " << parsedArgs.listen << ": no such file or directory" << std::endl;
            exit(-1);
        }
    }
    std::istream is(fin.rdbuf());
    
#ifdef NDEBUG
    // In production mode, listen argument must be defined
    if (parsedArgs.listen.empty()) {
        std::cerr << "lemondb: error: --listen argument not found, not allowed in production mode" << std::endl;
        exit(-1);
    }
#else
    // In debug mode, use stdin as input if no listen file is found
    if (parsedArgs.listen.empty()) {
        std::cerr << "lemondb: warning: --listen argument not found, use stdin instead in debug mode" << std::endl;
        is.rdbuf(std::cin.rdbuf());
    }
#endif
    
    QueryParser p;
    
    p.registerQueryBuilder(std::make_unique<QueryBuilder(Debug)>());
    p.registerQueryBuilder(std::make_unique<QueryBuilder(Listen)>());
    p.registerQueryBuilder(std::make_unique<QueryBuilder(ManageTable)>());
    p.registerQueryBuilder(std::make_unique<QueryBuilder(Complex)>());
    
    if (parsedArgs.threads < 0) {
        std::cerr << "lemondb: error: threads num can not be negative value " << parsedArgs.threads << std::endl;
        exit(-1);
    } else if (parsedArgs.threads == 0) {
        threadNum = std::thread::hardware_concurrency();
        // @TODO Auto detect the thread num
        std::cerr << "lemondb: info: auto detect thread num\n"<<"Running in: "<<threadNum<<" threads" << std::endl;
    } else if (parsedArgs.threads == 1) {
        std::cerr << "lemondb: info: running in " << parsedArgs.threads << " threads" << std::endl;
        threadsCase(is, p);
    } else {
        threadNum = parsedArgs.threads;
        std::cerr << "lemondb: info: running in " << parsedArgs.threads << " threads" << std::endl;
    }
    threadCounter=threadNum;
//    std::cout << "Thread number is : " << threadNum << std::endl;
    
    //    size_t counter = 0;
    while (is) {
        try {
            // A very standard REPL
            // REPL: Read-Evaluate-Print-Loop
            std::string queryStr = extractQueryString(is);
            Query::Ptr query = p.parseQuery(queryStr);
            QueryResult::Ptr result;
            std::string tname = query->tableName();
            
            if (query->cmdName() != "QUIT") {
                resultList.emplace_back(std::string());
                (*query).resultString = &(*(--resultList.end()));
            }
            else {
                break;
            }
            
            if (query->cmdName() == "LISTEN") {
                result = query->execute();
                std::string resultString = result->getString();
                
                pushQuery(tname, p);
                
                if (result->success()) {
                    if (result->display()) {
                        *((*query).resultString) = resultString;
                    }
                }
            }
            
            if (query->cmdName() == "LOAD") {
                std::thread *currentThread = new std::thread(&tableThread, tname);
                threadList.push_back(currentThread);
            }
            else if (query->cmdName() == "COPYTABLE") {
                tableMutex[query->getNewTableName()].lock();
                std::thread *currentThread = new std::thread(&tableThread, query->getNewTableName());
                /* then lock the thread */
                threadList.push_back(currentThread);
            }
            queryListMutex[tname].lock();
            queryMap[tname].emplace_back(move(query));
            queryListMutex[tname].unlock();
            //            std::cout << ++counter << "\n";
            //            if (result->success()) {
            //                if (result->display()) {
            //                    std::cout << *result;
            //                } else {
            //#ifndef NDEBUG
            //                    std::cout.flush();
            //                    std::cerr << *result;
            //#endif
            //                }
            //            } else {
            //                std::cout.flush();
            //                std::cerr << "QUERY FAILED:\n\t" << *result;
            //            }
        }  catch (const std::ios_base::failure& e) {
            // End of input
            break;
        } catch (const std::exception& e) {
            std::cout.flush();
            std::cerr << e.what() << std::endl;
        }
    }
    queryListEmpty = true;
    for(auto it = threadList.begin(); it != threadList.end(); ++it)
    {
        (*it)->join();
    }
    int count = 0;
    for(auto it = resultList.begin(); it != resultList.end(); ++it){
        std::cout << ++count << std::endl;
        std::cout << *(it);
    }
    
    return 0;
}
