#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

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
        char fn[50], chkfn[50], path[50];
        int c, nflag;

        nflag = 0;
        while ((c = getopt (argc, argv, "n:")) != -1)
                switch (c)
                {
                        case 'n':
                                nflag = 1;
                                break;
                        default:
                                break;
                        return 1;
                }

        time(&t);
        lt = localtime(&t);

        /* put your shopping list directory here */
        strcpy(path, "/home/runrin/docs/shop/");

        /* prepare the filename based on the first day of the week you are in */
        strcpy(fn, path);
        snprintf(fn + strlen(fn), 10, "%d", lt->tm_year + 1900);
        strcat(fn, "-");
        strcat(fn, padint(lt->tm_mon + 1));
        strcat(fn, "-");
        strcat(fn, padint(lt->tm_mday - lt->tm_wday));
        strcat(fn, "-sl");

        /* handle existing -2 file, and -n flag  */
        strcpy(chkfn, fn);
        strcat(chkfn, "-2.txt");
        if (access(chkfn, F_OK) != -1) {
                strcat(fn, "-2.txt");
                printf("A second list exists for this week.\n");
        }
        else 
                strcat(fn, nflag ? "-2.txt" : ".txt");


        if (nflag)
                printf("Creating a second list for this week.\n");

        fp = fopen(fn, "a+");
        for (int i = nflag ? 2 : 1; i < argc; i++) { 
                printf("appending \"%s\" to: %s\n", argv[i], fn + strlen(path));
                fprintf(fp, "%s\n", argv[i]);
        }
        fclose(fp);
}
