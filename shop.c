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

void lastmod(char *path) {
	struct stat attr;
	struct tm *lt;
	stat(path, &attr);

	lt = localtime(&attr.st_mtime);
	printf("%d/%d\t%d:%d\t", (lt->tm_mon + 1), lt->tm_mday, lt->tm_hour, lt->tm_min);	
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

int main()
{
        struct dirent *ent;
        unsigned count, i;
	char *fullpath;
        
        fullpath = (char *) malloc(100);
        parsedir(PATH, &ent, &count);
        for(i = 0; i < count; i++) {
                if (strcmp(ent[i].d_name, ".") == 0 || strcmp(ent[i].d_name, "..") == 0)
                        continue;
        	strcpy(fullpath, PATH);
                strcat(fullpath, "/");
                strcat(fullpath, ent[i].d_name);

                /*printf("%s\n", ent[i].d_name);*/
                lastmod(fullpath);
                printf("%s\n", fullpath);
        }
        free(ent);
}
