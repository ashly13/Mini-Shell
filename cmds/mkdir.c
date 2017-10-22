#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char ** argv){
	if (argc == 1){
		fprintf(stderr, "minsh: Argument required\n");
		return 1;
	}
	int i = 1;
	for (i = 1 ; i < argc ; i++){
		// Try creating the directory
		if ( mkdir(argv[i], 0755) < 0 ){
			fprintf(stderr, "minsh: %s - ", argv[i]);
			perror("");
			continue;
		}
	}
	return 0;
}
