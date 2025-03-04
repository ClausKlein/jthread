#include "condition_variable_any2.hpp"
#include "jthread.hpp"

#include <atomic>
#include <cassert>
#include <chrono>
#include <cstring>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
// using namespace ::std::literals;
using namespace std::chrono_literals;

//------------------------------------------------------

void testCVCallback()
{
    std::cout << "*** start testCVCallback()" << std::endl;

    bool ready{false};
    std::mutex readyMutex;
    std::condition_variable_any2 readyCV;

    bool cbCalled{false};
    {
        std::jthread t1{[&](std::stop_token stoken) {
            std::cout << "\nt1 started" << std::endl;
            std::stop_callback cb(stoken, [&] {
                std::cout << "\nt1 cb called (1sec)" << std::endl;
                std::this_thread::sleep_for(1s);
                cbCalled = true;
                std::cout << "\nend t1 cb" << std::endl;
            });
            std::cout << "\nt1 cb registered" << std::endl;
            std::unique_lock<std::mutex> lg{readyMutex};
            readyCV.wait_until(
                lg, [&ready] { return ready; }, stoken);
            std::cout << "\nend t1" << std::endl;
        }};

        std::this_thread::sleep_for(1s);
        std::cout << "\ndestruct t1" << std::endl;
    } // leave scope of t1 without join() or detach() (signals cancellation)
    std::cout << "\nt1 destructed" << std::endl;
    assert(cbCalled);
    std::cout << "\n*** OK" << std::endl;
}

//------------------------------------------------------

int main()
{
    try {
        std::set_terminate([]() {
            std::cout << "ERROR: terminate() called" << std::endl;
            assert(false);
        });

        std::cout << std::boolalpha;

        std::cout << "\n\n**************************\n";
        testCVCallback();
        std::cout << "\n\n**************************\n";
    } catch (const std::exception &e) {
        std::cerr << "EXCEPTION: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "EXCEPTION" << std::endl;
    }
}
