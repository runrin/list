#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <pwd.h>

/* XXX set a path to the directory where you will store your lists XXX */
#define PATH "/home/runrin/docs/shop"
#define NAME "XXXX-XX-XX_XX:XX:XX.txt"

/*============================================================================*/

/* take a path to a directory, a dirent array, and an unsigned int, and populate
 * the array with dirents of each file within the directory, out_num is set to
 * the total number of files */
int parse_dir(char *path, struct dirent **out, unsigned *out_num)
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

/*============================================================================*/

static void show_help()
{
        puts("Usage: list [OPTION] item1 item2 ...");
        puts("  -n\tcreate new list and append items");
        puts("  -e\techo current list to stdout");
        puts("  -o\topen current list in EDITOR");
        puts("  -v\tdisplay version information");
        puts("  -h\tdisplay this help and exit");
}

/*============================================================================*/

static void show_version()
{
        puts("list version 0.1.0"); 
}

/*============================================================================*/

int main(int argc, char*argv[])
{
        char *recent_fp;
        int c, nflag, eflag, oflag;
        size_t full_len;

        /* 1 for NULL terminator, 1 for the / between the path and filename */
        full_len = strlen(PATH) + strlen(NAME) + 2;
        
        recent_fp = malloc(full_len);

/*============================================================================*/

        nflag = eflag = oflag = 0;
        while ((c = getopt (argc, argv, "n:ehov")) != -1)
                switch (c)
                {
                        case 'n': nflag = 1; break;
                        case 'e': eflag = 1; break;
                        case 'o': oflag = 1; break;
                        case 'h': show_help(); exit(0);
                        case 'v': show_version(); exit(0);
                        default: break;
                        return 1;
                }

/*============================================================================*/

        /* create a new file with a name based on the date and time */
        if (nflag) {
                time_t nft;
                struct tm *lt;
                char *buffer, *new_fp;
                FILE *touch;

                new_fp = malloc(100);

                time(&nft);
                lt = localtime(&nft);

                buffer = malloc(full_len);

                strftime(new_fp, strlen(NAME) + 1, "%F_%T.txt", lt);
                printf("Creating new file %s...\n", new_fp);

                strcpy(buffer, PATH);
                strcat(buffer, "/");
                strcat(buffer, new_fp);

		if((touch = fopen(buffer, "a+")) != NULL) {
			fclose(touch);
		}
		else {
			fprintf(stderr, "Could not open file '%s'\n", buffer);
			return EXIT_FAILURE;
		}
                free(buffer);
                free(new_fp);
        }

/*============================================================================*/

        /* determine the most recent file and save the path */
        {
                time_t t;
                char *buffer, *last_path;
                struct dirent *ent;
                struct stat attr;
                unsigned count;

                /* last path will contain the path to the most recent file in
                 * our directory */
                buffer = malloc(100);

                /* fill a dirent array with every file in the directory */
                parse_dir(PATH, &ent, &count);
        
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
                                last_path = ent[i].d_name;
                        }
                }

                /* save the final path for later */
                strcpy(recent_fp, PATH);
                strcat(recent_fp, "/");
                strcat(recent_fp, last_path);

                free(buffer);
                /* last_path is freed with ent. they point to the same thing*/
                free(ent);
        }

/*============================================================================*/

        /* simply print the file to stdout, and exit */
        if (eflag) {
                char s;
                FILE *fp;
                fp=fopen(recent_fp,"r");
                while((s=fgetc(fp))!=EOF) {
                        printf("%c",s);
                }
                fclose(fp);
                exit(0);
        }

/*============================================================================*/

        /* open file with EDITOR and exit */
        if (oflag) {
		#define CHECK_COMMAND(X) ((system("which " X " 2> /dev/null 1> /dev/null") == 0) ? ("" X) : NULL)
                const char *editor;
                char *buffer;
                int out;
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
                        puts("Unable to find editor.");
                        puts("Set the EDITOR environment variable.");
                        exit(-1);
                }

                buffer = malloc(strlen(editor) + strlen(recent_fp) + 2);
                strcpy(buffer, editor);
                strcat(buffer, " ");
                strcat(buffer, recent_fp);

                printf("Opening %s with %s...\n", recent_fp, editor);
                out = system(buffer);

                free(buffer);
                exit(out);
        }

/*============================================================================*/

        /* add items to file */
        {
                FILE *fp;
		if((fp = fopen(recent_fp, "a+")) != NULL) {
			for (int i = nflag ? 2 : 1; i < argc; i++) { 
				printf("appending \"%s\" to: %s\n", argv[i], recent_fp);
				fprintf(fp, "%s\n", argv[i]);
			}
			fclose(fp);
		}
		else {
			fprintf(stderr, "Could not open file '%s'\n", recent_fp);
			return EXIT_FAILURE;
		}
                free(recent_fp);
                exit(0);
        }
}
