//
// Created by William Leonard Sumendap on 12/21/20.
//

#ifndef TRADEMATCHER_TESTCONCURENCY_H
#define TRADEMATCHER_TESTCONCURENCY_H

using namespace std;

#include <pthread.h>

class testConcurrency {
public:
    static void runAllTests();
    static void testTwoThreads();
};

#endif //TRADEMATCHER_TESTCONCURENCY_H
