#include<stdio.h>
#include<sys/stat.h>
#include<dirent.h>
#include<string.h>
#include<stdlib.h>
#include<bzlib.h>


char input_buffer[PATH_MAX], output_buffer[PATH_MAX];

typedef struct {
  int infd;
  int outfd;
  char filename[PATH_MAX];
} compress;

void start_compression(bz_stream *bs){
    bs->bzalloc = NULL;
    bs->bzfree = NULL;
    bs->opaque = NULL;

    BZ2_bzCompressInit(bs, 9, 0, 0);
}

void end_compression(bz_stream *bs){
    BZ2_bzCompressEnd(bs);
}

void files(char *fpath, int indent) {
    DIR *dir;
    struct dirent *entry_dir;

    dir = opendir(fpath);

    if(dir == NULL) {
        printf("Diretorio inexistente");
        return;
    }

    if(dir) {
        while((entry_dir = readdir(dir)) != NULL) {
            if(entry_dir->d_type == DT_DIR) {
                char path[PATH_MAX], dest_subdir[PATH_MAX], *path_part;

                if (strcmp(entry_dir->d_name, ".") == 0 || strcmp(entry_dir->d_name, "..") == 0)
                    continue;
                
                strcpy(path, fpath);
                strcat(path, "/");
                strcat(path, entry_dir->d_name);
                printf("%*s[%s]\n", indent, "", entry_dir->d_name);

                char *temp = strdup(path);
                strcpy(dest_subdir, "");
                while (path_part = strsep(&temp, "/")){
                    strcat(dest_subdir, path_part);
                    strcat(dest_subdir, ".bz2");
                    strcat(dest_subdir, "/");
                }

                free(temp);
                mkdir(dest_subdir, 0700);

                files(path, indent + 2);
            }
            else {
                char path[PATH_MAX], dest_subdir[PATH_MAX], *path_part;

                strcpy(path, fpath);
                strcat(path, "/");
                strcat(path, entry_dir->d_name);           
                printf("%*s%s\n", indent + 1, "", path);
                
                char *temp = strdup(path);
                strcpy(dest_subdir, ".");
                while (path_part = strsep(&temp, "/")){
                    strcat(dest_subdir, "/");
                    strcat(dest_subdir, path_part);
                    strcat(dest_subdir, ".bz2");
                }
                printf("ad %s\n", dest_subdir);

                FILE *input, *output;
                input = fopen(path, "rb");
                output = fopen(dest_subdir, "wb");

                bz_stream bs;
                start_compression(&bs);

                int action = BZ_RUN;
                int f = BZ_OK;

                do {
                    bs.avail_in = fread(input_buffer, 1, sizeof(input_buffer), input);
                    if(feof(input) != 0){
                        action = BZ_FINISH;
                    }
                    bs.next_in = input_buffer;
                    do {
                        bs.next_out = output_buffer;
                        bs.avail_out = sizeof(output_buffer);
                        f = BZ2_bzCompress(&bs, action);
                        fwrite(output_buffer, sizeof(output_buffer) - bs.avail_out, 1, output);
                    } while ((bs.avail_out == 0) && (f != BZ_STREAM_END));
                } while (action != BZ_FINISH);

                end_compression(&bs);
            }
        }

        closedir(dir);
    }

}

int main(int argc, char *argv[]) {

    printf("Número de Argumentos: %d\n", argc);
    printf("Origem: %s\n", argv[1]);
    printf("Destino: %s\n", argv[2]);

    char dest_dir[PATH_MAX];
    strcpy(dest_dir, argv[1]);
    strcat(dest_dir, ".bz2");
    mkdir(dest_dir, 0700);

    files(argv[1], 1);

    // char comand[1000];
    // strcpy(comand, "tar -cf ");

    // char new_path[PATH_MAX + 8];
    // strcpy(new_path, path);
    // strcat(new_path, ".bz2.tar ");
    
    // strcat(comand, new_path);
    // strcat(comand, path);

    // printf("%s", comand);
    // popen(comand, "r");
    return 0;
}