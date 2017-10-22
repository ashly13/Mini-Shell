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
#include <ctype.h>	// For tolower()

// My implementation of case insensitive string compare
int strcmpi(char const *a, char const *b)
{
	for (;; a++, b++){
		int d = tolower(*a) - tolower(*b);
		if (d != 0 || !*a)
			return d;
	}
}

int main(int argc, char ** argv){

	DIR * dir;

	// An array of directory entries would make it a lot easier to apply the options
	// I'm assuming that no directory has more than 1024 entries!
	struct dirent * entry[1024];
	int directory = 1;
	int opt_a = 0, opt_l = 0, opt_i = 0;

	// By default, the second argument will be the directory
	directory = 1;

	// Check if an option is specified
	if ( argv[1] != NULL && argv[1][0] == '-'  ){	// Option is specified
		int i = 0;
		for ( i = 1 ; i < strlen(argv[1]) ; i++ ){
			
			if ( argv[1][i] == 'l' ){
				opt_l = 1;
			}
			else if ( argv[1][i] == 'a' ){
				opt_a = 1;
			}
			else if ( argv[1][i] == 'i' ){
				opt_i = 1;
			}
			else{
				fprintf(stderr, "minsh: Invalid option\n");
				return 1;
			}
		}
			
		// If option is specified, the third argument is the directory
		directory = 2;
	}

	// Check if any directories are specified
	if ( argv[directory] == NULL ){
		argv[directory] = ".";
		argc++;
	}

	int i = directory;
	for ( i = directory ; i < argc ; i++ ){

		// Open directory stream
		if ( (dir = opendir(argv[i])) == NULL ){
			fprintf(stderr, "\nminsh: %s - No such file or directory\n\n", argv[i]);
			return 0;
		}

		printf("\n\n%s:\n", argv[i]);

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

		// Sort the directory entries based on name
		int j = 0;
		int k = 0;
		struct dirent * temp;
		for ( j = 0 ; j < entno-1 ; j++ ){
			for ( k = j+1 ; k < entno ; k++ ){
				if ( strcmpi(entry[j]->d_name, entry[k]->d_name) > 0 ){
					temp = entry[j];
					entry[j] = entry[k];
					entry[k] = temp;
				}
			}
		} 
	
		// Now apply all the options

		// See if -l is specified
		if ( opt_l == 1 ){
			char filename[1024];
			struct stat statbuf;

			for (entno = 0 ; entry[entno] != NULL ; entno++){
				// If -a is also specified
				if ( opt_a != 1 && (entry[entno]->d_name)[0] == '.'){
					continue;
				}

				// Find the absolute path of the file in order to call stat()
				strcpy(filename, argv[i]);
				strcat(filename, "/");
				strcat(filename, entry[entno]->d_name);

				// Call stat() and get file info
				if ( stat(filename, &statbuf) == -1){
					perror("minsh");
					return 1;
				}		

				// Print the file info
			
				printf("\n");
					
				// Display inode number also if -i is specified
				if ( opt_i == 1 ){
					printf("%-8ld ", entry[entno]->d_ino);
				}
				// Type of file
				if ( S_ISDIR(statbuf.st_mode) ){
					printf("d");
				}
				else if ( S_ISCHR(statbuf.st_mode) ){
					printf("c");
				}
				else if ( S_ISBLK(statbuf.st_mode) ){
					printf("b");
				}
				else if ( S_ISFIFO(statbuf.st_mode) ){
					printf("p");
				}
				else{
					printf("-");
				}
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
				printf(" %11ld", statbuf.st_size);
				// File last modification time
				struct tm * timestruct;
				timestruct = localtime(&statbuf.st_mtime);
				char timebuf[20];
				strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", timestruct);
				printf(" %15s", timebuf);
				// File Name
				printf("   %-15s", entry[entno]->d_name);
			}

		}
		else{
			for (entno = 0 ; entry[entno] != NULL ; entno++){
				if ( (entry[entno]->d_name)[0] != '.' || opt_a == 1 ){
					
					printf("\n");

					// Display inode number also if -i is specified
					if ( opt_i == 1 ){
						printf("%-8ld ", entry[entno]->d_ino);
					}

					printf("%s", entry[entno]->d_name);
				}
			}
		}
	}
	printf("\n\n");
	return 0;
}

