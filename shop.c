#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

/* zero pad months and days that are below 10 */
char* padint(int i) {
        static char buf[2] = {0};
        if (i < 10) {
                sprintf(buf, "0");
                snprintf(buf + strlen(buf), 10, "%d", i);
        }
        else
                snprintf(buf, 10, "%d", i);
        return &buf[0];
}
 
int main(int argc, char *argv[])
{
        FILE *fp;
        time_t t;
        struct tm *lt;
        char fn[30];

        time(&t);
        lt = localtime(&t);

        /* put your shopping list directory here */
        strcpy(fn, "/home/runrin/docs/shop/");
        snprintf(fn + strlen(fn), 10, "%d", lt->tm_year + 1900);
        strcat(fn, "-");
        strcat(fn, padint(lt->tm_mon + 1));
        strcat(fn, "-");
        strcat(fn, padint(lt->tm_mday - lt->tm_wday));
        strcat(fn, "-sl.txt");
       
        fp = fopen(fn, "a+");
        for (int i = 1; i < argc; i++) { 
                printf("appending: \"%s\" to: %s\n", argv[i], fn);
                fprintf(fp, "%s\n", argv[i]);
        }
        fclose(fp);
}
