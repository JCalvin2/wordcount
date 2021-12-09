#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

#define READ_END 0
#define WRITE_END 1

int main(int argc, char **argv)
{
	pid_t pid;
	int fd1[2];
	int fd2[2];

	/*Error Checking*/

        /*Checking if there is a command line argument (file) present*/
        if(argv[1] == NULL) //if the command line has no argument
        {
                printf("Please input a file through the command line: ./pwordcount <filename.txt>\n");
                exit(0);
        }

        /*Checking to see if the File is empty*/
        FILE *f = fopen(argv[1], "r"); //opening the file for reading
        if(f != NULL)
        {
		//seek through the file and obtaining the size of the file (how much is in it)
                fseek(f, 0, SEEK_END);
                int filesize = ftell(f);
                rewind(f);
                if(filesize == 0)
                {
                        printf("Please input a txt file with words.\n");
			printf("Word Count = 0\n");
                        exit(0);
                }
        }

        /*Checking to see if the given file is a txt*/
        char *filename = argv[1];
        char *ext = "txt";
        if(strstr(filename, ext) == NULL) //if the filename doesn't have the extension txt
        {
               printf("Please input a txt file.\n");
               exit(0);
	}

	/*Creating Pipe 1*/
	if(pipe(fd1) == -1)
	{
		fprintf(stderr, "Pipe 1 has Failed\n");
		return 1;
	}

	/*Creating Pipe 2*/
	if(pipe(fd2) == -1)
	{
		fprintf(stderr, "Pipe 2 has Failed\n");
		return 1;
	}
	
	/*Creating a Child Process with Fork*/
	pid = fork();

	if (pid < 0)
	{
		fprintf(stderr, "Fork has Failed\n");
		return 1;
	}

	/*Parent Process*/
	if (pid > 0)
	{
		/*Reading Input File*/
		printf("Process 1 is reading the input file now\n");
		fseek(f, 0, SEEK_END);
		int BUFFER_SIZE = ftell(f);
		rewind(f);
		char array[BUFFER_SIZE];

		/*Putting file into array*/
		fgets(array, BUFFER_SIZE, f);

		printf("Process 1 is sending data to Process 2\n");
		/*Write to the pipe 1*/
		write(fd1[WRITE_END], array, BUFFER_SIZE + 1);

		/*Close the write end of pipe 1*/
		close(fd1[WRITE_END]);
		
		printf("Process 1 is waiting on Process 2\n");
		wait(NULL);
		
		printf("Process 1 has recieved data from Process 2 and is reading\n");
		int spaces;
		/*Read from Pipe 2*/
		read(fd2[READ_END], &spaces, sizeof(spaces));

		/*Print Result*/
		printf("Process 1: Total Number of Words in the file: %d ", spaces);
		printf("\n");
	}

	/*Child Process*/
	else
	{
		//creating array for the data
		int BUFFER_SIZE = 1000;
		char array[BUFFER_SIZE];
		
		printf("Process 2 is reading data from Process 1\n");
		/*Read from pipe 1*/
		read(fd1[READ_END], array, BUFFER_SIZE);
		
		printf("Process 2 is counting the words now\n");
		int spaces = 1; //creating variable for the number of spaces in the file/array
		for(int i = 0; i < strlen(array); i++)
		{
			if(array[i] == ' ')
			{
				spaces++;
			}
		} 
		
		printf("Process 2 is sending results to Process 1\n");
		/*Write result to Pipe 2*/
		write(fd2[WRITE_END], &spaces, sizeof(spaces));
	}
	
	return 1;
}
