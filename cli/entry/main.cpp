#include <iostream>

#ifndef RSI_GIT_TAG
#define RSI_GIT_TAG "unknown"
#endif

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;
    std::cout << "rsi " << RSI_GIT_TAG << '\n';
    return 0;
}
