#include "Application.h"

#include <proto/exec.h>
#include <stdarg.h>

#ifdef APPLICATION_GUI
#include <proto/intuition.h>
#endif

void check_stack(void) {
    struct Task* task = FindTask(nullptr);
    uint stack_free = (uint)task->tc_SPReg - (uint)task->tc_SPLower;
    printf("%u stack bytes free\n", stack_free);
}

void print_error(const char* format, ...) {
    va_list args;
    va_start(args, format);

    static char message[128];
    vsnprintf(message, sizeof(message), format, args);

#ifdef APPLICATION_GUI
    if (IntuitionBase) {
        struct EasyStruct es = {
            .es_StructSize = sizeof(struct EasyStruct),
            .es_Title = "Error",
            .es_TextFormat = message,
            .es_GadgetFormat = "Ok",
        };

        EasyRequest(nullptr, &es, nullptr);
        return;
    }
#else
    fputs(message, stderr);
    fputc('\n', stderr);
#endif

    va_end(args);
}
