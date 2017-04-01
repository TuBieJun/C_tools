#include <stdio.h>
#include <string.h>
#include <zconf.h>
#include <zlib.h>
#define N 1000

void read_fastq(gzFile *fp,char *target_id){
        int num_row = 1;
	char ch[N];
        while(gzgets(fp,ch,N)!= NULL){
		if (num_row%4 == 1){
                     if(strstr(ch,target_id)){
			printf("%s",ch);
                        gzgets(fp,ch,N);
		        printf("%s",ch);
			gzgets(fp,ch,N);
			printf("%s",ch);
			gzgets(fp,ch,N);
			printf("%s",ch);
			exit(0);
		     }else
			     num_row++;
		  
		}else
			num_row++;
	}
	
}

int main(int argc, char *argv[])
{
	if (argc != 3){
		printf("usage: find_seq_fastq fastq.gz seq_id\n");
		exit(1);
	}
        
	gzFile *fp;
	
	if ((fp = gzopen(argv[1], "rb")) == NULL){
                printf("cannot find file:%s\n",argv[1]);
		exit(1);
	}
	read_fastq(fp,argv[2]);
	
}
