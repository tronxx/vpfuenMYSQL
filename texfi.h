/* Estructura especial para cada archivo textual */

    struct detex {
        char x_sit; /* flag for report requirement */
        FILE *x_fp; /* the pointer for the i/o file */
        char *x_s; /* In case it is opened for reading */
        int x_lpp, /* lines per page allowed */
            x_tlp, /* total lines in page */
            x_pg, /* page number */
            x_fl, /* line to start footer */
            x_lbh, /* label for header routine */
            x_lbf; /* label for footer routine */
        };
    typedef struct detex REPOR;
