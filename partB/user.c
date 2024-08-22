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
        // uncomment when running ./a.out on terminal to get this input prompt
        // printf("Enter set element: ");
        fgets(ip, 256, stdin);
        ip[strlen(ip)] = '\0';
        temp_len = snprintf(buff, strlen(ip), "%s", ip);
        len = temp_len;
        temp_len = write(fd, buff, len);
        if(temp_len < len){
            printf("Seems like %s already exists in the set or is invalid\n", buff);
            i--;
        }
        if(i>1 && i%2 == 0 && i!=n-1){
            // uncomment out line 52 to 56 to get read access error since we
            // will be opening the file even though we alread have it opened
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
    /* 
        assuming that we have are inserting multiples of 1e9, we will have
        maximum 100*9 digits and 100 spaces. Hence 1024 should suffice (not
        considering -ve nos). Should this not be enough, change it to an 
        appropriate value and change the value of BUFFER_LEN in set.c to this
        updated value
     */
    char read_buff[1024];
    read_buff[len-1] = '\0';
    printf("Final values in set: ");
    len = read(fd, read_buff, 1024);
    puts(read_buff);
    close(fd);
    return 0;
}