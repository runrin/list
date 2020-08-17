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

/* zero pad integers that are below 10, and return the int as a string */
/* kind of like itoa(), but pads single digit numbers */
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

/* exists for testing purposes only really */
/* never actually called anymore */
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

/* take a path to a directory, a dirent array, and an unsigned int, and fill
 * populate the array with dirents of each file within the directory, out_num
 * is set to the total number of files */
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
        
        /* initialize strings so that strcpy() can be used later */
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
                                exit(0);
                        default:
                                break;
                        return 1;
                }

        /* create a new file, and append some items to the list */
        if (nflag) {
                time(&nft);
                lt = localtime(&nft);

                /* prepare the filename based on the current date/time */
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
                /* create the file if it doesnt exist */
                touch = fopen(newfile, "a+");
                fclose(touch);
                printf("\n");

        }


        parsedir(PATH, &ent, &count);
        
        /* we will store the time_t here */
        t = 0;

        /* removed so -e works with lp */
        /* printf("Finding most recent file...\n\n"); */

        for(i = 0; i < count; i++) {
                /* ignore ., and .. */
                if (strcmp(ent[i].d_name, ".") == 0 || strcmp(ent[i].d_name, "..") == 0)
                        continue;

        	strcpy(fullpath, PATH);
                strcat(fullpath, "/");
                strcat(fullpath, ent[i].d_name);

                /* removed so -e works with lp */
                /*printf("%s\n", ent[i].d_name);*/
                /* removed so -e works with lp */
                /* printf("%s\n", fullpath); */

                stat(fullpath, &attr);

                /* removed so -e works with lp */
                /* printf("\ttime since epoch: %ld\n", attr.st_mtime); */

                /* check if the current file's epoch time is larger than the largerst previous one. */
                if (attr.st_mtime > t) {
                        /* removed so -e works with lp */
                        /* printf("\t%s was modified more recently than previous files.\n", ent[i].d_name); */

                        /* store the new largest epoch time, and the filename */
	                t = attr.st_mtime;
                        latestpath = ent[i].d_name;
                }
        }
        free(ent);

        /* concatenate the most recently edited file with PATH/ */
        strcpy(fulllatest, PATH);
        strcat(fulllatest, "/");
        strcat(fulllatest, latestpath);
        /* removed so -e works with lp */
        /* printf("\nThe most recently modified file is: %s\n\n", fulllatest); */


        /* simply print the file to stdout, and exit */
        if (eflag) {
                fp=fopen(fulllatest,"r");
                while((s=fgetc(fp))!=EOF) {
                        printf("%c",s);
                }
                fclose(fp);
                exit(0);
        }


        /* add items to file */
        fp = fopen(fulllatest, "a+");
        for (int i = nflag ? 2 : 1; i < argc; i++) { 
                printf("appending \"%s\" to: %s\n", argv[i], latestpath);
                fprintf(fp, "%s\n", argv[i]);
        }
        fclose(fp);
        exit(0);
}
