#include <stdio.h>
#include <stdlib.h>
#include <bzlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <string.h>

char input_buffer[65536], output_buffer[65536];

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
                char path[PATH_MAX];
                if (strcmp(entry_dir->d_name, ".") == 0 || strcmp(entry_dir->d_name, "..") == 0)
                    continue;
                strcpy(path, fpath);
                strcat(path, "/");
                strcat(path, entry_dir->d_name);
                printf("%*s[%s]\n", indent, "", entry_dir->d_name);
                files(path, indent + 2);
             }
            else {
                char path[PATH_MAX];
                strcpy(path, fpath);
                strcat(path, "/");
                strcat(path, entry_dir->d_name);           
                printf("%*s%s\n", indent + 1, "", path);

                FILE *input, *output;
                input = fopen(path, "rb");
                output = fopen(strcat(path, ".bz2"), "wb");

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

int main() {
    DIR *dir;

    char path[PATH_MAX];
    scanf("%s", path);
    files(path, 1);

    return 0;
}