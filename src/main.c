#include <dirent.h>
#include <limits.h> //For PATH_MAX
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

void scanFile(char *path, char *target)
{
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        printf("Error reading path %s", path);
        return;
    }
    int line_count = 0;
    char line[65536];
    int target_length = strlen(target);
    while (fgets(line, 65536, fp)) {
        line_count++;

        char c;
        int match_index = 0;
        int is_matching = 0;
        int found_indexes[1024];
        int found_count = 0;
        int last_char_index = 0;
        for (int i = 0; (c = line[i]) != '\0'; last_char_index = i++) {
            if (c == target[match_index]) {
                is_matching = 1;
                if (match_index == target_length - 1) {
                    found_indexes[found_count] = i - target_length + 1;
                    found_count++;
                    match_index = 0;
                    is_matching = 0;
                }
                match_index++;
            } else {
                match_index = 0;
                is_matching = 0;
            }
        }
        if (found_count == 0) {
            goto line_break;
        }

        int rendered_found_count = 1;
        int found_index = found_indexes[0];
        printf("%s:%d:", path, line_count);
        for (int i = 0; (c = line[i]) != '\0'; i++) {
            if (i == found_index + target_length) {
                printf("\033[0m");
                if (found_count > rendered_found_count) {
                    found_index = found_indexes[rendered_found_count];
                    rendered_found_count++;
                }
            }
            if (i == found_index) {
                printf("\033[1;32m");
            }
            putchar(c);
        }
    line_break:
        continue;
    }
    fclose(fp);
}

int handlePath(char *path, char *keyword)
{
    struct stat path_stat;
    if (stat(path, &path_stat) != 0) {
        printf("Error reading path %s\n", path);
        return 1;
    }

    if (S_ISREG(path_stat.st_mode)) {
        scanFile(path, keyword);
    } else if (S_ISDIR(path_stat.st_mode)) {
        struct dirent *de;

        DIR *dr = opendir(path);

        if (dr == NULL) {
            printf("Could not open %s\n", path);
            return 1;
        }

        char buf[PATH_MAX + 1];
        while ((de = readdir(dr)) != NULL) {
            if (strcmp(de->d_name, "..") != 0 && strcmp(de->d_name, ".")) {
                strcpy(buf, path);
                strcat(buf, "/");
                strcat(buf, de->d_name);
                handlePath(buf, keyword);
            }
        }

        closedir(dr);
    } else
        printf("%s is not a file or a dir\n", path);

    return 0;
}

int main(int argc, char **argv)
{
    // if (!isatty(fileno(stdin))) {
    //     int i = 0;
    //     char pipe[65536];
    //     while (-1 != (pipe[i++] = getchar()))
    //         ;
    //     fprintf(stdout, "piped content: >>%s<<\n", pipe);
    // }
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <path> <keyword>\n", argv[0]);
        return 1;
    }

    char *path = argv[1];
    char *keyword = argv[2];

    return handlePath(path, keyword);
}
