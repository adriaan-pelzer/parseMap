#ifndef _PARSEMAP_H_
#define _PARSEMAP_H_

struct El {
    char *el;
    char *id;
    char *ref;
    char *name;
    char *lat;
    char *lon;
};

struct ParserCtx {
    int size;
    struct El *els;
};

typedef void (*handler)( void *user_data, struct ParserCtx *parser_ctx, char tag_open );

int parseMap_read ( char *filename );
void parseMap_register_handler ( handler element_handler );
void parseMap_register_user_data ( void *user_data );

#endif
