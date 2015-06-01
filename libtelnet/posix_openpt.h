int posix_openpt(int oflag);
int grantpt(int fildes);
int unlockpt(int fildes);
char *ptsname(int fildes);
