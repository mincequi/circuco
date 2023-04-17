/*
This was something that was bugging me for a long time too. Today I think I found a solution.

Create one main cpp file for all your tests you want to run with one build/compilation in a test folder.
Put all your tests in separate files
a) in header files somewhere, preferably under the same test folder
b) in cpp files in another test folder, but make sure that in your platformio.ini that specific test folder is added to the test_ignore options.
In your test files, create all your test functions as usual
Add one extra function - e.g. runAllMyTests() - in your test file which should call all other test functions. At the beginning of this function add a call to UnitySetTestFile(__FILE__);
Include all your separate test files in your main test file.
Implement setup() and loop() only in your main test file and call all your runAllMyTests() (you should name them differently or use different namespaces) functions between UNITY_BEGIN() and UNITY_END().
Depending on the granularity of the messages you want, you can choose to use the RUN_TEST() macro in your separate test files to encapsulate your individual test functions within your runAllMyTests() functions or use the macro only in your main cpp file to encapculate only the runAllMyTests() functions themselves (in this case you will get one FAIL/PASS per test file, but the line numbers in an error message will still be correct). In this latter case you have to use unique names though. (Using namespace names within the test macros does not seem to work well, but I did only superficial tests for that.)

I have tested with the following file structure:

/test
   /test_conversion
      test_ConvCalcIfRanges.h
      test_ConvCalcSubranges.h
      test_conversion.cpp

In the headers I put all the individual test functions and all global variables that they depended on in their own unique namespaces, only the runAllMyTests() function remained in global namespace.

test\test_conversion\test_ConvCalcSubranges.h:548: test_find_subranges_and_set_min_max_ok       [PASSED]
test\test_conversion\test_ConvCalcSubranges.h:549: test_find_subranges_and_set_min_max_error    [PASSED]
test\test_conversion\test_ConvCalcSubranges.h:369: test_pairing_primary_and_secondary_subranges_ok: something went wrong        [FAILED]
test\test_conversion\test_ConvCalcSubranges.h:551: test_pairing_primary_and_secondary_subranges_error   [PASSED]
test\test_conversion\test_ConvCalcIfRanges.h:68: test_ifranges  [PASSED]
If you need, you can also set up tearDown_TestFile() functions in all separate test files and then call them from the tearDown() function defined in your main test cpp. One approach for using such a structure could be the following:

using TearDownFuncT = void (*)();
TearDownFuncT _tearDownFunc = nullptr;

void tearDown(void)
{
    // clean stuff up here after tests cases fail
    if (_tearDownFunc)
    {
        _tearDownFunc();
    }
    _tearDownFunc = nullptr;
}

#define SET_TEARDOWN(f) _tearDownFunc = &f
#define NO_TEARDOWN _tearDownFunc = nullptr
Just use SET_TEARDOWN before calling your runAllMyTests() function from a test file where tearDown()is needed.
*/