#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <errno.h>
#include "report.h"

void Info(LPCSTR szMsg) {
    fprintf(stdout, "%s\n", szMsg ? szMsg : "(unspecified info)");
}

void Warning(LPCSTR szMsg) {
    fprintf(stderr, "W: %s\n", szMsg ? szMsg : "(unspecified warning)");
}

void Error(LPCSTR szMsg) {
    fprintf(stderr, "E: %s\n", szMsg ? szMsg : "(unspecified error)");
    exit(1);
}

