#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(void)
{
    char buff[256], ip[256];
    int fd = open("/proc/partb_24CS60R43_24CS60R26", O_RDWR);
    if(fd == -1){
        printf("Somethin wrong with opening file!\n");
        return 0;
    }
    int n, i, j;
    printf("Enter the set size: ");
    
    fgets(ip, 256, stdin);
    // printf("ip: %s\n", ip);
    ip[strlen(ip)] = '\0';
    // printf("ip: %s\n", ip);
    int len = snprintf(buff, strlen(ip)+2, "%s", ip), temp_len;
    // write to the set first. let it decide if it is valid
    write(fd, buff, len);
    // convert n to integer now
    sscanf(buff, "%d", &n);
    // printf("n: %d\n", n);
    // do not close as it will write it again
    for(int i=0;i<n;i++){
        printf("Enter set element: ");
        fgets(ip, 256, stdin);
        ip[strlen(ip)] = '\0';
        temp_len = snprintf(buff, strlen(ip), "%s", ip);
        // sscanf(buff, "%d", &j);
        // printf("%s\n", buff);
        // break;
        len = temp_len;
        temp_len = write(fd, buff, len);
        if(temp_len < len){
            printf("Seems like %s already exists in the set or is invalid\n", buff);
            i--;
        }
        // printf("len: %d, temp_len: %d\n", len, temp_len);
    }
    close(fd);
    return 0;
}	
// sudo rmmod set
// make clean
// make
// gcc user.c
// sudo insmod ./set.ko
// ./a.out