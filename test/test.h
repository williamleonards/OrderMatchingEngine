//
// Created by William Leonard Sumendap on 7/10/20.
//

#ifndef TRADEMATCHER_TEST_H
#define TRADEMATCHER_TEST_H

using namespace std;

#include <pthread.h>

class test {
public:
    static void runAllTests();
    static void testInit();
    static void testInitUser();
    static void testSingleUser();
    static void testSingleDelete();
    static void testDeleteAfterTrade();
    static void testBuyOut();
    static void testSellOut();
    static void testAll();
    static void stressTest();
};


#endif //TRADEMATCHER_TEST_H
