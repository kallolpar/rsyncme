/*
 * @file        test_rm7.c
 * @brief       Test suite #8.
 * @details     Test of rm_tx_local_push.
 * @author      Piotr Gregor <piotrek.gregor at gmail.com>
 * @version     0.1.2
 * @date        15 June 2016 10:44 PM
 * @copyright   LGPLv2.1
 */


#include "test_rm8.h"


const char* rm_test_fnames[RM_TEST_FNAMES_N] = {
    "rm_f_1_ts7", "rm_f_2_ts7","rm_f_4_ts7", "rm_f_8_ts7", "rm_f_65_ts7",
    "rm_f_100_ts7", "rm_f_511_ts7", "rm_f_512_ts7", "rm_f_513_ts7", "rm_f_1023_ts7",
    "rm_f_1024_ts7", "rm_f_1025_ts7", "rm_f_4096_ts7", "rm_f_7787_ts7", "rm_f_20100_ts7"};

uint32_t	rm_test_fsizes[RM_TEST_FNAMES_N] = { 1, 2, 4, 8, 65,
                                                100, 511, 512, 513, 1023,
                                                1024, 1025, 4096, 7787, 20100 };

uint32_t
rm_test_L_blocks[RM_TEST_L_BLOCKS_SIZE] = { 1, 2, 3, 4, 8, 10, 13, 16,
                    24, 32, 50, 64, 100, 127, 128, 129,
                    130, 200, 400, 499, 500, 501, 511, 512,
                    513, 600, 800, 1000, 1100, 1123, 1124, 1125,
                    1200, 100000 };

static int
test_rm_copy_files_and_postfix(const char *postfix) {
    int         err;
    FILE        *f, *f_copy;
    uint32_t    i, j;
    char        buf[RM_FILE_LEN_MAX + 50];
    unsigned long const seed = time(NULL);

    i = 0;
    for (; i < RM_TEST_FNAMES_N; ++i) {
        f = fopen(rm_test_fnames[i], "rb");
        if (f == NULL) {
            /* file doesn't exist, create */
            RM_LOG_INFO("Creating file [%s]", rm_test_fnames[i]);
            f = fopen(rm_test_fnames[i], "wb+");
            if (f == NULL) {
                exit(EXIT_FAILURE);
            }
            j = rm_test_fsizes[i];
            RM_LOG_INFO("Writing [%u] random bytes to file [%s]", j, rm_test_fnames[i]);
            srand(seed);
            while (j--) {
                fputc(rand(), f);
            }
        } else {
            RM_LOG_INFO("Using previously created file [%s]", rm_test_fnames[i]);
        }
        strncpy(buf, rm_test_fnames[i], RM_FILE_LEN_MAX); /* create copy */
        strncpy(buf + strlen(buf), postfix, 49);
        buf[RM_FILE_LEN_MAX + 49] = '\0';
        f_copy = fopen(buf, "rb+");
        if (f_copy == NULL) { /* if doesn't exist then create */
            RM_LOG_INFO("Creating copy [%s] of file [%s]", buf, rm_test_fnames[i]);
            f_copy = fopen(buf, "wb");
            if (f_copy == NULL) {
                RM_LOG_ERR("Can't open [%s] copy of file [%s]", buf, rm_test_fnames[i]);
                return -1;
            }
            err = rm_copy_buffered(f, f_copy, rm_test_fsizes[i]);
            switch (err) {
                case 0:
                    break;
                case -2:
                    RM_LOG_ERR("Can't write from [%s] to it's copy  [%s]", buf, rm_test_fnames[i]);
                    fclose(f);
                    fclose(f_copy);
                    return -2;
                case -3:
                    RM_LOG_ERR("Can't write from [%s] to it's copy  [%s], error set on original file", buf, rm_test_fnames[i]);
                    fclose(f);
                    fclose(f_copy);
                    return -3;
                case -4:
                    RM_LOG_ERR("Can't write from [%s] to it's copy  [%s], error set on copy", buf, rm_test_fnames[i]);
                    fclose(f);
                    fclose(f_copy);
                    return -4;
                default:
                    RM_LOG_ERR("Can't write from [%s] to it's copy  [%s], unknown error", buf, rm_test_fnames[i]);
                    fclose(f);
                    fclose(f_copy);
                    return -13;
            }
            fclose(f);
            fclose(f_copy);
        } else {
            RM_LOG_INFO("Using previously created copy of file [%s]", rm_test_fnames[i]);
        }
    }
    return 0;
}

