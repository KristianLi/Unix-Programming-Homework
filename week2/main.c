#include "apue.h"
#include "dirent.h"
#include "limits.h"

typedef int Myfunc(const char *,const struct stat *,int);

static Myfunc myfunc,myfunc2;
static int myftw(char *,Myfunc *),myftw2(char*,char*,Myfunc *),myftw3(char*,char*,Myfunc*);
static int dopath(Myfunc *),dopath2(Myfunc *),dopath3(char*,char*,Myfunc*);
int fun1(int argc,char** argv),fun2(int argc,char **argv),fun3(int argc,char **argv);
static long nreg,ndir,nblk,nchr,nfifo,nslink,nsock,ntot,nsmall;



int compare_files(const char *file1_path, const char *file2_path) {
    FILE *file1 = fopen(file1_path, "r");
    FILE *file2 = fopen(file2_path, "r");

    if (!file1 || !file2) {
        return -1;
    }

    // 比较文件大小
    fseek(file1, 0, SEEK_END);
    fseek(file2, 0, SEEK_END);
    long size1 = ftell(file1);
    long size2 = ftell(file2);
    rewind(file1);
    rewind(file2);

    if (size1 != size2) {
        fclose(file1);
        fclose(file2);
        return 0; // 文件大小不同，不相同
    }

    // 比较文件内容
    char *line1 = NULL;
    char *line2 = NULL;
    size_t len = 0;
    int isDifferent = 0;

    while (getline(&line1, &len, file1) != -1) {
        if (getline(&line2, &len, file2) == -1 || strcmp(line1, line2) != 0) {
            isDifferent = 1;
            break;
        }
    }

    free(line1);
    free(line2);
    fclose(file1);
    fclose(file2);

    if (isDifferent) {
        return 0; // 文件内容不同，不相同
    } else {
        return 1; // 文件大小和内容相同，相同
    }
}

int compare_names(char *s1,char *s2){
    int flag=0;
    int i,j=0;
    int n1= strlen(s1),n2= strlen(s2);
    while(!flag){
        if(j>=n2)break;
        i=0;
        while(s1[i]==s2[j]){
            i++;
            j++;
        }
        if(i<n1) {
            while (s2[j] != ' ' && j < n2)j++;
            j++;
        }
        else if(i>=n1 && (s2[j]==' '|| s2[j]=='\0'))
            flag=1;
        else break;
    }
    return flag;
}

int main(int argc, char** argv){
    if(argc==2)
        exit(fun1(argc,argv));
    if(argc==4) {
        if (strcmp(argv[2], "-comp") == 0)
            exit(fun2(argc, argv));
        else if(strcmp(argv[2],"-name")==0)
            exit(fun3(argc, argv));
    }
    else if(argc>4)
        if(strcmp(argv[2],"-name")==0)
            exit(fun3(argc, argv));
    exit(-1);
}

#define FTW_F 1
#define FTW_D 2
#define FTW_DNR 3
#define FTW_NS 4
#define FTW_R 5

static char *fullpath,*fullpath2;
static size_t pathlen,pathlen2;

static int myftw(char *pathname,Myfunc *func){
    fullpath= path_alloc(&pathlen);
    if(pathlen<= strlen(pathname)){
        pathlen= strlen(pathname)*2;
        if((fullpath= realloc(fullpath,pathlen))==NULL)
            err_sys("");
    }
    strcpy(fullpath,pathname);
    realpath(pathname,fullpath);
    return dopath(func);
}

static int dopath(Myfunc *func){
    struct stat statbuf;
    struct dirent *dirp;
    DIR *dp;
    int ret,n;
    if(lstat(fullpath,&statbuf)<0)
        return func(fullpath,&statbuf,FTW_NS);
    if(S_ISDIR(statbuf.st_mode)==0)
        return func(fullpath,&statbuf,FTW_F);
    if((ret=func(fullpath,&statbuf,FTW_D))!=0)
        return ret;
    n= strlen(fullpath);
    if(n+NAME_MAX+2 > pathlen){
        pathlen*=2;
        if((fullpath= realloc(fullpath,pathlen))==NULL)
            err_sys("");
    }
    fullpath[n++]='/';
    fullpath[n]=0;
    if((dp= opendir(fullpath))==NULL)
        return func(fullpath,&statbuf,FTW_DNR);
    while((dirp= readdir(dp))!=NULL){
        if(strcmp(dirp->d_name,".")==0 || strcmp(dirp->d_name,"..")==0)
            continue;
        strcpy(&fullpath[n],dirp->d_name);
        if((ret= dopath(func))!=0)
            break;
    }
    fullpath[n-1]=0;
    closedir(dp);
    return ret;
}

static int myfunc(const char *pathname,const struct stat *statptr,int type){
    switch (type) {
        case FTW_F:
            switch (statptr->st_mode & S_IFMT) {
                case S_IFREG:nreg++;
                    if (statptr->st_size <= 4096)
                        nsmall++;
                    break;
                case S_IFBLK:nblk++;
                    break;
                case S_IFCHR:nchr++;
                    break;
                case S_IFIFO:nfifo++;
                    break;
                case S_IFLNK:nslink++;
                    break;
                case S_IFSOCK:nsock++;
                    break;
                case S_IFDIR:
                    err_dump("");
            }
            break;
        case FTW_D:ndir++;
            break;
        case FTW_DNR:
            err_ret("");
            break;
        case FTW_NS:
            err_ret("");
            break;
        default:
            err_dump("");
    }
    return 0;
}

