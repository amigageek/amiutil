#include "app.h"

#ifdef APP_GUI
#include <proto/intuition.h>
#endif

#include <stdarg.h>

void print_error(const char* format, ...) {
    va_list args;
    va_start(args, format);

    static char message[128];
    vsnprintf(message, sizeof(message), format, args);

#ifdef APP_GUI
    if (IntuitionBase) {
        struct EasyStruct es = {
            .es_StructSize = sizeof(struct EasyStruct),
            .es_Title = "Error",
            .es_TextFormat = message,
            .es_GadgetFormat = "Ok",
        };

        EasyRequest(NULL, &es, NULL);
        return;
    }
#else
    fputs(message, stderr);
    fputc('\n', stderr);
#endif

    va_end(args);
}
