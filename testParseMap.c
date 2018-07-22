#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <parseMap.h>

char is_child_of ( const char *tag_name, struct ParserCtx *parser_ctx ) {
    size_t i = 0;

    if ( parser_ctx->size )
        for ( i = parser_ctx->size; i; i-- )
            if ( ! strncmp ( parser_ctx->els[i-1].el, tag_name, strlen ( tag_name ) ) )
                return 1;

    return 0;
}

static void element_handler ( void *user_data, struct ParserCtx *parser_ctx, char tag_open ) {
    if ( parser_ctx->size == 0 ) {
        printf ( "END\n" );
    } else {
        struct El current_element = parser_ctx->els[parser_ctx->size - 1];

        if ( current_element.name )
            printf ( "%s: %s\n",
                    is_child_of ( "node", parser_ctx ) ? "node" :
                    is_child_of ( "way", parser_ctx ) ? "way" :
                    is_child_of ( "relation", parser_ctx ) ? "relation" :
                    "nothing",
                    current_element.name );
    }
}

int main ( int argc, char **argv ) {
    int rc = EXIT_FAILURE;
    char *filename = NULL;
    handler elm_handler = element_handler;

    if ( argc < 2 ) {
        fprintf ( stderr, "Usage: %s file\n", argv[0] );
        goto over;
    }

    filename = argv[1];

    parseMap_register_handler ( elm_handler );

    if ( parseMap_read ( filename ) == EXIT_FAILURE ) {
        fprintf ( stderr, "Cannot parse map: %s\n", strerror ( errno ) );
        goto over;
    }

    rc = EXIT_SUCCESS;
over:
    return rc;
}
