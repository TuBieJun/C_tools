#include <stdio.h>
#include <string.h>
#define BLOCK_SIZE 1024


int main(int argc, char *argv[]) 
{
	FILE *fp;
	while((fp = fopen(argv[1], "r")) == NULL) {
		fprintf(stderr, "can't open %s\n", argv[1]);
		return -1;
	}
	char buff[BLOCK_SIZE];
    //char *line = (char *)malloc(BLOCK_SIZE*sizeof('a'));
	while(fgets(buff, BLOCK_SIZE-1, fp) != NULL ) {
        if (buff[strlen(buff)-1] == '\n') {
			printf("%s", buff);
		} else {
			char *temp = (char *)malloc(2*BLOCK_SIZE*sizeof('a'));
			strcat(temp, buff);
			if (fgets(buff, BLOCK_SIZE-1, fp) != NULL) {
				strcat(temp, buff);
				printf("%s", temp);
				free(temp);
			} else {
				return 0;
			}
		}
	}
	fclose(fp);
	return 0;
}
