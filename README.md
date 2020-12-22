# OrderMatchingEngine

## About
A simple implementation of an order matching engine. Supports buy/sell orders with integer amounts (volume). The engine matches any incoming order with pre-existing ones, creating trades that follow the prices of pre-existing orders.

When a buy order is placed, the engine tries to match some (or all) of the amount with current pending sell orders. If there are matches, a trade occurs following the price of the pending sell order. Any untraded amount of this new order goes to the buy heap (pending buy orders).

Similarly, when a sell order is placed, the engine tries to match the current pending buy orders. Whenever a match occurs, a trade occurs following the price of the pending buy order. Any untraded amount of this new order goes to the sell heap (pending sell orders).

The engine also supports efficient deletion of a pending order.

Additional features include viewing the buy/sell heap and viewing pending orders and trade history from the perspective of a user.

The buy/sell heap is returned as a list of pairs containing the price point and its volume.

## Operation Runtime Complexities
Initialisation of the trade engine: O(1).

Creating a user: O(1).

Placing a buy order: 
* Worst-case complexity: O(n + log m), where n is the number of orders in the sell tree (not its size!) and m is the size (i.e., the number of distinct price points) of the buy tree.
* Amortised complexity: O(log m), where m is the size (i.e., the number of distinct price points) of the buy tree.

Placing a sell order: 
* Worst-case complexity: O(n + log m), where n is the number of orders in the buy tree (not its size!) and m is the size (i.e., the number of distinct price points) of the sell tree.
* Amortised complexity: O(log m), where m is the size (i.e., the number of distinct price points) of the sell tree.

Deleting an order: O(log m), where m is the size (number of keys) of the corresponding buy/sell tree.

Getting pending buy orders from the buy tree: O(m), where m is the size (i.e., the number of distinct price points) of the buy tree.

Getting pending sell orders from the sell tree: O(m), where m is the size (i.e., the number of distinct price points) of the buy tree.

Getting a user’s pending orders (buy/sell): O(n), where n is the number of such orders.

Getting a user’s buy history: O(1). It returns a pointer to the vector of such trades.

Getting a user’s sell history: O(1). It returns a pointer to the vector of such trades.

## Concurrency Support:
This project supports concurrency using locks, with the assumption that no user appears in more than 1 thread. However, threads are allowed to create multiple users.

## Compiling and Executing
Run CMake build:
```bash
cmake build .
```
Run Makefile:
```bash
make
```
Run the executable:
```bash
./OrderMatchingEngine
```
All testcases will be run, producing the following output:
```bash
testInit passed
testInitUser passed
testSingleUser passed
testSingleDelete passed
testDeleteAfterTrade passed
testBuyOut passed
testSellOut passed
testAll passed
stressTest passed
testTwoThreads passed
testFourThreads passed
testVariableVol passed
testVariablePrice passed
testDepletingOrder passed
testAllOperationsConcurrently passed
```
