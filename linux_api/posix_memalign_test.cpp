/*
 * int posix_memalign(void **memptr, size_t alignment, size_t size);
 * 
 * The posix_memalign() function shall allocate size bytes aligned on a boundary specified by alignment, 
 * and shall return a pointer to the allocated memory in memptr. 
 * The value of alignment shall be a multiple of sizeof( void *), that is also a power of two.
 * Upon successful completion, the value pointed to by memptr shall be a multiple of alignment.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

static const int MALLOC_SIZE = 100;
int main()
{
    void *p = NULL;
    
    printf("before align: %p\n", p);

    // 13非2的幂，align会失败
    //if (posix_memalign(&p, 13, MALLOC_SIZE) != 0) {
    if (posix_memalign(&p, pow(2, 13), MALLOC_SIZE) != 0) {
        fprintf(stderr, "posix_memalign failed.\n");
        return -1;
    }

    printf("after align: %p\n", p);
    free(p);
    p = NULL;

    return 0;
}
