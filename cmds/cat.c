#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char ** argv){

	// If no arguments are specified, read from standard input
	if ( argc == 1 ){
		// Display data from input to output
		char buffer[1024];
		int nbytes;	
		while ( 1 ){
			// Read from input
			nbytes = read(0, buffer, 1024);
			if (nbytes < 0){	// Error
				perror("\nminsh");
				break;
			}
			else if (nbytes == 0){	// End of file
				write(1, "\n", 1);
				break;
			}
			
			// Write to output
			if ( write(1, buffer, nbytes) < 0 ){	// Error
				perror("\nminsh");
				break;
			}
		}
		return 1;
	}

	// Loop through the available arguments (files)
	int i = 1;
	int fd;
	for ( i = 1 ; i < argc ; i++ ){

		// Open file
		if ( (fd = open(argv[i], O_RDONLY)) < 0 ){
			fprintf(stderr, "\nminsh: %s - ", argv[i]);
			perror("");
			return 0;
		}

		printf("\n\n%s:\n\n", argv[i]);

		// Display data from file
		char buffer[1024];
		int nbytes;	
		while ( 1 ){
			// Read from file
			nbytes = read(fd, buffer, 1024);
			if (nbytes < 0){	// Error
				perror("\nminsh");
				break;
			}
			else if (nbytes == 0){	// End of file
				write(1, "\n", 1);
				break;
			}
			
			// Write to output
			if ( write(1, buffer, nbytes) < 0 ){	// Error
				perror("\nminsh");
				break;
			}
		}

		close(fd);
		
	}
	printf("\n\n");
	return 0;
}

