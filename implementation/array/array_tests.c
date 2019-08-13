#include "array.h"


#ifndef test
    void exit(int);
    static inline
    void test_failed(const char* file, const int line, const char* msg) {
        printf("%s:%i: %s\n", file, line, msg);
        exit(1);
    }
    #define test(expr) \
        (((expr) ? 0 : \
        (test_failed(__FILE__, __LINE__, "test("#expr") failed"), 1)))
#endif


static size_t destructed_element_count = 0;


void destructed_element_count_destructor(int* begin, int* end) {
    for(; begin < end; ++begin) {
        destructed_element_count += 1;
    }
}


void array_tests(void) {
    array_t(int) a = NULL;
    test(array_size(a) == 0);
    test(array_capacity(a) == 0);

    array_alloc(a, 0, destructed_element_count_destructor);
    test(array_size(a) == 0);
    test(array_capacity(a) == 0);

    array_append(a, 1);
    test(array_size(a) == 1);
    test(array_capacity(a) >= 1);
    test(a[0] == 1);


    array_append(a, 2);
    test(array_size(a) == 2);
    test(array_capacity(a) >= 2);
    test(a[0] == 1);
    test(a[1] == 2);


    array_append(a, 3);
    test(array_size(a) == 3);
    test(array_capacity(a) >= 3);
    test(a[0] == 1);
    test(a[1] == 2);
    test(a[2] == 3);


    array_insert(a, 0, 0);
    test(array_size(a) == 4);
    test(array_capacity(a) >= 4);
    test(a[0] == 0);
    test(a[1] == 1);
    test(a[2] == 2);
    test(a[3] == 3);


    array_reserve(a, 16);
    test(array_size(a) == 4);
    test(array_capacity(a) == 16);
    test(a[0] == 0);
    test(a[1] == 1);
    test(a[2] == 2);
    test(a[3] == 3);


    array_shrink(a);
    test(array_size(a) == 4);
    test(array_capacity(a) == 4);
    test(a[0] == 0);
    test(a[1] == 1);
    test(a[2] == 2);
    test(a[3] == 3);


    array_remove(a, 0);
    test(array_size(a) == 3);
    test(array_capacity(a) == 4);
    test(a[0] == 1);
    test(a[1] == 2);
    test(a[2] == 3);
    test(destructed_element_count == 1);
    destructed_element_count = 0;


    array_remove_unordered(a,0);
    test(array_size(a) == 2);
    test(array_capacity(a) == 4);
    test(a[0] == 3);
    test(a[1] == 2);
    test(destructed_element_count == 1);
    destructed_element_count = 0;


    array_clear(a);
    test(array_size(a) == 0);
    test(array_capacity(a) >= 0);
    test(destructed_element_count == 2);
    destructed_element_count = 0;


    array_append(a, 0);
    array_append(a, 1);
    array_append(a, 2);
    test(array_size(a) == 3);
    test(array_capacity(a) >= 3);
    test(destructed_element_count == 0);


    array_free(a);
    test(a == NULL);
    test(array_size(a) == 0);
    test(array_capacity(a) == 0);
    test(destructed_element_count == 3);
    destructed_element_count = 0;


    enum { TEST_LENGTH = 1024 };


    array_alloc(a, 0, destructed_element_count_destructor);
    for (int i = 0; i < TEST_LENGTH; ++i) {
        array_append(a, i);
        test(a[i] == i);
    }
    test(array_size(a) == TEST_LENGTH);
    test(array_capacity(a) >= TEST_LENGTH);
    for (int i = 0; i < TEST_LENGTH; ++i) {
        test(a[i] == i);
    }
    {
        int i = 0;
        const int* const end = array_end(a);
        for (int* itr = array_begin(a); itr < end; ++itr) {
            test(*itr == i++);
        }
    }
    {
        int i = 0;
        while (array_size(a)) {
            test(a[0] == i++);
            array_remove(a,0);
        }
        test(array_size(a) == 0);
        test(array_capacity(a) >= TEST_LENGTH);
        test(destructed_element_count == TEST_LENGTH);
        destructed_element_count = 0;
    }
    array_free(a);
    test(a == NULL);
    test(array_size(a) == 0);
    test(array_capacity(a) == 0);


    array_alloc(a, 0, destructed_element_count_destructor);
    for (int i = 0; i < TEST_LENGTH; ++i) {
        array_insert(a, 0, i);
    }
    test(array_size(a) == TEST_LENGTH);
    test(array_capacity(a) >= TEST_LENGTH);
    for (int i = 0; i < TEST_LENGTH; ++i) {
        test(a[i] == (TEST_LENGTH - 1) - i);
    }
    array_free(a);
    test(a == NULL);
    test(array_size(a) == 0);
    test(array_capacity(a) == 0);


    puts("array tests passed");
}
