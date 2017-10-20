#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <pwd.h>	// For getpwuid() - mapping uid to user name
#include <grp.h>	// For getgrgid() - mapping gid to group name
#include <time.h>	// For strftime() - converting mtime in stat() to string

int main(int argc, char ** argv){

	DIR * dir;

	// An array of directory entries would make it a lot easier to apply the options
	// I'm assuming that no directory has more than 1024 entries!
	struct dirent * entry[1024];
	char * directory;
	char option = ' ';

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
	char filename[1024];
	struct stat statbuf;
	switch (option){
		case 'a':	// Display hidden directories and files also
			for (entno = 0 ; entry[entno] != NULL ; entno++){
				printf("\n%s", entry[entno]->d_name);
			}
			break;
		case 'l':
			for (entno = 0 ; entry[entno] != NULL ; entno++){

				if ((entry[entno]->d_name)[0] == '.'){
					continue;
				}

				// Find the absolute path of the file in order to call stat()
				strcpy(filename, directory);
				strcat(filename, "/");
				strcat(filename, entry[entno]->d_name);

				// Call stat() and get file info
				if ( stat(filename, &statbuf) == -1){
					perror("minsh");
					return 1;
				}		

				// Print the file info
				
				printf("\n");
				S_ISDIR(statbuf.st_mode)? printf("d") : printf("-");
				// Permissions for User
				(S_IRUSR && statbuf.st_mode)? printf("r") : printf("-");
				(S_IWUSR && statbuf.st_mode)? printf("w") : printf("-");
				(S_IXUSR && statbuf.st_mode)? printf("x") : printf("-");
				// Permissions for Group
				(S_IRGRP && statbuf.st_mode)? printf("r") : printf("-");
				(S_IWGRP && statbuf.st_mode)? printf("w") : printf("-");
				(S_IXGRP && statbuf.st_mode)? printf("x") : printf("-");
				// Permissions for Other Users
				(S_IROTH && statbuf.st_mode)? printf("r") : printf("-");
				(S_IWOTH && statbuf.st_mode)? printf("w") : printf("-");
				(S_IXOTH && statbuf.st_mode)? printf("x") : printf("-");
				// Number of links
				printf(" %3li",statbuf.st_nlink);
				// User Name
				printf(" %-15s", getpwuid(statbuf.st_uid)->pw_name);
				// Group name
				printf(" %-15s", getgrgid(statbuf.st_gid)->gr_name);
				// File Size
				printf(" %8ld", statbuf.st_size);
				// File last modification time
				struct tm * timestruct;
				timestruct = localtime(&statbuf.st_mtime);
				char timebuf[20];
				strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", timestruct);
				printf(" %15s", timebuf);
				// File Name
				printf("   %-15s", entry[entno]->d_name);
			}
			break;
		case ' ':
			for (entno = 0 ; entry[entno] != NULL ; entno++){
				if ((entry[entno]->d_name)[0] != '.'){
					printf("\n%s", entry[entno]->d_name);
				}
			}
			break;
		default:
			fprintf(stderr, "minsh: Invalid argument to ls");
	}
	printf("\n\n");

	return 0;
}