static int myftw2(char *pathname,char *pathname2,Myfunc *func){
    fullpath= path_alloc(&pathlen);
    if(pathlen<= strlen(pathname)){
        pathlen= strlen(pathname)*2;
        if((fullpath= realloc(fullpath,pathlen))==NULL)
            err_sys("");
    }
    strcpy(fullpath,pathname);
    realpath(pathname,fullpath);
    fullpath2= path_alloc(&pathlen2);
    if(pathlen2<= strlen(pathname2)){
        pathlen2= strlen(pathname2)*2;
        if((fullpath2= realloc(fullpath2,pathlen2))==NULL)
            err_sys("");
    }
    strcpy(fullpath2,pathname2);
    realpath(pathname2,fullpath2);
    struct stat statbuf;
    DIR *dp;
    if(lstat(fullpath2,&statbuf)<0)
        err_quit("");
    if(S_ISREG(statbuf.st_mode)==0)
        err_sys("<filename> must be regular!");
    return dopath2(func);
}

static int dopath2(Myfunc *func){
    struct stat statbuf;
    struct dirent *dirp;
    DIR *dp;
    int ret=0,n;
    if(lstat(fullpath,&statbuf)<0)
        return func(fullpath,&statbuf,FTW_NS);
    if(S_ISREG(statbuf.st_mode))
        if(compare_files(fullpath,fullpath2)){
            fprintf(stderr, "%s\n", fullpath);
            return ret;
        }
    if(S_ISDIR(statbuf.st_mode)) {
        n = strlen(fullpath);
        if (n + NAME_MAX + 2 > pathlen) {
            pathlen *= 2;
            if ((fullpath = realloc(fullpath, pathlen)) == NULL)
                err_sys("");
        }
        fullpath[n++] = '/';
        fullpath[n] = 0;
        if ((dp = opendir(fullpath)) == NULL)
            return func(fullpath, &statbuf, FTW_DNR);
        while ((dirp = readdir(dp)) != NULL) {
            if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)
                continue;
            strcpy(&fullpath[n], dirp->d_name);
            if ((ret = dopath2(func)) != 0)
                break;
        }
        fullpath[n - 1] = 0;
        closedir(dp);
    }
    return ret;
}

static int myfunc2(const char *pathname,const struct stat *statptr,int type){
    switch (type) {
        case FTW_DNR:
            err_ret("");
            break;
        case FTW_NS:
            err_ret("");
            break;
        default:
            break;
    }
    return 0;
}

static int myftw3(char *pathname,char *files,Myfunc *func){
    fullpath= path_alloc(&pathlen);
    if(pathlen<= strlen(pathname)){
        pathlen= strlen(pathname)*2;
        if((fullpath= realloc(fullpath,pathlen))==NULL)
            err_sys("");
    }
    strcpy(fullpath,pathname);
    realpath(pathname,fullpath);
    return dopath3(pathname,files,func);
}

static int dopath3(char *pathname,char *files,Myfunc* func){
    struct stat statbuf;
    struct dirent *dirp;
    DIR *dp;
    int ret=0,n;
    if(lstat(fullpath,&statbuf)<0)
        return func(fullpath,&statbuf,FTW_NS);
    if(S_ISDIR(statbuf.st_mode)) {
        n = strlen(fullpath);
        if (n + NAME_MAX + 2 > pathlen) {
            pathlen *= 2;
            if ((fullpath = realloc(fullpath, pathlen)) == NULL)
                err_sys("");
        }
        fullpath[n++] = '/';
        fullpath[n] = 0;
        if ((dp = opendir(fullpath)) == NULL)
            return func(fullpath, &statbuf, FTW_DNR);
        while ((dirp = readdir(dp)) != NULL) {
            if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)
                continue;
            strcpy(&fullpath[n], dirp->d_name);
            if ((ret = dopath3(pathname,files,func)) != 0)
                break;
        }
        fullpath[n - 1] = 0;
        closedir(dp);
    }
    else {
        n= strlen(fullpath);
        char *p=&fullpath[n-1];
        while(*p!='/')p--;
        p++;
        if(compare_names(p,files))
            fprintf(stderr, "%s\n", fullpath);
    }
    return ret;
}

int fun1(int argc,char** argv){
    int ret;
    ret= myftw(argv[1],myfunc);
    ntot=nreg+ndir+nblk+nchr+nfifo+nslink+nsock;
    if(ntot == 0)ntot=1;
    printf("regular files  =%7ld, %5.2F %%\n",nreg,nreg*100.0/ntot);
    printf("directories    =%7ld, %5.2F %%\n",ndir,ndir*100.0/ntot);
    printf("block special  =%7ld, %5.2F %%\n",nblk,nblk*100.0/ntot);
    printf("char special   =%7ld, %5.2F %%\n",nchr,nchr*100.0/ntot);
    printf("FIFOs          =%7ld, %5.2F %%\n",nfifo,nfifo*100.0/ntot);
    printf("symbolic links =%7ld, %5.2F %%\n",nslink,nslink*100.0/ntot);
    printf("sockets        =%7ld, %5.2F %%\n",nsock,nsock*100.0/ntot);
    printf("<=4096 files   =%7ld, %5.2F %%\n",nsmall,nsmall*100.0/ntot);
    return ret;
}

int fun2(int argc,char **argv){
    int ret;
    ret=myftw2(argv[1],argv[3],myfunc2);
    return ret;
}

int fun3(int argc,char **argv){
    int ret;
    int n=0;
    int i=3;
    for (; i < argc; ++i) {
        n+= strlen(argv[i]);
    }
    char str[n+1];
    char *p=str;
    for (i = 3; i < argc; ++i) {
        strcpy(p,argv[i]);
        p+= strlen(argv[i]);
        *p=' ';
        p++;
    }
    ret=myftw3(argv[1],str,myfunc2);
    return ret;
}
