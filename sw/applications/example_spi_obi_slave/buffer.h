#define DATA_1_LENGTH sizeof(test_data_1) //In bytes 
#define DATA_2_LENGTH sizeof(test_data_2) //In bytes 
#define DATA_3_LENGTH sizeof(test_data_3) //In bytes 


uint32_t test_data_1[] = {
    0x76543211, 0xfedcba99, 0x579a6f91, 0x657d5bef, 0x758ee420, 0x01234568, 0xfedbca97, 0x89abde00,
    0x76543212, 0xfedcba9a, 0x579a6f92, 0x657d5bf0, 0x758ee421, 0x01234569, 0xfedbca98, 0x89abde01,
    0x76543213, 0xfedcba9b, 0x579a6f93, 0x657d5bf1, 0x758ee422, 0x0123456a, 0xfedbca99, 0x89abde02,
    0x76543214, 0xfedcba9c, 0x579a6f94, 0x657d5bf2, 0x758ee423, 0x0123456b, 0xfedbca9a, 0x89abde03,
    0x76543215, 0xfedcba9d, 0x579a6f95, 0x657d5bf3, 0x758ee424, 0x0123456c, 0xfedbca9b, 0x89abde04,
    0x76543216, 0xfedcba9e, 0x579a6f96, 0x657d5bf4, 0x758ee425, 0x0123456d, 0xfedbca9c, 0x89abde05,
    0x76543217, 0xfedcba9f, 0x579a6f97, 0x657d5bf5, 0x758ee426, 0x0123456e, 0xfedbca9d, 0x89abde06,
    0x76543218, 0xfedcbaa0, 0x579a6f98, 0x657d5bf6, 0x758ee427, 0x0123456f, 0xfedbca9e, 0x89abde07,
    0x76543219, 0xfedcbaa1, 0x579a6f99, 0x657d5bf7, 0x758ee428, 0x01234570, 0xfedbca9f, 0x89abde08,
    0x7654321a, 0xfedcbaa2, 0x579a6f9a, 0x657d5bf8, 0x758ee429, 0x01234571, 0xfedbcaa0, 0x89abde09,
    0x7654321b, 0xfedcbaa3, 0x579a6f9b, 0x657d5bf9, 0x758ee42a, 0x01234572, 0xfedbcaa1, 0x89abde0a,
    0x7654321c, 0xfedcbaa4, 0x579a6f9c, 0x657d5bfa, 0x758ee42b, 0x01234573, 0xfedbcaa2, 0x89abde0b,
    0x7654321d, 0xfedcbaa5, 0x579a6f9d, 0x657d5bfb, 0x758ee42c, 0x01234574, 0xfedbcaa3, 0x89abde0c,
    0x7654321e, 0xfedcbaa6, 0x579a6f9e, 0x657d5bfc, 0x758ee42d, 0x01234575, 0xfedbcaa4, 0x89abde0d,
    0x7654321f, 0xfedcbaa7, 0x579a6f9f, 0x657d5bfd, 0x758ee42e, 0x01234576, 0xfedbcaa5, 0x89abde0e,
    0x76543220, 0xfedcbaa8, 0x579a6fa0, 0x657d5bfe, 0x758ee42f, 0x01234577, 0xfedbcaa6, 0x89abde0f,
    0x76543221, 0xfedcbaa9, 0x579a6fa1, 0x657d5bff, 0x758ee430, 0x01234578, 0xfadbcaa7, 0x89abde10,
    0x76543222, 0xfedcbaaa, 0x579a6fa2, 0x657d5c00, 0x758ee431, 0x01234579, 0xfadbcaa8, 0x89abde11,
    0x76543223, 0xfedcbaab, 0x579a6fa3, 0x657d5c01, 0x758ee432, 0x0123457a, 0xfadbcaa9, 0x89abde12,
    0x76543224, 0xfedcbaac, 0x579a6fa4, 0x657d5c02, 0x758ee433, 0x0123457b, 0xfadbcaaa, 0x89abde13,
    0x76543225, 0xfedcbaad, 0x579a6fa5, 0x657d5c03, 0x758ee434, 0x0123457c, 0xfadbcaab, 0x89abde14,
    0x76543226, 0xfedcbaae, 0x579a6fa6, 0x657d5c04, 0x758ee435, 0x0123457d, 0xfadbcaac, 0x89abde15,
    0x76543227, 0xfedcbaaf, 0x579a6fa7, 0x657d5c05, 0x758ee436, 0x0123457e, 0xfadbcaad, 0x89abde16,
    0x76543228, 0xfedcbab0, 0x579a6fa8, 0x657d5c06, 0x758ee437, 0x0123457f, 0xfadbcaae, 0x89abde17,
    0x76543220, 0xfedcbaa8, 0x579a6fa0, 0x657d5bfe, 0x758ee42f, 0x01234577, 0xfedbcaa6, 0x89abde0f,
    0x76543221, 0xfedcbaa9, 0x579a6fa1, 0x657d5bff, 0x758ee430, 0x01234578, 0xfadbcaa7, 0x89abde10,
    0x76543222, 0xfedcbaaa, 0x579a6fa2, 0x657d5c00, 0x758ee431, 0x01234579, 0xfadbcaa8, 0x89abde11,
    0x76543223, 0xfedcbaab, 0x579a6fa3, 0x657d5c01, 0x758ee432, 0x0123457a, 0xfadbcaa9, 0x89abde12,
    0x76543224, 0xfedcbaac, 0x579a6fa4, 0x657d5c02, 0x758ee433, 0x0123457b, 0xfadbcaaa, 0x89abde13,
    0x76543225, 0xfedcbaad, 0x579a6fa5, 0x657d5c03, 0x758ee434, 0x0123457c, 0xfadbcaab, 0x89abde14,
    0x76543226, 0xfedcbaae, 0x579a6fa6, 0x657d5c04, 0x758ee435, 0x0123457d, 0xfadbcaac, 0x89abde15,
    0x76543227, 0xfedcbaaf, 0x579a6fa7, 0x657d5c05, 0x758ee436, 0x0123457e, 0xfadbcaad, 0x89abde16,
    0x76543228, 0xfedcbab0, 0x579a6fa8, 0x657d5c06, 0x758ee437, 0x0123457f, 0xfadbcaae, 0x89abde17
};


