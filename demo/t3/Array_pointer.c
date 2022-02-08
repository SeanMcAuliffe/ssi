#include <stdio.h>

int main()
{
    char a[10] = "geek";
    printf("%lu \n", sizeof(a));
    a[0] = 'j';
    printf("%s \n", a);

    char *p = "geek";
    printf("%lu \n", sizeof(p));
 
    // Uncommenting below line would cause undefined behaviour
    // (Caused segmentation fault on gcc)
    // s2[0] = 'j'; 
    return 0;
}
