#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<dirent.h>
#include<pthread.h>


struct FILE_P {
    char orig_path[PATH_MAX];
    char dest_path[PATH_MAX];
    struct FILE_P *next;
}; typedef struct FILE_P File_P;

struct FILE_I {
    char orig_path[PATH_MAX];
    char dest_path[PATH_MAX];
    struct FILE_I *next;
}; typedef struct FILE_I File_I;

struct FILE_M {
    char orig_path[PATH_MAX];
    char dest_path[PATH_MAX];
    struct FILE_M *next;
}; typedef struct FILE_M File_M;

struct FILE_J {
    char orig_path[PATH_MAX];
    char dest_path[PATH_MAX];
    struct FILE_J *next;
}; typedef struct FILE_J File_J;

struct FILES {
    int number_files_p;
    int number_files_i;
    int number_files_m;
    int number_files_j;
    File_P *file_p;
    File_I *file_i;
    File_M *file_m;
    File_J *file_j;
}; typedef struct FILES Files;


void pool_of_producers(char *path, char *d_path, Files *files){
    struct dirent **dir_arq_name_list;
    
    int n;
    n = scandir(path, &dir_arq_name_list, NULL, alphasort);
    if (n < 0)
        perror("scandir");
    else {
        int f = 1;
        while (n--) {
            if (f==4) f=1;
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

                    if (f == 1){
                        File_P *file_p = (File_P *)malloc(sizeof(File_P));
                        strcpy(file_p->orig_path, orig_path);
                        strcpy(file_p->dest_path, dest_path);
                        file_p->next = files->file_p;
                        files->file_p = file_p;
                        files->number_files_p++;
                    } else if (f == 2){
                        File_I *file_i = (File_I *)malloc(sizeof(File_I));
                        strcpy(file_i->orig_path, orig_path);
                        strcpy(file_i->dest_path, dest_path);
                        file_i->next = files->file_i;
                        files->file_i = file_i;
                        files->number_files_i++;
                    } else if (f == 3){
                        File_M *file_m = (File_M *)malloc(sizeof(File_M));
                        strcpy(file_m->orig_path, orig_path);
                        strcpy(file_m->dest_path, dest_path);
                        file_m->next = files->file_m;
                        files->file_m = file_m;
                        files->number_files_m++;
                    } else if (f == 4){
                        File_J *file_j = (File_J *)malloc(sizeof(File_J));
                        strcpy(file_j->orig_path, orig_path);
                        strcpy(file_j->dest_path, dest_path);
                        file_j->next = files->file_j;
                        files->file_j = file_j;
                        files->number_files_j++;
                    }
                    

                }
            }
            free(dir_arq_name_list[n]);
            f++;
        }
        free(dir_arq_name_list);
    }
}

void *pool_of_consumers_p(void *f){
    Files *files = (Files *)f;
    while(files->file_p!=NULL && files->number_files_p!=0){
        File_P *file_p;
        file_p = files->file_p;

        char command[PATH_MAX+10];
        strcpy(command, "bzip2 -c ");
        strcat(command, file_p->orig_path);
        strcat(command, " > ");
        strcat(command, file_p->dest_path);
        popen(command, "r");
        wait(NULL);

        files->file_p = files->file_p->next; 
        files->number_files_p--;
    }
    pthread_exit(0);
}

void *pool_of_consumers_i(void *f){
    Files *files = (Files *)f;
    while(files->file_i!=NULL && files->number_files_i!=0){
        File_I *file_i;
        file_i = files->file_i;

        char command[PATH_MAX+10];
        strcpy(command, "bzip2 -c ");
        strcat(command, file_i->orig_path);
        strcat(command, " > ");
        strcat(command, file_i->dest_path);
        popen(command, "r");
        wait(NULL);

        files->file_i = files->file_i->next; 
        files->number_files_i--;
    }
    pthread_exit(0);
}

void *pool_of_consumers_m(void *f){
    Files *files = (Files *)f;
    while(files->file_m!=NULL && files->number_files_m!=0){
        File_M *file_m;
        file_m = files->file_m;

        char command[PATH_MAX+10];
        strcpy(command, "bzip2 -c ");
        strcat(command, file_m->orig_path);
        strcat(command, " > ");
        strcat(command, file_m->dest_path);
        popen(command, "r");
        wait(NULL);

        files->file_m = files->file_m->next; 
        files->number_files_m--;
    }
    pthread_exit(0);
}

void *pool_of_consumers_j(void *f){
    Files *files = (Files *)f;
    while(files->file_j!=NULL && files->number_files_j!=0){
        File_J *file_j;
        file_j = files->file_j;

        char command[PATH_MAX+10];
        strcpy(command, "bzip2 -c ");
        strcat(command, file_j->orig_path);
        strcat(command, " > ");
        strcat(command, file_j->dest_path);
        popen(command, "r");
        wait(NULL);

        files->file_j = files->file_j->next; 
        files->number_files_j--;
    }
    pthread_exit(0);
}


int main(int argc, char *argv[]){
    
    pthread_t thread_1, thread_2, thread_3, thread_4;

    char orig_dir[PATH_MAX], temp_dest_dir[PATH_MAX], dest_dir[PATH_MAX], *path_part;
    strcpy(orig_dir, argv[1]);
    strcpy(dest_dir, argv[2]);

    Files *files = (Files *)malloc(sizeof(Files));
    files->file_p = NULL;
    files->file_i = NULL;
    files->number_files_p = 0;
    files->number_files_i = 0;
    files->number_files_p = 0;

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

    pthread_create(&thread_1, NULL, pool_of_consumers_p, files);
    pthread_create(&thread_2, NULL, pool_of_consumers_i, files);
    pthread_create(&thread_3, NULL, pool_of_consumers_m, files);
    pthread_create(&thread_4, NULL, pool_of_consumers_j, files);

    pthread_join(thread_1, NULL);
    pthread_join(thread_2, NULL);
    pthread_join(thread_3, NULL);
    pthread_join(thread_4, NULL);

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
