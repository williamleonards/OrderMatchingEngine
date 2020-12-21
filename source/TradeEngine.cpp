//
// Created by William Leonard Sumendap on 7/10/20.
//

#include "TradeEngine.h"
#include <pthread.h>

TradeEngine::TradeEngine() {
    nextUserID = 0;
}
TradeEngine::~TradeEngine() {
    for (auto p : users) {
        delete p.second;
    }
    for (auto p : sellTree) {
        list<Order*> *lst = p.second->second;
        for (Order *ord : *lst) {
            delete ord;
        }
        delete p.second;
    }
}
int TradeEngine::createUser(string name) {
    User *user = new User(nextUserID, name);
    users[nextUserID] = user;
    if (nextUserID == (1 << 31) - 1) {
        cout << "User limit reached" << endl;
        return -1;
    }
    nextUserID++;
    return nextUserID - 1;
}
vector<Trade*> TradeEngine::placeBuyOrder(int issuerID, int price, int amt) {
    User *user = users[issuerID];
    vector<Trade*> ans;
    if (user == NULL) {
        cout << "User ID not known" << endl;
        return ans;
    }
    int remaining = amt;
    ans = generateTrades(true, price, issuerID, remaining);
    if (remaining > 0) { // put surplus amount on buy tree
        putRemainingOrderOnTree(true, user, price, remaining);
    }
    return ans;
}
vector<Trade*> TradeEngine::placeSellOrder(int issuerID, int price, int amt) {
    User *user = users[issuerID];
    vector<Trade*> ans;
    if (user == NULL) {
        cout << "User ID not known" << endl;
        return ans;
    }
    int remaining = amt;
    ans = generateTrades(false, price, issuerID, remaining);
    if (remaining > 0) { // put surplus amount on sell tree
        putRemainingOrderOnTree(false, user, price, remaining);
    }
    return ans;
}
void TradeEngine::deleteOrder(int issuerID, int orderID) { //lazy deletion
    User *user = users[issuerID];
    if (user == NULL) {
        cout << "User ID not known" << endl;
        return;
    }
    unordered_map<int, Order*> *userOrders = (user->getOrders());
    Order *order = userOrders->at(orderID);
    if (order == NULL) {
        cout << "Order ID not known" << endl;
        return;
    }
    int price = order->getPrice();
    pair<int, list<Order*>*> *p = order->getType() ? buyTree[price] : sellTree[price];
    if (p == NULL) {
        cout << "Heap incorrectly configured!, type = " << order->getType() << endl;
        return;
    }
    p->first -= order->getAmt();
    order->setInvalid();
    userOrders->erase(orderID);
}
vector<pair<int, int>> TradeEngine::getPendingBuys() {
    vector<pair<int, int>> v;
    for (auto itr = buyTree.rbegin(); itr != buyTree.rend(); itr++) {
        int price = itr->first;
        int vol = itr->second->first;
        if (vol != 0) {
            v.push_back(pair<int, int>(price, vol));
        }
    }
    return v;
}
vector<pair<int, int>> TradeEngine::getPendingSells() {
    vector<pair<int, int>> v;
    for (auto itr = sellTree.begin(); itr != sellTree.end(); itr++) {
        int price = itr->first;
        int vol = itr->second->first;
        if (vol != 0) {
            v.push_back(pair<int, int>(price, vol));
        }
    }
    return v;
}
vector<Order*> TradeEngine::getPendingOrders(int userID) {
    User *user = users[userID];
    vector<Order*> ans;
    if (user == NULL) {
        cout << "User ID not known" << endl;
        return ans;
    }
    auto orders = user->getOrders();
    for (auto itr = orders->begin(); itr != orders->end(); itr++) {
        ans.push_back(itr->second);
    }
    return ans;
}
vector<Trade*>* TradeEngine::getBuyTrades(int userID) {
    User *user = users[userID];
    vector<Trade*> ans;
    if (user == NULL) {
        cout << "User ID not known" << endl;
        return nullptr;
    }
    return user->getBought();
}
vector<Trade*>* TradeEngine::getSellTrades(int userID) {
    User *user = users[userID];
    vector<Trade*> ans;
    if (user == NULL) {
        cout << "User ID not known" << endl;
        return nullptr;
    }
    return user->getSold();
}