uint32_t test_data_2[] = {
    0x12345678, 0xabcdef01, 0x87654321, 0x0fedcba9, 0x11223344, 0x99aabbcc, 0x55ffccaa, 0x10203040,
    0xaabbccdd, 0x33445566, 0x77889900, 0xffeeddcc, 0xdeadbeef, 0xcafebabe, 0x0badc0de, 0xbeadfeed,
    0x01234567, 0x89abcdef, 0x76543210, 0xf0e1d2c3
};


uint32_t test_data_3[] = {
    0x1a2b3c4d, 0x5e6f7283, 0x9abcdef0, 0x01234567, 0x89abcdef, 0x76543210, 0xabcdef01, 0xdeadbeef,
    0xcafebabe, 0x0badc0de, 0xfacefeed, 0x11223344, 0x55667788, 0x99aabbcc, 0xddeeff00, 0x01234567,
    0x89abcdef, 0x1f2e3d4c, 0x5a6b7c8d, 0x9e0f1a2b, 0x3c4d5e6f, 0x7e829abc, 0xdef01234, 0x56789abc,
    0xabcdef01, 0xfedcba98, 0x76543210, 0x01234567, 0x89abcdef, 0x22334455, 0x66778899, 0xaabbccdd,
    0xdeadbeef, 0xcafebabe, 0x0badc0de, 0xfacefeed, 0xbadc0ffe, 0xd00dfeed, 0xfeedc0de, 0xbeadfeed,
    0xf00dbabe, 0xc001d00d, 0xabcddcba, 0x12345678, 0x9abcdef0, 0xabcdef01, 0x87654321, 0xfedcba98,
    0x01234567, 0x89abcdef, 0xfacecafe, 0xdeadf00b, 0xcafef00d, 0x00112233, 0x44556677, 0x8899aabb,
    0xccddeeff, 0x10203040, 0x50607080, 0x90a0b0c0, 0xd0e0f000, 0x12345678, 0xdeadbeef, 0xcafebabe,
    0x89abcdef, 0x22334455, 0x66778899
};
