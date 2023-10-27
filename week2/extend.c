#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/unistd.h>
#include <unistd.h>

char *path_alloc(size_t *pathlen) {
    char *ptr;

    // 获取系统指定的路径名最大长度
    if ((*pathlen = pathconf("/", _PC_PATH_MAX)) < 0) {
        // 如果无法获取，使用一个合适的默认值
        if ((*pathlen = 1024) < 0) {
            // 如果默认值也不合适，返回NULL，并设置errno为ENOMEM
            errno = ENOMEM;
            return NULL;
        }
    } else {
        // 如果能获取到，加上路径名结束符的长度
        (*pathlen)++;
    }

    // 分配内存
    if ((ptr = malloc(*pathlen)) == NULL) {
        // 内存分配失败，返回NULL
        errno = ENOMEM;
        return NULL;
    }

    // 返回分配的内存指针
    return ptr;
}
