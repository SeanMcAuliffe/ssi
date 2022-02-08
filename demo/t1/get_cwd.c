#include <unistd.h>
#include <stdio.h>
#include <limits.h>

#include <unistd.h>
int main()
{
    char buf[80];
    getcwd(buf, sizeof(buf));
    printf("current working directory : %s\n", buf);
}