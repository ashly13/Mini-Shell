#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <errno.h>


int main(int argc, char ** argv){

	DIR * dir;
	struct dirent * entry;

	// Open the directory stream
	if ( argv[1] == NULL ){		// List current directory
		if ( (dir = opendir(".")) == NULL ){
			perror("minsh");
			return 0;
		}
	}
	else{
		if ( (dir = opendir(argv[1])) == NULL ){
			perror("minsh");
			return 0;
		}
	}

	// Read every directory entry and print it
	
	errno = 0;	// Distinguish between end of directory
			// and error

	while ( 1 ){
		entry = readdir(dir);
		if (entry == NULL){
			if (errno == 0){	// End of directory
				printf("\n\n");
				return 0;
			}
			else{	// Error has occurred
				perror("minsh");
				return 1;
			}
		}
		else{	// Entry available
			printf("\n%s",entry->d_name);
		}
	}
	return 0;
}

