#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

char* itoa(int val, int base){
        static char buf[32] = {0};
        int i = 30;
        for(; val && i ; --i, val /= base)
        buf[i] = "0123456789abcdef"[val % base];
        return &buf[i+1];
}

char* paditoa(int i) {
        static char buf[2] = {0};
        if (i < 10) {
                strcpy(buf, "0");
                strcat(buf, itoa(i, 10));
        }
        else
                strcpy(buf, itoa(i, 10));
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

        strcpy(fn, "/home/runrin/docs/shop/");
        strcat(fn, itoa(lt->tm_year + 1900, 10));
        strcat(fn, "-");
        strcat(fn, paditoa(lt->tm_mon + 1));
        strcat(fn, "-");
        strcat(fn, paditoa(lt->tm_mday - lt->tm_wday));
        strcat(fn, "-sl.txt");
       
        fp = fopen(fn, "a+");
        for (int i = 1; i < argc; i++) { 
                printf("appending: \"%s\" to: %s\n", argv[i], fn);
                fprintf(fp, "%s\n", argv[i]);
        }
        fclose(fp);
}
