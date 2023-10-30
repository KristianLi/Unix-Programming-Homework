#include <fcntl.h>
#include "apue.h"
#include "sys/times.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define BUFFERSIZE 131072

int main(int argc, char** argv) {
    if (argc != 2 && argc != 3) {
        fprintf(stderr, "Usage: ./timewrite <outfile> [sync]\n");
        exit(-1);
    }

    int fd;
    if (argc == 2) {
        fd = open(argv[1], O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    } else {
        if (strcmp(argv[2], "sync") == 0) {
            fd = open(argv[1], O_WRONLY | O_CREAT | O_SYNC, S_IRUSR | S_IWUSR);
        } else {
            fprintf(stderr, "Usage: ./timewrite <outfile> [sync]\n");
            exit(-1);
        }
    }

    if (fd < 0) {
        fprintf(stderr, "Open Failed!\n");
        exit(-1);
    }

    char buf[BUFFERSIZE];
    long length = 0;
    if ((length = read(STDIN_FILENO, buf, BUFFERSIZE)) < 0) {
        fprintf(stderr, "file read error\n");
        exit(-1);
    }

    struct tms buffer;
    clock_t start, end;
    int ticks = sysconf(_SC_CLK_TCK);

    fprintf(stdout, "BUFFSIZE    用户CPU(s)    系统CPU(s)   时钟时间(s)  循环次数\n");
	int size;
    for (size = 256; size <= BUFFERSIZE; size *= 2) {
        if (lseek(fd, 0, SEEK_SET) == -1) {
            perror("lseek error");
            exit(-1);
        }

        start = times(&buffer);
        int RestOfLength = length;
        int loop = 0;
        while (RestOfLength > 0) {
            int Min = (RestOfLength > size) ? size : RestOfLength;
            RestOfLength -= Min;
            if (write(fd, buf + loop * size, Min) != Min) {
                perror("write error");
                exit(-1);
            }
            loop++;
        }
        end = times(&buffer);
        fprintf(stdout, "%8d     %.2lf          %.2lf         %.2lf %10d\n", size,
                (double)(buffer.tms_utime) / ticks, (double)(buffer.tms_stime) / ticks,
                (double)(end - start) / ticks, loop);
    }
    close(fd);
    exit(0);
}
