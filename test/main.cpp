/*
*
*/

#include <iostream>
#include "slib/delegate.hpp"
#include "slib/args_list.hpp"
#include "slib/signals.hpp"
#include <chrono>
#include <functional>

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

int STATIC_INT = 0;

int static_function(int a)
{
    return a * 2;
}

int static_function2(int a)
{
    return STATIC_INT = a * 2;
}

int static_function3()
{
    return STATIC_INT;
}

//////////////////////////////////////////////////////////////////////////

bool test1()
{
    // Testing delegate and args_list

    std::cout << std::endl;

    // Declare delegate
    slib::delegate<int(int)> d;

    // Bind delegate to static function
    d.bind<static_function>();

    // Declare arguments list and initialize it with value
    slib::args_list<int(int)> a(10);

    // Call delegate binded function and check result
    if (d(3) != 6)
    {
        std::cout << "delegate test failed. // LINE = " << __LINE__ << std::endl;
        return false;
    }

    // Change argument value in argument list
    a.arg<0>() = 5;

    // Check if this works properly
    if (a.arg<0>() != 5)
    {
        std::cout << "args_list argument change test failed. // LINE = " << __LINE__ << std::endl;
        return false;
    }

    // Change argument value back to initial value
    a.arg<0>() = 10;

    // Check result
    if (a.arg<0>() != 10)
    {
        std::cout << "args_list argument change test failed. // LINE = " << __LINE__ << std::endl;
        return false;
    }

    // Call delegate binded function with predefined arguments list and check result
    if (a(d) != 20)
    {
        std::cout << "args_list delegate call test failed. // LINE = " << __LINE__ << std::endl;
        return false;
    }

    // Change argument value in argument list
    a.arg<0>() = 100;

    // Call static function with predefined arguments list and check result
    if (a(static_function) != 200)
    {
        std::cout << "args_list static-function call test failed. // LINE = " << __LINE__ << std::endl;
        return false;
    }

    // Change argument value in argument list
    a.arg<0>() = 300;

    // Call static function with predefined arguments list and check result
    if (slib::invoke<int>(static_function, a.args()) != 600)
    {
        std::cout << "slib::util::invoke static-function call test failed. // LINE = " << __LINE__ << std::endl;
        return false;
    }

    // Check that empty arguments list can be also compiled and used properly
    slib::args_list<int()> a2;
    slib::delegate<int()> d2;
    d2.bind<static_function3>();
    if (a2(d2) != 0)
    {
        return false;
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////

bool test2()
{
    // Testing signal and slot

    std::cout << std::endl;

    // Create slot
    slib::slot<int(int)> slt;

    // Bind slot to static function
    slt.bind<static_function2>();

    // Create signal
    slib::signal<int(int)> sgnl;

    // Check that signal and slot are not connected to somewhat
    if (sgnl.connected() || slt.connected())
    {
        std::cout << "sgnl.connected() || slt.connected() // LINE = " << __LINE__ << std::endl;
        return false;
    }

    // Connect signal and slot
    slib::connect(sgnl, slt);

    // Check that now signal and slot are connected
    if (!sgnl.connected() || !slt.connected())
    {
        std::cout << "!sgnl.connected() || !slt.connected() // LINE = " << __LINE__ << std::endl;
        return false;
    }

    // Check initial value of global variable
    if (STATIC_INT != 0)
    {
        std::cout << "STATIC_INT != 0 // LINE = " << __LINE__ << std::endl;
        return false;
    }

    // Emit signal with argument equal to 10
    // slot will be called and binded static function will be invoked
    sgnl(10);

    // Check result of static function call
    if (STATIC_INT != 20)
    {
        std::cout << "STATIC_INT != 20 // LINE = " << __LINE__ << std::endl;
        return false;
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

typedef bool(*pTest)();

const pTest tests[] = {
    test1,
    test2
};

//////////////////////////////////////////////////////////////////////////

int main()
{
    std::cout << "Begin testing...\n";

    int result = 0;

    int i = 0;

    // Run all tests
    for (auto test : tests)
    {
        ++i;

        std::cout << "--- Test" << i << ": ";
        if (!test())
        {
            result = -1;
            std::cout << "FAILED!";
        }
        else
        {
            std::cout << "OK!";
        }

        std::cout << std::endl;
    }

    std::cout << "Testing complete.\nInput something to exit: ";

    char c = 0;
    std::cin >> c;

    return 0;
}
