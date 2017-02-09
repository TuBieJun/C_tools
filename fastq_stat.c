#include<stdio.h>
#include<string.h>
#include<zlib.h>
#include<zconf.h>
#define N 999

long READ_NUM = 0;
long BASE_NUM = 0;
long GC = 0;
long Q20 = 0;
long Q30 = 0;

void GC_stat(char *read)
{
     int i;
     for (i=0;i<strlen(read);i++){
         if (read[i] == 'C' || read[i] == 'G'){
            GC += 1;        
         }
     }
}

void quality_stat(char *read_qual)
{
    int i;
    for (i=0;i<strlen(read_qual);i++){
        if ((read_qual[i]-33)>=20){
           Q20 += 1;
           if((read_qual[i]-33)>=30){
           Q30 += 1;
           }
        }
    }
}

void gzipfastq(gzFile *fp)
{
    char ch[N+1];
    int count = 1;

    while (gzgets(fp, ch, N) != NULL){
          if (count%4 == 2){
		  BASE_NUM += strlen(ch);
		  READ_NUM += 1;
                  GC_stat(ch);
	  }
          if (count%4 == 0){
                  quality_stat(ch);
          }
	  count += 1;
    }
    printf("read numbers:\t%ld\n",READ_NUM);
    printf("base numbers:\t%ld\n",BASE_NUM);
    printf("GC content:\t%f\n",(float)GC/BASE_NUM);
    printf("Q20 content:\t%f\n",(float)Q20/BASE_NUM);
    printf("Q30 content:\t%f\n",(float)Q30/BASE_NUM);
}

void fastq(FILE *fp)
{
    char ch[N+1];
    int count = 1;

    while (fgets(ch, N, fp) != NULL){
	if (count%4 == 2){
		BASE_NUM += strlen(ch);
		READ_NUM += 1;
                GC_stat(ch);
	}
        if (count%4 == 0){
                quality_stat(ch);
                
        }
	count += 1;

    }
    printf("read numbers:\t%ld\n",READ_NUM);
    printf("base numbers:\t%ld\n",BASE_NUM);
    printf("GC content:\t%f\n",(float)GC/BASE_NUM);
    printf("Q20 content:\t%f\n",(float)Q20/BASE_NUM);
    printf("Q30 content:\t%f\n",(float)Q30/BASE_NUM);
}	

int main(int argc,char *argv[])
{
	char ch[N+1];
	int count=1;
        if (strcmp(argv[2], "gz") == 0){
		gzFile *fp;
                if ((fp=gzopen(argv[1], "rb")) == NULL ){
                     printf("Cannot find file:%s\n",argv[1]);
                     exit(1);
                }
		//fp = gzopen(argv[1],"rb");
                gzipfastq(fp);
		gzclose(fp);
	}else if(strcmp(argv[2], "fastq") == 0){
		FILE *fp;
		if ((fp = fopen(argv[1],"r")) == NULL){
                    printf("Cannot find file:%s\n",argv[1]);
                    exit(1);
                }
		fastq(fp);
		fclose(fp);
	}else{
		 printf("please input fastq or fastq.gz file\n");
	}
	return 0;
}
