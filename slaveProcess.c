#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

int main(void) 
{
	char fileToHash[256];
	
	pid_t child;
	int fd[2];
	if(pipe(fd) < 0)
	{
		fprintf(stderr, "Pipe Failed\n");
		perror("Pipe Failed\n");
		exit(1);
	}

	child = fork();
	if(child < 0)
	{
		fprintf(stderr, "Fork Failed\n");
		perror("Fork Failed\n");
		exit(1);
	}
	else if (child == 0)
	{
		dup2(fd[1], 1);
		close(fd[0]);
		char command [255 + 7];
		sprintf(command, "%s %s", "md5sum", fileToHash); 
		system(command);
		exit(1);
	}
	else
	{
		char buf[255 + 7];
		close(fd[1]);
    		read(fd[0], buf, sizeof(buf));
		char new [300];
    		sprintf(new, "%s %s", "HASH: ", buf);
    		printf("%s\n", new);
		exit(1);
	}
	return 0;
}
