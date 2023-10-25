#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <bits/getopt_core.h>

void getFileDetails(const char *path, int dflag) {
    struct stat st;

    if ((dflag && (stat(path, &st) == 0)) || (!dflag && (stat(path, &st) == 0) && S_ISREG(st.st_mode))) {
        printf(" [*]  %s | Size: %lld bytes ", path, (long long)st.st_size);
        printf("| Type: %s ", S_ISDIR(st.st_mode) ? "Directory" : "File");

        printf("| Permissions: ");
        printf((S_ISDIR(st.st_mode) ? "d" : "-"));
        printf((st.st_mode & S_IRUSR) ? "r" : "-");
        printf((st.st_mode & S_IWUSR) ? "w" : "-");
        printf((st.st_mode & S_IXUSR) ? "x" : "-");
        printf((st.st_mode & S_IRGRP) ? "r" : "-");
        printf((st.st_mode & S_IWGRP) ? "w" : "-");
        printf((st.st_mode & S_IXGRP) ? "x" : "-");
        printf((st.st_mode & S_IROTH) ? "r" : "-");
        printf((st.st_mode & S_IWOTH) ? "w" : "-");
        printf((st.st_mode & S_IXOTH) ? "x" : "-");

        struct passwd *pw = getpwuid(st.st_uid);
        struct group *gr = getgrgid(st.st_gid);
        if (pw && gr) {
            printf("| Owner: %s | Group: %s\n", pw->pw_name, gr->gr_name);
        } else {
            printf("| Owner: %d | Group: %d\n", st.st_uid, st.st_gid);
        }
    }
}

void iterateOverDir(const char *dirname, int dflag) {
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(dirname))) {
        perror("opendir");
        exit(1);
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", dirname, entry->d_name);

        getFileDetails(path, dflag);

        if (entry->d_type == DT_DIR) {
            iterateOverDir(path, dflag);
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "You should use: %s <directory> [-d]\n", argv[0]);
        return 1;
    }

    int c;
    int dflag = 0;
    opterr = 0;

    c = getopt(argc, argv, "d");
    if (c != -1 && c == 'd') {
        dflag = 1;
    }

    iterateOverDir(argv[1], dflag);

    return 0;
}