// helper methods defined below
bool TradeEngine::firstOrderIsStale(list<Order*> *lst) {
    Order *first = lst->front();
    if (!first->checkValid()) {
        lst->pop_front();
        delete first;
        return true;
    }
    return false;
}
void TradeEngine::putRemainingOrderOnTree(bool buyOrSell, User *user, int price, int remaining) {
    map<int, pair<int, list<Order*>*>*> &tree = buyOrSell ? buyTree : sellTree;
    pair<int, list<Order*>*> *p = tree[price];
    Order *leftover = user->issueOrder(buyOrSell, price, remaining);
    if (p != NULL) {
        p->first += remaining;
        p->second->push_back(leftover);
    } else {
        list<Order*> *lst = new list<Order*>();
        lst->push_back(leftover);
        tree[price] = new pair<int, list<Order*>*>(remaining, lst);
    }
}
vector<Trade*> TradeEngine::generateTrades(bool buyOrSell, int &price, int &issuerID, int &remaining) {
    User *user = users[issuerID];
    vector<Trade*> ans;
    // generate trades when appropriate
    if (buyOrSell) {
        for (auto itr = sellTree.begin(); itr != sellTree.end(); itr++) {
            int currPrice = itr->first;
            if (currPrice > price || remaining <= 0) break;
            int amtLeft = itr->second->first;
            list<Order*> *orders = itr->second->second;
            consumePendingOrders(true, issuerID, remaining, amtLeft, currPrice, orders, ans);
            itr->second->first = amtLeft;
        }
    } else {
        for (auto itr = buyTree.rbegin(); itr != buyTree.rend(); itr++) {
            int currPrice = itr->first;
            if (currPrice < price || remaining <= 0) break;
            int amtLeft = itr->second->first;
            list<Order*> *orders = itr->second->second;
            consumePendingOrders(false, issuerID, remaining, amtLeft, currPrice, orders, ans);
            itr->second->first = amtLeft;
        }
    }
    return ans;
}
void TradeEngine::consumePendingOrders(bool buyOrSell, int &issuerID, int &remaining, int &amtLeft,
        int &currPrice, list<Order*> *orders, vector<Trade*> &ans) {
    while (remaining > 0 && !orders->empty()) {
        if (firstOrderIsStale(orders)) continue; // remove if current order is stale and continue
        Order *first = orders->front();
        int currAmt = first->getAmt();
        int counterpartyID = first->getIssuerID();

        // figure out who's the buyer and the seller
        int buyerID = buyOrSell ? issuerID : counterpartyID;
        int sellerID = buyOrSell ? counterpartyID : issuerID;
        User *buyer = users[buyerID];
        User *seller = users[sellerID];

        if (remaining < currAmt) { // current order not finished
            Trade *trade = new Trade(remaining, currPrice, buyerID, sellerID);
            ans.push_back(trade);
            // update buyer's and seller's finished orders
            buyer->getBought()->push_back(trade);
            seller->getSold()->push_back(trade);
            // update first order amount
            first->setAmt(currAmt - remaining);
            amtLeft -= remaining;
            remaining = 0;
        } else { // current order finished
            Trade *trade = new Trade(currAmt, currPrice, buyerID, sellerID);
            ans.push_back(trade);
            // update buyer's and seller's finished orders
            buyer->getBought()->push_back(trade);
            seller->getSold()->push_back(trade);
            orders->pop_front();
            // update seller's orders
            seller->getOrders()->erase(first->getID());
            delete first;
            remaining -= currAmt;
            amtLeft -= currAmt;
        }
    }
}
