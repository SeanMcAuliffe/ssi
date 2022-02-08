#include <unistd.h>
int main() {
    char* args[3];
    args[0]="ls";
    args[1]="-l";
    args[2]=NULL;//what if not
    execvp(args[0],args);
    return 0;
}
