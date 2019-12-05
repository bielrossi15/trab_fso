#include <stdio.h>
#include <stdlib.h>
#include <bzlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <string.h>


typedef struct {
  int infd;
  int outfd;
  char filename[PATH_MAX];
} compress;

void files(char * fpath, int indent) {
    DIR * dir;
    struct dirent * entry_dir;

    dir = opendir(fpath);

    if(dir == NULL) {
        printf("Diretorio inexistente");
        return;
    }

    if(dir) {
        while((entry_dir = readdir(dir)) != NULL) {
             if(entry_dir->d_type == DT_DIR) {
                char path[PATH_MAX];
                if (strcmp(entry_dir->d_name, ".") == 0 || strcmp(entry_dir->d_name, "..") == 0)
                    continue;
                strcpy(path, fpath);
                strcat(path, "/");
                strcat(path, entry_dir->d_name);
                // snprintf(path, sizeof(path), "%s/%s", fpath, entry_dir->d_name);
                // printf("%*s[%s]\n", indent, "", entry_dir->d_name);
                printf("%*s[%s]\n", indent, "", entry_dir->d_name);
                files(path, indent + 2);
             }
            else {
                printf("%*s%s\n", indent + 1, "", entry_dir->d_name);
            }
        }

        closedir(dir);
    }

}

int main() {
    DIR *dir;
    char path[PATH_MAX];
    scanf("%s", path);
    files(path, 1);

    return 0;
}