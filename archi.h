/* Structure to define data file parameters */
    struct _archi {
        int a_fd; /* file descriptor */
        int a_act, /* 0= read only 2 = readn and write (mode) */
            a_k, /* active key */
            a_sz; /* size of record */
        char a_type; /* 0=normal 1=VSAM */
        struct vsam *a_vsam; /* pointer to a VSAM structure */
        char *a_buf; /* buffer for record data */
        char *a_dat; /* posicion de datos */
        long a_lug; /* actual position of file */
        long a_max; /* size of file */
        };
    typedef struct _archi ARCHI;
    extern ARCHI *ar;
