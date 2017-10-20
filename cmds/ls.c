#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <errno.h>


int main(int argc, char ** argv){

	DIR * dir;

	// An array of directory entries would make it a lot easier to apply the options
	// I'm assuming that no directory has more than 1024 entries!
	struct dirent * entry[1024];
	char * directory;
	char option;

	// By default, the second argument will be the directory
	directory = argv[1];

	// Check if an option is specified
	if ( argv[1] != NULL && argv[1][0] == '-'  ){	// Option is specified
		option = argv[1][1];
		// If option is specified, the third argument is the directory
		directory = argv[2];
	}

	// Check if directory is specified
	if ( directory == NULL ){
		directory = ".";
	}

	// Open directory stream
	if ( (dir = opendir(directory)) == NULL ){
		perror("minsh");
		return 0;
	}

	// Read every directory entry and print it
	
	errno = 0;	// Distinguish between end of directory
			// and error

	int entno = 0;	// Entry number - keep track of position in the array of entries
	while ( 1 ){
		// Adds the entry to the array of entries
		// Automatically adds NULL to specify the end too!!
		entry[entno] = readdir(dir);
		if (entry[entno] == NULL){
			if (errno == 0){	// End of directory
				break;
			}
			else{	// Error has occurred
				perror("minsh");
				return 1;
			}
		}
		entno++;
	}
	
	// Now apply the options
	for (entno = 0 ; entry[entno] != NULL ; entno++){
		if (entry[entno]->d_name[0] == '.'){	// Hidden file or directory 
			if (option == 'a'){	// Option is 'a' (for hidden)
				printf("\n%s", entry[entno]->d_name);
			}
		}
		else{
			printf("\n%s", entry[entno]->d_name);
		}
	}
	printf("\n\n");

	return 0;
}

