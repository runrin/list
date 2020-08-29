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
#define NAME "XXXX-XX-XX_XX:XX:XX-sl.txt"

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
        int c, nflag, eflag, oflag;
        char s;

        nflag = eflag = oflag = 0;
        
        /* initialize strings so that strcpy() can be used later */
        fullpath = latestpath = fulllatest = newfile = (char *) malloc(100);


        while ((c = getopt (argc, argv, "n:eho")) != -1)
                switch (c)
                {
                        case 'n':
                                nflag = 1;
                                break;
                        case 'e':
                                eflag = 1;
                                break;
                        case 'o':
                                oflag = 1;
                                break;

                        case 'h':
                                printf("Usage: list [OPTION] item1 item2 ...\n");
                                printf("  -n\tcreate new list and append items\n");
                                printf("  -e\techo current list to stdout\n");
                                printf("  -o\topen current list in EDITOR\n");
                                printf("  -h\tdisplay this help and exit\n");
                                exit(0);
                        default:
                                break;
                        return 1;
                }

        /* create a new file with a name based on the date and time */
        if (nflag) {
                time_t nft;
                struct tm *lt;
                char *buffer;
                size_t full_len;

                time(&nft);
                lt = localtime(&nft);

                full_len = strlen(PATH) + strlen(NAME);
                buffer = malloc(full_len);

                strftime(newfile, strlen(NAME), "%F_%T-sl.txt", lt);
                printf("Creating new file %s...\n", newfile);

                strcpy(buffer, PATH);
                strcat(buffer, "/");
                strcat(buffer, newfile);
                printf("%s\n", buffer);

                touch = fopen(buffer, "a+");
                fclose(touch);
                free(buffer);
                printf("\n");
        }


        /* fill a dirent array with every file in the directory */
        parsedir(PATH, &ent, &count);
        
        /* we will store the time_t associated with each file here 
         * TODO why is this declared earlier, should it be within a smaller 
         * scope? */
        t = 0;

        for(i = 0; i < count; i++) {
                /* ignore . and .. */
                if (strcmp(ent[i].d_name, ".") == 0 || strcmp(ent[i].d_name, "..") == 0)
                        continue;

                /* TODO why is fullpath declared so much earlier, is this the
                 * correct thing to do? */
        	strcpy(fullpath, PATH);
                strcat(fullpath, "/");
                strcat(fullpath, ent[i].d_name);

                stat(fullpath, &attr);

                /* check if the current file's epoch time is larger than the largerst previous one. */
                if (attr.st_mtime > t) {
                        /* store the new largest epoch time, and the filename */
	                t = attr.st_mtime;
                        latestpath = ent[i].d_name;
                }
        }
        free(ent);

        /* concatenate the most recently edited file with PATH/
         * TODO again, fulllatest not really necessary */
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

        /* open file with EDITOR */
        if (oflag) {
		#define CHECK_COMMAND(X) ((system("which " X " 2>&1 >/dev/null") == 0) ? ("" X) : NULL)
                const char *editor;
                char *buffer;
                /* find some editor to use */
		if (!(
				(editor = getenv("EDITOR")) ||
				(editor = CHECK_COMMAND("nano")) ||
				(editor = CHECK_COMMAND("pico")) ||
				(editor = CHECK_COMMAND("vim")) ||
				(editor = CHECK_COMMAND("vi")) ||
				(editor = CHECK_COMMAND("ex")) ||
				(editor = CHECK_COMMAND("ed")))) {
			editor = "\0";
		}
		#undef CHECK_COMMAND

                if (!strcmp("\0", editor)) {
                        printf("Unable to find editor.\nSet the EDITOR environment variable.");
                        exit(-1);
                }

                buffer = malloc(strlen(editor) + strlen(fulllatest) + 2);
                strcpy(buffer, editor);
                strcat(buffer, " ");
                strcat(buffer, fulllatest);

                printf("Opening %s with %s...\n", latestpath, editor);
                system(buffer);

                free(buffer);
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
