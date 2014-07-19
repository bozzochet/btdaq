#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
	int log, uid, d;
	if (argc < 4) {
		fprintf(stderr, "Usage: %s logfile uid program [params]\n", argv[0]);
		return 1;
	}



	log = open(argv[1], O_CREAT|O_APPEND|O_WRONLY, 0600);
	uid = atoi(argv[2]);
	if (log < 0) {
		fprintf(stderr, "Error opening log file: %s\n", strerror(-log));
		return 1;
	}
	if (uid > 0) {
		setuid(uid);
	}
	d = dup2(log, 1);
	if (d != 1) {
		fprintf(stderr, "Error duplicating descriptor: %s\n", strerror(-d));
		return 1;
	}
	
	d = dup2(log, 2);
	if (d != 2) {
		fprintf(stderr, "Error duplicating descriptor: %s\n", strerror(-d));
		return 1;
	}
	
	daemon(0, 1);
	d = execvp(argv[3], argv + 3);
	printf("Error executing %s: %s\n", argv[3], strerror(-d)); 
}
