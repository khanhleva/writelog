#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <math.h>
#include <string.h>
#define NARGS_SEQ(_1,_2,_3,_4,_5,_6,_7,_8,_9,N,...) N
#define NARGS(...) NARGS_SEQ(__VA_ARGS__, 9, 8, 7, 6, 5, 4, 3, 2, 1)
#define localtrace(...) localtrace(NARGS(__VA_ARGS__), __VA_ARGS__)
#define getstring(...) getstring(__VA_ARGS__)

#define MAX_LENGTH_SIZE 200

#define V_(...) __VA_ARGS__
#define I_(M,...) V_(M(__VA_ARGS__))

#define DLT_TRACE(...)  {\
                            struct Timestamp timestamp = getTimestamp();\
                            char infolog[MAX_LENGTH_SIZE] = "";\
                            sprintf(infolog, "[%s][%s][%s][%d] ", timestamp.timestring, (char*)__FILE__, (char*)__FUNCTION__, __LINE__);\
                            char strtext[MAX_LENGTH_SIZE] = "";\
                            char* mstr = I_(getstring, __VA_ARGS__);\
                            if(mstr != NULL)\
                            {\
                                strcpy(strtext, mstr);\
                            }\
                            localtrace(infolog, strtext);\
                        }

struct Timestamp {
    time_t seconds;
    long milliseconds;
    char timestring[32];
};

struct Timestamp getTimestamp()
{
    char   timebuffer[32]     = {0};
    struct tm      *tmval     = NULL;
    struct tm       gmtval    = {0};
    struct timespec curtime   = {0};

    struct Timestamp timestamp;

    // Get current time
    clock_gettime(CLOCK_REALTIME, &curtime);

    // Set the fields
    timestamp.seconds      = curtime.tv_sec;
    timestamp.milliseconds = curtime.tv_nsec/1.0e6;

    if((tmval = gmtime_r(&timestamp.seconds, &gmtval)) != NULL)
    {
        // Build the first part of the time
        strftime(timebuffer, sizeof timebuffer, "%Y-%m-%d %H:%M:%S", &gmtval);

        // Add the milliseconds part and build the time string
        snprintf(timestamp.timestring, sizeof timestamp.timestring, "%s.%03ld", timebuffer, timestamp.milliseconds); 
    }

    return timestamp;
}

void (localtrace)(int num, ...)
{
    FILE *fp;
    fp = fopen("/tmp/lvk.log", "r+");
    fseek(fp, 0, SEEK_END);
    int current_pos = ftell(fp);
    fseek(fp, current_pos, SEEK_SET);
    va_list args;
    va_start(args, NULL);
    for(int i = 0; i < num; i++)
    {
        char* info = va_arg(args, char*);
        printf("info[%s]", info);
        fwrite(info, 1, strlen(info), fp);
    }
    fputs("\n", fp);
    printf("\n");
    va_end(args);
    fclose(fp);
}

char* getstring(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    char* result = malloc(MAX_LENGTH_SIZE);
    if(!result)
        return NULL;
    vsprintf(result, fmt, args);
    printf("result[%s]", result);
    return result;
}

int main()
{
    for(int i = 0; i < 100; i++)
    {
        DLT_TRACE("%d, %s", 5, "LVK");
    }
}