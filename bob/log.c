
/*

    log.c

    Logging support

*/

#include <stdio.h>
#include "inc/bob.h"
#include "inc/tokens.h"

#define LOG_NAME "log.txt"

FILE* g_fp = NULL;

// Open log
void LogOpen(void) {
    g_fp = fopen(LOG_NAME, "w");
}

// Close log
void LogClose(void) {
    if (g_fp) fclose (g_fp);
    g_fp = NULL;
}

// Write log message
void LogPrint(LPCSTR msg) {
    if (!g_fp) return;
    fprintf(g_fp, "%s\n", msg);
    fflush(g_fp);
}

void LogToken(int token) {
    LPCSTR str = NULL;
    switch (token) {
    case END_OF_FILE:   str = "EOF";        break;
    case UNKNOWN:       str = "UNKNOWN";    break;
    case NUMBER:        str = "NUMBER";     break;
    case LEFT_BRACE:    str = "{";          break;
    case RIGHT_BRACE:   str = "}";          break;
    case LEFT_PAREN:    str = "(";          break;
    case RIGHT_PAREN:   str = ")";          break;
    case PLUS:          str = "+";          break;
    case MINUS:         str = "-";          break;
    case TIMES:         str = "*";          break;
    case DIVIDE:        str = "/";          break;
    default:
        break;
    }
    if (!str) {
        str = lookup_token_name(token);
    }
    char sz[80];
    sprintf(sz, "Token: %s", str);
    LogPrint( sz );
}