static int
test_rm_delete_copies_of_files_postfixed(const char *postfix) {
    int         err;
    uint32_t    i;
    char        buf[RM_FILE_LEN_MAX + 50];

    i = 0;
    for (; i < RM_TEST_FNAMES_N; ++i) {
        strncpy(buf, rm_test_fnames[i], RM_FILE_LEN_MAX); /* open copy */
        strncpy(buf + strlen(buf), postfix, 49);
        buf[RM_FILE_LEN_MAX + 49] = '\0';
        RM_LOG_INFO("Removing (unlink) [%s] copy of file [%s]", buf, rm_test_fnames[i]);
        err = unlink(buf);
        if (err != 0) {
            RM_LOG_ERR("Can't unlink [%s] copy of file [%s]", buf, rm_test_fnames[i]);
            return -1;
        }
    }
    return 0;
}

int
test_rm_setup(void **state) {
    int         err;
    uint32_t    i,j;
    FILE        *f;
    void        *buf;
    struct rm_session   *s;
    unsigned long seed;

#ifdef DEBUG
    err = rm_util_chdir_umask_openlog("../build/debug", 1, "rsyncme_test_8");
#else
    err = rm_util_chdir_umask_openlog("../build/release", 1, "rsyncme_test_8");
#endif
    if (err != 0) {
        exit(EXIT_FAILURE);
    }
    rm_state.l = rm_test_L_blocks;
    *state = &rm_state;

    i = 0;
    seed = time(NULL);
    for (; i < RM_TEST_FNAMES_N; ++i) {
        f = fopen(rm_test_fnames[i], "rb+");
        if (f == NULL) {
            /* file doesn't exist, create */
            RM_LOG_INFO("Creating file [%s]", rm_test_fnames[i]);
            f = fopen(rm_test_fnames[i], "wb");
            if (f == NULL) {
                exit(EXIT_FAILURE);
            }
            j = rm_test_fsizes[i];
            RM_LOG_INFO("Writing [%u] random bytes to file [%s]", j, rm_test_fnames[i]);
            srand(seed);
            while (j--) {
                fputc(rand(), f);
            }
        } else {
            RM_LOG_INFO("Using previously created file [%s]", rm_test_fnames[i]);
        }
        fclose(f);
    }

    /* find biggest L */
    i = 0;
    j = 0;
    for (; i < RM_TEST_L_BLOCKS_SIZE; ++i) {
        if (rm_test_L_blocks[i] > j) j = rm_test_L_blocks[i];
    }
    buf = malloc(j);
    if (buf == NULL) {
        RM_LOG_ERR("Can't allocate 1st memory buffer of [%u] bytes, malloc failed", j);
	}
    assert_true(buf != NULL);
    rm_state.buf = buf;
    buf = malloc(j);
    if (buf == NULL) {
        RM_LOG_ERR("Can't allocate 2nd memory buffer of [%u] bytes, malloc failed", j);
	}
    assert_true(buf != NULL);
    rm_state.buf2 = buf;

    /* session for local push */
    s = rm_session_create(RM_PUSH_LOCAL, 0);
    if (s == NULL) {
        RM_LOG_ERR("Can't allocate session local push");
	}
    assert_true(s != NULL);
    rm_state.s = s;
    return 0;
}

int
test_rm_teardown(void **state) {
    int     i;
    FILE    *f;
    struct  test_rm_state *rm_state;

    rm_state = *state;
    assert_true(rm_state != NULL);
    if (RM_TEST_8_DELETE_FILES == 1) {
        i = 0; /* delete all test files */
        for (; i < RM_TEST_FNAMES_N; ++i) {
            f = fopen(rm_test_fnames[i], "wb+");
            if (f == NULL) {
                RM_LOG_ERR("Can't open file [%s]", rm_test_fnames[i]);	
            } else {
                RM_LOG_INFO("Removing file [%s]", rm_test_fnames[i]);
                remove(rm_test_fnames[i]);
            }
        }
    }
    free(rm_state->buf);
    free(rm_state->buf2);
    rm_session_free(rm_state->s);
    return 0;
}

