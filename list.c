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

/* take a path to a directory, a dirent array, and an unsigned int, and populate
 * the array with dirents of each file within the directory, out_num is set to
 * the total number of files */
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
        /* unsigned i; */
        char *newestfp;
        int c, nflag, eflag, oflag;
        
        newestfp = (char *) malloc(100);

/*----------------------------------------------------------------------------*/

        nflag = eflag = oflag = 0;
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

/*----------------------------------------------------------------------------*/

        /* create a new file with a name based on the date and time */
        if (nflag) {
                time_t nft;
                struct tm *lt;
                char *buffer, *newfp;
                size_t full_len;
                FILE *touch;

                newfp = (char *) malloc(100);

                time(&nft);
                lt = localtime(&nft);

                full_len = strlen(PATH) + strlen(NAME);
                buffer = malloc(full_len);

                strftime(newfp, strlen(NAME) + 1, "%F_%T-sl.txt", lt);
                printf("Creating new file %s...\n", newfp);

                strcpy(buffer, PATH);
                strcat(buffer, "/");
                strcat(buffer, newfp);

                touch = fopen(buffer, "a+");
                fclose(touch);
                free(buffer);
                printf("\n");
        }

/*----------------------------------------------------------------------------*/

        /* determine the most recent file and save the path */
        {
                time_t t;
                char *buffer, *lastpath;
                struct dirent *ent;
                struct stat attr;
                unsigned count;

                buffer = lastpath = (char *) malloc(100);

                /* fill a dirent array with every file in the directory */
                parsedir(PATH, &ent, &count);
        
                t = 0;
                for(unsigned i = 0; i < count; i++) {
                        /* ignore . and .. */
                        if (strcmp(ent[i].d_name, ".") == 0 ||
                                        strcmp(ent[i].d_name, "..") == 0)
                                continue;

                        strcpy(buffer, PATH);
                        strcat(buffer, "/");
                        strcat(buffer, ent[i].d_name);

                        stat(buffer, &attr);

                        /* check if the current file's epoch time is larger than
                         * the largerst previous one. */
                        if (attr.st_mtime > t) {
                                /* store the new largest epoch time, and the
                                 * filename */
                                t = attr.st_mtime;
                                lastpath = ent[i].d_name;
                        }
                }
                free(ent);

                /* save the final path for later */
                strcpy(newestfp, PATH);
                strcat(newestfp, "/");
                strcat(newestfp, lastpath);
        }

/*----------------------------------------------------------------------------*/

        /* simply print the file to stdout, and exit */
        if (eflag) {
                char s;
                FILE *fp;
                fp=fopen(newestfp,"r");
                while((s=fgetc(fp))!=EOF) {
                        printf("%c",s);
                }
                fclose(fp);
                exit(0);
        }

/*----------------------------------------------------------------------------*/

        /* open file with EDITOR and exit */
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

                buffer = malloc(strlen(editor) + strlen(newestfp) + 2);
                strcpy(buffer, editor);
                strcat(buffer, " ");
                strcat(buffer, newestfp);

                printf("Opening %s with %s...\n", newestfp, editor);
                system(buffer);

                free(buffer);
                exit(0);
        }

/*----------------------------------------------------------------------------*/

        /* add items to file */
        {
                FILE *fp;
                fp = fopen(newestfp, "a+");
                for (int i = nflag ? 2 : 1; i < argc; i++) { 
                        printf("appending \"%s\" to: %s\n", argv[i], newestfp);
                        fprintf(fp, "%s\n", argv[i]);
                }
                fclose(fp);
                exit(0);
        }
}
