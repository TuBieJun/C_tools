#include <stdio.h>
#include <zlib.h>
#include <zconf.h>
#include <string.h>
#define N 999


void cut_string(char *line_string,char *out_line_string,int start, int end)
{
    int i,j;
    //char out_line_string[N+1];
    //printf("%s\n", line_string);
    for (i=start,j=0;line_string[i]&&i<=end-1;i++,j++){
	         out_line_string[j]=line_string[i];
	    }
    out_line_string[j]='\0';
    //printf("%s\n", out_line_string);	 
    //return out_line_string;

}
	 

void read_fastq(gzFile *fp, gzFile *fp_o,int c_h, int c_t)
{
    char ch[N+1];
    int count = 1;
    int read_len;
    char out_ch[N+1]; 
    while(gzgets(fp, ch, N) != NULL){
	//printf("%s\n", ch);
        if (count%4 == 2 || count%4 == 0){
		//printf("%d\n",strlen(ch));
		ch[strlen(ch)-1]='\0';
		read_len = strlen(ch);
		//printf("%d\n",strlen(ch));
		cut_string(ch,out_ch,c_h,read_len-c_t-1);
		//printf("%s\n",out_ch);
		strcat(out_ch, "\n");
	        gzputs(fp_o, out_ch);
	        strcpy(out_ch,""); // init the out string

                    	    
	}else{
		gzputs(fp_o, ch);
		//printf("%s",ch);
        }
        count += 1;	
        }
    
    //fp_o.close();
    gzclose(fp);
    gzclose(fp_o);
}

int main(int argc, char *argv[])
{
    if (argc != 5){
       printf("usage: <fastq> <cut_head_size> <cut_tail_size>\n");
       exit(1);
    }
    
    gzFile *fp;
    gzFile *fp_o;

    if ((fp = gzopen(argv[1], "rb")) == NULL) {
	 printf("Cannot find file:%s\n", argv[1]);
	 exit(1);
    }
    fp_o = gzopen(argv[4], "wb");
    
    //printf("%d\n",atoi(argv[2]));
    //printf("%d\n",atoi(argv[3]));
    read_fastq(fp, fp_o,atoi(argv[2]),atoi(argv[3]));

} 
