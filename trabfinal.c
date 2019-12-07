#include<stdio.h>
#include<sys/stat.h>
#include<dirent.h>
#include<string.h>
#include<stdlib.h>
#include<bzlib.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>


struct FILE {
    char orig_path[PATH_MAX];
    char dest_path[PATH_MAX];
    struct FILE *next;
}; typedef struct FILE File;

struct FILES {
    int number_files;
    File *file;
}; typedef struct FILES Files;


void pool_of_producers(char *path, char *d_path, Files *files){
    struct dirent **dir_arq_name_list;
    
    int n;
    n = scandir(path, &dir_arq_name_list, NULL, alphasort);
    if (n < 0)
        perror("scandir");
    else {
        while (n--) {
            if (strcmp(dir_arq_name_list[n]->d_name, ".")!=0 && strcmp(dir_arq_name_list[n]->d_name, "..")!=0){
                if (dir_arq_name_list[n]->d_type == DT_DIR){
                    
                    char subdir_path[PATH_MAX], subdir_d_path[PATH_MAX];
                    strcpy(subdir_path, path);
                    strcat(subdir_path, "/");
                    strcat(subdir_path, dir_arq_name_list[n]->d_name);

                    strcpy(subdir_d_path, d_path);
                    strcat(subdir_d_path, "/");
                    strcat(subdir_d_path, dir_arq_name_list[n]->d_name);
                    mkdir(subdir_d_path, 0700);

                    pool_of_producers(subdir_path, subdir_d_path, files);
                } else {
                    char orig_path[PATH_MAX], dest_path[PATH_MAX];

                    strcpy(orig_path, path);
                    strcat(orig_path, "/");
                    strcat(orig_path, dir_arq_name_list[n]->d_name);

                    strcpy(dest_path, d_path);
                    strcat(dest_path, "/");
                    strcat(dest_path, dir_arq_name_list[n]->d_name);
                    strcat(dest_path, ".bz2");

                    File *file = (File *)malloc(sizeof(File));
                    strcpy(file->orig_path, orig_path);
                    strcpy(file->dest_path, dest_path);
                    file->next = files->file;
                    files->file = file;
                    files->number_files++;
                }
            }
            free(dir_arq_name_list[n]);
        }
        free(dir_arq_name_list);
    }
}

void pool_of_consumers(Files *files){
    while(files->file!=NULL && files->number_files!=0){
        File *file;
        file = files->file;

        char command[PATH_MAX+10];
        strcpy(command, "bzip2 -c ");
        strcat(command, file->orig_path);
        strcat(command, " > ");
        strcat(command, file->dest_path);
        popen(command, "r");
        wait(NULL);

        files->file = files->file->next;
        files->number_files--;
    }
}

int main(int argc, char *argv[]){
    
    char orig_dir[PATH_MAX], temp_dest_dir[PATH_MAX], dest_dir[PATH_MAX], *path_part;
    strcpy(orig_dir, argv[1]);
    strcpy(dest_dir, argv[2]);

    Files *files = (Files *)malloc(sizeof(Files));
    files->number_files = 0;
    files->file = NULL;

    char *temp = strdup(orig_dir);
    strcpy(temp_dest_dir, "");
    while (path_part = strsep(&temp, "/")){
        if (strcmp(path_part, ".")==0){
            strcat(temp_dest_dir, "./");
        } else if (strcmp(path_part, "")!=0){
            strcat(temp_dest_dir, path_part);
            strcat(temp_dest_dir, ".bz2");
            strcat(temp_dest_dir, "/");
        }
    }
    mkdir(temp_dest_dir, 0700);

    pool_of_producers(orig_dir, temp_dest_dir, files);
    pool_of_consumers(files);

    char command[2*PATH_MAX + 30];
    strcpy(command, "tar -cf ");
    
    strcat(dest_dir, " ");
    strcat(command, dest_dir);
    strcat(command, temp_dest_dir);
    strcat(command, " --remove-files");
    popen(command, "r");
    wait(NULL);
    
    return 0;
}
