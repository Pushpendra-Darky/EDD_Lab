#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>

int8_t write_buf[1024];
int8_t read_buf[1024];

int main()
{
	int fd;
	int option;

	fd=open("/dev/CharDev1",O_RDWR);
	if(fd<0)
	{
		printf("ERROR IN OPENING FILE\n");
		return 0;
	}
	while(1)
	{
		printf("Please Enter The Option\n");
		printf("1. Write\n");
		printf("2. Read\n");
		printf("3. Exit\n");
		printf("\n");
		scanf("%d", &option);

		switch(option)
		{
			case 1:
				printf("ENter The String To Write Into The Driver :");
				scanf("  %[^\t\n]s", write_buf);
				printf("Data Writing ...");
			    write(fd,write_buf, strlen(write_buf)+1);
				printf("Done!\n");
				break;

			case 2:
				printf("Data Reading ...");
				read(fd,read_buf,1024);
				printf("Done!\n\n");
				printf("Data = %s\n\n", read_buf);
				break;

			case 3:
				close(fd);
				exit(1);
				break;

			default :
				printf("Enter The Valid Option\n");
				

		}
	}
	close(fd);
}