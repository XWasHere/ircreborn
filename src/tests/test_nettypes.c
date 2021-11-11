#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <common/util.h>
#include <networking/types.h>

#include <tests/util.h>

int run_test_nettypes() {
    TEST(nettype_int, {
        int in    = 42069;
        char* out = malloc(255);

        write_int(out, in);
        int res = read_int(out);

        INT_ASSERT_EQUALS(res, in);

        free(out);
    });

    TEST(nettype_string, {
        char* in  = "if youre seeing this, the thing worked";
        char* out = malloc(255);

        write_string(out, in, strlen(in));
        nstring_t* res = read_string(out);

        INT_ASSERT_EQUALS(res->len, strlen(in));
        STRING_ASSERT_EQUALS(res->str, in);

        free(out);
        free(res);
    });

    TEST(nettype_true, {
        int   in  = 1;
        char* out = malloc(255);

        write_bool(out, in);
        int res = read_bool(out);

        INT_ASSERT_EQUALS(res, 1);

        free(out);
    });

    TEST(nettype_false, {
        int   in  = 0;
        char* out = malloc(255);

        write_bool(out, in);
        int res = read_bool(out);

        INT_ASSERT_EQUALS(res, 0);

        free(out);
    });
}