/* @brief   Test if result file @f_z is reconstructed properly
 *          when x file is same as y (file has no changes). */
void
test_rm_tx_local_push_1(void **state) {
    return;
}

/* @brief   Test #2. */
/* @brief   Test if result file @f_z is reconstructed properly
 *          when x is copy of y, but first byte in x is changed. */
void
test_rm_tx_local_push_2(void **state) {
    int                     err;
    char                    buf_x_name[RM_FILE_LEN_MAX + 50];   /* @x (copy of @y with changed single byte at the beginning) */
    const char              *f_y_name;  /* @y name */
    unsigned char           cx, cz;
    FILE                    *f_copy, *f_x, *f_y;
    struct test_rm_file     *f_z;
    int                     fd_x, fd_y, fd_z;
    size_t                  i, j, k, L, f_x_sz, f_y_sz, f_z_sz;
    struct test_rm_state    *rm_state;
    struct stat             fs;
    struct rm_session_push_local    *prvt;
    rm_push_flags                   flags;
    size_t                          send_threshold;

    err = test_rm_copy_files_and_postfix("_test_1");
    if (err != 0) {
        RM_LOG_ERR("Error copying files, skipping test");
        return;
    }

    rm_state = *state;
    assert_true(rm_state != NULL);

    /* a storage for our result file metainfo */
    f_z = &rm_state->f_z;

    /* test on all files */
    i = 0;
    for (; i < RM_TEST_FNAMES_N; ++i) {
        f_y_name = rm_test_fnames[i];
        f_y = fopen(f_y_name, "rb");
        if (f_y == NULL) {
            RM_LOG_PERR("Can't open file [%s]", f_y_name);
        }
        assert_true(f_y != NULL);
        /* get file size */
        fd_y = fileno(f_y);
        memset(&fs, 0, sizeof(fs));
        if (fstat(fd_y, &fs) != 0) {
            RM_LOG_PERR("Can't fstat file [%s]", f_y_name);
            fclose(f_y);
            assert_true(1 == 0);
        }
        f_y_sz = fs.st_size;
        if (f_y_sz < 2) {
            RM_LOG_INFO("File [%s] size [%u] is too small for this test, skipping", f_y_name, f_y_sz);
            fclose(f_y);
            continue;
        }

        /* change byte in copy */
        strncpy(buf_x_name, f_y_name, RM_FILE_LEN_MAX);
        strncpy(buf_x_name + strlen(buf_x_name), "_test_2", 49);
        buf_x_name[RM_FILE_LEN_MAX + 49] = '\0';
        f_copy = fopen(buf_x_name, "rb+");
        if (f_copy == NULL) {
            RM_LOG_PERR("Can't open file [%s]", buf_x_name);
        }
        f_x = f_copy;
        /* get @x size */
        fd_x = fileno(f_x);
        memset(&fs, 0, sizeof(fs));
        if (fstat(fd_x, &fs) != 0) {
            RM_LOG_PERR("Can't fstat file [%s]", buf_x_name);
            fclose(f_x);
            assert_true(1 == 0);
        }
        f_x_sz = fs.st_size;
        /* read first byte */
        if (rm_fpread(&cx, sizeof(unsigned char), 1, 0, f_x) != 1) {
            RM_LOG_ERR("Error reading file [%s], skipping this test", buf_x_name);
            fclose(f_x);
            fclose(f_y);
            continue;
        }
        /* change first byte, so ZERO_DIFF delta can't happen in this test, therefore this would be an error in this test */
        cx = (cx + 1) % 256;
        if (rm_fpwrite(&cx, sizeof(unsigned char), 1, 0, f_x) != 1) {
            RM_LOG_ERR("Error writing to file [%s], skipping this test", buf_x_name);
            fclose(f_x);
            fclose(f_y);
            continue;
        }

        j = 0;
        for (; j < RM_TEST_L_BLOCKS_SIZE; ++j) {
            L = rm_test_L_blocks[j];
            RM_LOG_INFO("Validating testing #1 of local push, file [%s], size [%u], block size L [%u]", f_y_name, f_y_sz, L);
            if (0 == L) {
                RM_LOG_INFO("Block size [%u] is too small for this test (should be > [%u]), skipping file [%s]", L, 0, f_y_name);
                continue;
            }
            if (f_y_sz < 2) {
                RM_LOG_INFO("File [%s] size [%u] is too small for this test, skipping", f_y_name, f_y_sz);
                continue;
            }
            RM_LOG_INFO("Testing local push #1: file @x[%s] size [%u] file @y[%s], size [%u], block size L [%u]", buf_x_name, f_x_sz, f_y_name, f_y_sz, L);

  err = rm_tx_local_push(buf_x_name, f_y_name, L, send_threshold, flags);
            assert_int_equal(err, 0);


            /* verify files size */
            if (fflush(f_z->f) !=0) {
                RM_LOG_PERR("Can't fflush file [%s]", f_z->name);
                fclose(f_x);
                fclose(f_y);
                fclose(f_z->f);
                assert_true(1 == 0);
            }
            memset(&fs, 0, sizeof(fs)); /* get @x size */
            if (fstat(fd_x, &fs) != 0) {
                RM_LOG_PERR("Can't fstat file [%s]", buf_x_name);
                fclose(f_x);
                fclose(f_y);
                fclose(f_z->f);
                assert_true(1 == 0);
            }
            f_x_sz = fs.st_size;
            fd_z = fileno(f_z->f);  /* get @z size */
            memset(&fs, 0, sizeof(fs));
            if (fstat(fd_z, &fs) != 0) {
                RM_LOG_PERR("Can't fstat file [%s]", f_z->name);
                fclose(f_x);
                fclose(f_y);
                fclose(f_z->f);
                assert_true(1 == 0);
            }
            f_z_sz = fs.st_size;
            assert_true(f_x_sz == f_z_sz && "File sizes differ!");

            k = 0;
            while (k < f_x_sz) {
                if (rm_fpread(&cx, sizeof(unsigned char), 1, k, prvt->f_x) != 1) {
                    RM_LOG_CRIT("Error reading file [%s]!", buf_x_name);
                    fclose(f_x);
                    fclose(f_y);
                    fclose(f_z->f);
                    assert_true(1 == 0 && "ERROR reading byte in file @x!");
                }
                if (rm_fpread(&cz, sizeof(unsigned char), 1, k, prvt->f_z) != 1) {
                    RM_LOG_CRIT("Error reading file [%s]!", f_z->name);
                    fclose(f_x);
                    fclose(f_y);
                    fclose(f_z->f);
                    assert_true(1 == 0 && "ERROR reading byte in file @z!");
                }
                if (cx != cz) {
                    RM_LOG_CRIT("Bytes [%u] differ: cx [%u], cz [%u]\n");
                }
                assert_true(cx == cz && "Bytes differ!");
                ++k;
            }

            /* move file pointer back to the beginning */
            rewind(f_x);
            rewind(f_y);

            /* close result file */
            fclose(f_z->f);

            /* and unlink/remove */
            if (RM_TEST_8_DELETE_FILES == 1) {
                if (unlink(f_z->name) != 0) {
                    RM_LOG_ERR("Can't unlink result file [%s]", f_z->name);
                    assert_true(1 == 0);
                }
            }
		}
		fclose(f_x);
        fclose(f_y);
        RM_LOG_INFO("PASSED test #1: files [%s] [%s] passed delta reconstruction for all blocks sizes, files are the same", f_y_name, f_z->name);
	}

    if (RM_TEST_8_DELETE_FILES == 1) {
        err = test_rm_delete_copies_of_files_postfixed("_test_1");
        if (err != 0) {
            RM_LOG_ERR("Error removing files (unlink)");
            assert_true(1 == 0 && "Error removing files (unlink)");
            return;
        }
    }
    return;
}
