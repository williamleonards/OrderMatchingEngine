#include <iostream>
#include <unordered_map>

#include "../test/test.h"
#include "../test/testConcurrency.h"

using namespace std;

int main() {

    test::runAllTests();
    testConcurrency::runAllTests();

    return 0;
}