#include <expat.h>
#include <stdio.h>
#include <string.h>
#include "parseMap.h"

#define MAXCHARS 1000000

static char handler_registered = 0;
static handler elm_handler;

void *user_data = NULL;

void update_ctx ( struct ParserCtx *parser_ctx, const char *el, const char **attr, char open ) {
    int i = 0;

    if ( open ) {
        char key_is_name = 0;

        parser_ctx->size++;
        parser_ctx->els = realloc ( parser_ctx->els, parser_ctx->size * sizeof ( struct El ) );
        memset ( &parser_ctx->els[parser_ctx->size - 1], 0, sizeof ( struct El ) );
        parser_ctx->els[parser_ctx->size - 1].el = strdup ( el );

        for ( i = 0; attr[i]; i += 2 ) {
            if ( !strncmp ( attr[i], "id", 2 ) ) {
                parser_ctx->els[parser_ctx->size - 1].id = strdup ( attr[i + 1] );
            } else if ( !strncmp ( attr[i], "ref", 3 ) ) {
                parser_ctx->els[parser_ctx->size - 1].ref = strdup ( attr[i + 1] );
            } else if ( !strncmp ( attr[i], "lat", 3 ) ) {
                parser_ctx->els[parser_ctx->size - 1].lat = strdup ( attr[i + 1] );
            } else if ( !strncmp ( attr[i], "lon", 3 ) ) {
                parser_ctx->els[parser_ctx->size - 1].lon = strdup ( attr[i + 1] );
            } else if ( !strncmp ( attr[i], "k", 1 ) && !strncmp ( attr[i+1], "name", 4 ) ) {
                key_is_name = 1;
            } else if ( key_is_name && !strncmp ( attr[i], "v", 1 ) ) {
                key_is_name = 0;
                parser_ctx->els[parser_ctx->size - 1].name = strdup ( attr[i + 1] );
            } else {
                continue;
            }
        }
    } else {
        parser_ctx->size--;

        if (parser_ctx->els[parser_ctx->size].el)
            free (parser_ctx->els[parser_ctx->size].el);
        if (parser_ctx->els[parser_ctx->size].id)
            free (parser_ctx->els[parser_ctx->size].id);
        if (parser_ctx->els[parser_ctx->size].ref)
            free (parser_ctx->els[parser_ctx->size].ref);
        if (parser_ctx->els[parser_ctx->size].name)
            free (parser_ctx->els[parser_ctx->size].name);
        if (parser_ctx->els[parser_ctx->size].lat)
            free (parser_ctx->els[parser_ctx->size].lat);
        if (parser_ctx->els[parser_ctx->size].lon)
            free (parser_ctx->els[parser_ctx->size].lon);

        parser_ctx->els = realloc ( parser_ctx->els, parser_ctx->size * sizeof ( struct El ) );
    }
}

void handler_start ( void *data, const char *el, const char **attr ) {
    struct ParserCtx *parser_ctx = (struct ParserCtx *) data;

    update_ctx ( parser_ctx, el, attr, 1 );
    if ( handler_registered )
        elm_handler ( user_data, parser_ctx, 1 );
}

void handler_end ( void *data, const char *el ) {
    struct ParserCtx *parser_ctx = (struct ParserCtx *) data;

    update_ctx ( parser_ctx, el, NULL, 0 );
    if ( handler_registered )
        elm_handler ( user_data, parser_ctx, 0 );
}

int parseMap_read ( char *filename ) {
    int rc = EXIT_FAILURE;
    FILE *fp = NULL;
    XML_Parser parser = NULL;
    struct ParserCtx parser_ctx;
    char *xml_text = NULL;
    size_t size = 0, count = 0;

    if ( ( xml_text = calloc ( 1, MAXCHARS ) ) == NULL ) {
        goto over;
    }

    memset ( &parser_ctx, 0, sizeof ( struct ParserCtx ) );

    if ( ( parser = XML_ParserCreate ( "UTF-8" ) ) == NULL ) {
        goto over;
    }

    XML_SetUserData ( parser, &parser_ctx );

    XML_SetElementHandler ( parser, handler_start, handler_end );

    if ( ( fp = fopen ( filename, "r" ) ) == NULL ) {
        goto over;
    }

    while ( ( size = fread ( xml_text, sizeof ( char ), MAXCHARS, fp ) ) > 0 ) {
        count += size;

        if ( XML_Parse ( parser, xml_text, size, size < MAXCHARS ? XML_TRUE : XML_FALSE ) == XML_STATUS_ERROR ) {
            goto over;
        }
    }

    rc = EXIT_SUCCESS;
over:
    if ( xml_text )
        free ( xml_text );
    if ( parser_ctx.els )
        free ( parser_ctx.els );
    if ( fp )
        fclose ( fp );
    if ( parser )
        XML_ParserFree ( parser );

    return rc;
}

void parseMap_register_handler ( handler element_handler ) {
    elm_handler = element_handler;
    handler_registered = 1;
}

void parseMap_register_user_data ( void *u_data ) {
    user_data = u_data;
}
