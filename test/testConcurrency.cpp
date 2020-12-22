//
// Created by William Leonard Sumendap on 12/21/20.
//

#include "testConcurrency.h"
#include "../source/TradeEngine.h"

const vector<pair<int, int>> EMPTY_TREE;
const vector<Trade *> EMPTY_TRADES;
const vector<Order *> EMPTY_ORDERS;

void testConcurrency::runAllTests() {
    testConcurrency::testTwoThreads();
    testConcurrency::testFourThreads();
    testConcurrency::testVariableVol();
    testConcurrency::testVariablePrice();
    testConcurrency::testDepletingOrder();
    testConcurrency::testAllOperationsConcurrently();
}

// enforce that both buy and sell trees are empty
void enforceEmptyTrees(TradeEngine *t) {
    assert(t->getPendingBuys() == EMPTY_TREE);
    assert(t->getPendingSells() == EMPTY_TREE);
}

/* In the absence of deletions, the total volume in the buy and sell trees as well as in all trades
 * has to equal the sum of volumes bought in to the system through buy/sell orders. */
void enforceVolumeInvariant(TradeEngine *t, long long expectedVol) {
    assert(t->getTotalVolume() == expectedVol);
}

void *submitBuys(void *arg) {
    TradeEngine *t = (TradeEngine *) arg;
    for (int i = 0; i < 100000; i++) {
        int buyerID = t->createUser("buyer" + to_string(i));
        t->placeBuyOrder(buyerID, 10, 1);
    }
}

void *submitSells(void *arg) {
    TradeEngine *t = (TradeEngine *) arg;
    for (int i = 0; i < 100000; i++) {
        int sellerID = t->createUser("seller" + to_string(i));
        t->placeSellOrder(sellerID, 10, 1);
    }
}

void testConcurrency::testTwoThreads() {
    TradeEngine *t = new TradeEngine();

    pthread_t buyThread;
    pthread_t sellThread;

    pthread_create(&buyThread, NULL, &submitBuys, (void *) t);
    pthread_create(&sellThread, NULL, &submitSells, (void *) t);

    pthread_join(buyThread, NULL);
    pthread_join(sellThread, NULL);

    enforceEmptyTrees(t);
    enforceVolumeInvariant(t, 200000);

    cout << "testTwoThreads passed" << endl;

    delete t;
}

void testConcurrency::testFourThreads() {
    TradeEngine *t = new TradeEngine();

    pthread_t buyThread1;
    pthread_t buyThread2;
    pthread_t sellThread1;
    pthread_t sellThread2;

    pthread_create(&buyThread1, NULL, &submitBuys, (void *) t);
    pthread_create(&sellThread1, NULL, &submitSells, (void *) t);
    pthread_create(&buyThread2, NULL, &submitBuys, (void *) t);
    pthread_create(&sellThread2, NULL, &submitSells, (void *) t);

    pthread_join(buyThread1, NULL);
    pthread_join(sellThread1, NULL);
    pthread_join(buyThread2, NULL);
    pthread_join(sellThread2, NULL);

    enforceEmptyTrees(t);
    enforceVolumeInvariant(t, 400000);

    cout << "testFourThreads passed" << endl;

    delete t;
}

void *submitBuysVariableVol(void *arg) {
    TradeEngine *t = (TradeEngine *) arg;
    for (int i = 0; i < 10000; i++) {
        int buyerID = t->createUser("buyer" + to_string(i));
        t->placeBuyOrder(buyerID, 10, i + 1);
    }
}

void *submitSellsVariableVol(void *arg) {
    TradeEngine *t = (TradeEngine *) arg;
    for (int i = 0; i < 10000; i++) {
        int sellerID = t->createUser("seller" + to_string(i));
        t->placeSellOrder(sellerID, 10, i + 1);
    }
}

void testConcurrency::testVariableVol() {
    TradeEngine *t = new TradeEngine();

    pthread_t buyThread;
    pthread_t sellThread;

    pthread_create(&buyThread, NULL, &submitBuysVariableVol, (void *) t);
    pthread_create(&sellThread, NULL, &submitSellsVariableVol, (void *) t);

    pthread_join(buyThread, NULL);
    pthread_join(sellThread, NULL);

    enforceEmptyTrees(t);
    enforceVolumeInvariant(t, 10000 * 10001);

    cout << "testVariableVol passed" << endl;

    delete t;
}

