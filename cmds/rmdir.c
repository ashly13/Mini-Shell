#include <stdio.h>
#include <unistd.h>

int main(int argc, char ** argv){
	if (argc == 1){
		fprintf(stderr, "minsh: Argument required\n");
		return 1;
	}
	int i = 1;
	for (i = 1 ; i < argc ; i++){
		// Try deleting the directory
		if ( rmdir(argv[i]) < 0 ){
			fprintf(stderr, "minsh: %s - ", argv[i]);
			perror("");
			continue;
		}
	}
	return 0;
}
