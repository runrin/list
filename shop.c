#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <pwd.h>

#define PATH "/home/runrin/docs/shop"

/* zero pad months and days that are below 10 */
char* padint(int i) {
        static char buf[3] = {0};
        if (i < 10) {
                sprintf(buf, "0");
                snprintf(buf + strlen(buf), 10, "%d", i);
        }
        else
                snprintf(buf, 3, "%d", i);
        return &buf[0];
}

int lastmod(char *path) {
	struct stat attr;
	struct tm *lt;
        char* outstr;

        outstr = (char *) malloc(100);
	stat(path, &attr);

	lt = localtime(&attr.st_mtime);

        strcpy(outstr, padint(lt->tm_mon + 1));
        strcat(outstr, "/");
        strcat(outstr, padint(lt->tm_mday));
        strcat(outstr, " ");
        strcat(outstr, padint(lt->tm_hour));
        strcat(outstr, ":");
        strcat(outstr, padint(lt->tm_min));

        /* printf("%s ", outstr); */
        return(0);
}

int parsedir(char *path, struct dirent **out, unsigned *out_num)
{
        DIR *dir;
        unsigned int capacity;
        struct dirent *ent;
        *out = NULL;
        capacity = *out_num = 0;
        if ((dir = opendir(path)) != NULL) {
                while ((ent = readdir(dir)) != NULL) {
                        if(*out_num == capacity){
                          capacity = (capacity == 0) ? 16 : capacity << 1;
                          *out = realloc(*out, capacity * sizeof(struct dirent));
                        }
                        /* printf("In parsedir: '%s'\n", ent->d_name); */
                        memcpy(*out + *out_num, ent, sizeof(struct dirent));
                        ++*out_num;
                }
        closedir(dir);
        return(0);
        }
        else {
                perror("");
                return EXIT_FAILURE;
        }
}

int main(int argc, char*argv[])
{
        struct dirent *ent;
        struct stat attr;
        time_t t;
        unsigned count, i;
	char *fullpath;
        char *latestpath, *fulllatest, *newfile;

        FILE *fp, *touch;
        time_t nft;
        struct tm *lt;
        int c, nflag, eflag;
        char s;

        nflag = eflag = 0;
        
        fullpath = (char *) malloc(100);
        latestpath = (char *) malloc(100);
        fulllatest = (char *) malloc(100);
        newfile = (char *) malloc(100);


        while ((c = getopt (argc, argv, "n:eh")) != -1)
                switch (c)
                {
                        case 'n':
                                nflag = 1;
                                break;
                        case 'e':
                                eflag = 1;
                                break;
                        case 'h':
                                printf("Standard usage:\n");
                                printf("\tshop item1 item2 ...\n");
                                printf("Create a new list:\n");
                                printf("\tshop -n item1 item2 ...\n");
                                printf("Echo current list:\n");
                                printf("\tshop -e\n");
                                printf("See this help message:\n");
                                printf("\tshop -h\n");
                        default:
                                break;
                        return 1;
                }

        if (nflag) {
                time(&nft);
                lt = localtime(&nft);

                /* prepare the filename based on the first day of the week you are in */
                strcpy(newfile, PATH);
                strcat(newfile, "/");
                snprintf(newfile + strlen(PATH) + 1, 10, "%d", lt->tm_year + 1900);
                strcat(newfile, "-");
                strcat(newfile, padint(lt->tm_mon + 1));
                strcat(newfile, "-");
                strcat(newfile, padint(lt->tm_mday));
                strcat(newfile, "_");
                strcat(newfile, padint(lt->tm_hour));
                strcat(newfile, ":");
                strcat(newfile, padint(lt->tm_min));
                strcat(newfile, ":");
                strcat(newfile, padint(lt->tm_sec));
                strcat(newfile, "-sl.txt");
                
                printf("Creating new file %s...\n", newfile);
                touch = fopen(newfile, "a+");
                fclose(touch);
                printf("\n");

        }


        parsedir(PATH, &ent, &count);
        t = 0;

        /* printf("Finding most recent file...\n\n"); */

        for(i = 0; i < count; i++) {
                if (strcmp(ent[i].d_name, ".") == 0 || strcmp(ent[i].d_name, "..") == 0)
                        continue;
        	strcpy(fullpath, PATH);
                strcat(fullpath, "/");
                strcat(fullpath, ent[i].d_name);

                /*printf("%s\n", ent[i].d_name);*/
                lastmod(fullpath);
                /* printf("%s\n", fullpath); */

                stat(fullpath, &attr);

                /* printf("\ttime since epoch: %ld\n", attr.st_mtime); */
                if (attr.st_mtime > t) {
                        /* printf("\t%s was modified more recently than previous files.\n", ent[i].d_name); */
	                t = attr.st_mtime;
                        latestpath = ent[i].d_name;
                }
        }
        free(ent);


        strcpy(fulllatest, PATH);
        strcat(fulllatest, "/");
        strcat(fulllatest, latestpath);
        /* printf("\nThe most recently modified file is: %s\n\n", fulllatest); */


        if (eflag) {
                fp=fopen(fulllatest,"r");
                while((s=fgetc(fp))!=EOF) {
                        printf("%c",s);
                }
                fclose(fp);
                exit(0);
        }



        fp = fopen(fulllatest, "a+");
        for (int i = nflag ? 2 : 1; i < argc; i++) { 
                printf("appending \"%s\" to: %s\n", argv[i], latestpath);
                fprintf(fp, "%s\n", argv[i]);
        }
        fclose(fp);

}
