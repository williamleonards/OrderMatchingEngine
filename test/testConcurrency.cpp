//
// Created by William Leonard Sumendap on 12/21/20.
//

#include "testConcurrency.h"
#include "../source/TradeEngine.h"

const vector<pair<int, int>> EMPTY_TREE;
const vector<Trade*> EMPTY_TRADES;
const vector<Order*> EMPTY_ORDERS;

void testConcurrency::runAllTests() {
    testConcurrency::testTwoThreads();
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
void* submitBuys(void* arg) {
    TradeEngine *t = (TradeEngine *) arg;
    for (int i = 0; i < 100000; i++) {
        int buyerID = t->createUser("buyer" + to_string(i));
        t->placeBuyOrder(buyerID, 10, 1);
    }
}
void* submitSells(void* arg) {
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