void *submitBuysVariablePrice(void *arg) {
    TradeEngine *t = (TradeEngine *) arg;
    for (int i = 0; i < 1000; i++) {
        int buyerID = t->createUser("buyer" + to_string(i));
        t->placeBuyOrder(buyerID, i + 1, 1);
    }
}

void *submitSellsVariablePrice(void *arg) {
    TradeEngine *t = (TradeEngine *) arg;
    for (int i = 0; i < 1000; i++) {
        int sellerID = t->createUser("seller" + to_string(i));
        t->placeSellOrder(sellerID, i + 1, 1);
    }
}

void testConcurrency::testVariablePrice() {
    TradeEngine *t = new TradeEngine();

    pthread_t buyThread;
    pthread_t sellThread;

    pthread_create(&buyThread, NULL, &submitBuysVariablePrice, (void *) t);
    pthread_create(&sellThread, NULL, &submitSellsVariablePrice, (void *) t);

    pthread_join(buyThread, NULL);
    pthread_join(sellThread, NULL);

    enforceVolumeInvariant(t, 2000);

    cout << "testVariablePrice passed" << endl;

    delete t;
}

void *submitDepletingOrder(void *arg) {
    TradeEngine *t = (TradeEngine *) arg;
    int buyerID = t->createUser("buyer");
    t->placeBuyOrder(buyerID, 1000, 1000);
}

void testConcurrency::testDepletingOrder() {
    TradeEngine *t = new TradeEngine();

    pthread_t buyThread;
    pthread_t sellThread;

    pthread_create(&buyThread, NULL, &submitDepletingOrder, (void *) t);
    pthread_create(&sellThread, NULL, &submitSellsVariablePrice, (void *) t);

    pthread_join(buyThread, NULL);
    pthread_join(sellThread, NULL);

    enforceEmptyTrees(t);
    enforceVolumeInvariant(t, 2000);

    cout << "testDepletingOrder passed" << endl;

    delete t;
}

void *createUsers(void *arg) {
    TradeEngine *t = (TradeEngine *) arg;
    for (int i = 0; i <= 500; i++) {
        t->createUser("user" + to_string(i));
    }
}

void *evenUsers(void *arg) {
    TradeEngine *t = (TradeEngine *) arg;
    for (int i = 0; i < 500; i += 2) {
        t->placeBuyOrder(i, i + 1, i + 1);
        if (i % 30 == 0) { // sometimes, check user trades and orders
            t->getBuyTrades(i);
            t->getSellTrades(i);
            t->getPendingOrders(i);
        }
        if (i % 20 == 0) { // delete some of the buy requests
            t->deleteOrder(i, 0);
        }
    }
}

void *oddUsers(void *arg) {
    TradeEngine *t = (TradeEngine *) arg;
    for (int i = 1; i < 500; i += 2) {
        t->placeSellOrder(i, i + 1, i + 1);
        if ((i - 1) % 30 == 0) { // sometimes, check user trades and orders
            t->getBuyTrades(i);
            t->getSellTrades(i);
            t->getPendingOrders(i);
        }
        if ((i - 1) % 20 == 0) { // delete some of the sell requests
            t->deleteOrder(i, 0);
        }
    }
}

void *observeBuyTree(void *arg) {
    TradeEngine *t = (TradeEngine *) arg;
    for (int i = 0; i < 50; i++) {
        t->getPendingBuys();
    }
}

void *observeSellTree(void *arg) {
    TradeEngine *t = (TradeEngine *) arg;
    for (int i = 0; i < 50; i++) {
        t->getPendingSells();
    }
}

void testConcurrency::testAllOperationsConcurrently() {
    TradeEngine *t = new TradeEngine();

    createUsers((void *) t);

    pthread_t evenUsersThread;
    pthread_t oddUsersThread;
    pthread_t buyTreeObserver;
    pthread_t sellTreeObserver;

    pthread_create(&evenUsersThread, NULL, &evenUsers, (void *) t);
    pthread_create(&oddUsersThread, NULL, &oddUsers, (void *) t);
    pthread_create(&buyTreeObserver, NULL, &observeBuyTree, (void *) t);
    pthread_create(&sellTreeObserver, NULL, &observeSellTree, (void *) t);

    pthread_join(evenUsersThread, NULL);
    pthread_join(oddUsersThread, NULL);
    pthread_join(buyTreeObserver, NULL);
    pthread_join(sellTreeObserver, NULL);

    cout << "testAllOperationsConcurrently passed" << endl;

    delete t;
}