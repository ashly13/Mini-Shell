#include <stdio.h>
#include <unistd.h>

int main(){
	write(1, "\33[H\33[2J", 7);
	return 0;
}
