//
// Created by William Leonard Sumendap on 7/10/20.
//

#ifndef TRADEMATCHER_TRADE_H
#define TRADEMATCHER_TRADE_H

using namespace std;

#include<vector>
#include<string>
#include<list>
#include<unordered_map>
#include<map>

#include "User.h"

class Trade {
public:
    Trade(int amt_, int price_, int buyerID_, int sellerID_);
    int getAmt();
    int getPrice();
    int getBuyerID();
    int getSellerID();

    // Logical equals: return true if other != NULL and all the corresponding fields are equal
    bool equals(Trade *other);
private:
    int amt;
    int price;
    int buyerID;
    int sellerID;
};


#endif //TRADEMATCHER_TRADE_H
