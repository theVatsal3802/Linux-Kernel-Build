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
    int n, i, j, x, len, temp_len;
    while(1){
        // printf("Enter the set size: ");
        
        fgets(ip, 256, stdin);
        // 1234
        ip[strlen(ip)-1] = '\0';
        len = snprintf(buff, strlen(ip)+1, "%s", ip);
        // write to the set first. let it decide if it is valid
        x = write(fd, buff, len);
        // printf("x: %d, len: %d\n", x, len);
        if(x<len){
            printf("Invalid length entered\n");
            continue;
        }
        // convert n to integer now
        sscanf(buff, "%d", &n);
        break;
    }
    // do not close as it will write it again
    for(int i=0;i<n;i++){
        // printf("Enter set element: ");
        fgets(ip, 256, stdin);
        ip[strlen(ip)] = '\0';
        temp_len = snprintf(buff, strlen(ip), "%s", ip);
        len = temp_len;
        temp_len = write(fd, buff, len);
        // printf("%d %d\n", len, temp_len);
        if(temp_len < len){
            printf("Seems like %s already exists in the set or is invalid\n", buff);
            i--;
        }
        if(i>1 && i%2 == 0 && i!=n-1){
            // uncomment out line 47 to 52 to get read access error
            char read_buff[1024];
            // int fd = open("/proc/partb_24CS60R43_24CS60R26", O_RDWR);
            // if(fd == -1){
            //     printf("Somethin wrong with opening file!\n");
            //     return 0;
            // }
            len = read(fd, read_buff, 1024);
            read_buff[len-1] = '\0';
            puts(read_buff);
        }
    }
    char read_buff[1024];
    len = read(fd, read_buff, 1024);
    read_buff[len-1] = '\0';
    // printf("Final values in set: ");
    puts(read_buff);
    close(fd);
    return 0;
}	
// sudo rmmod set
// make clean
// make
// gcc user.c
// sudo insmod ./set.ko
// ./a.out