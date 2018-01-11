/* 
    author: liteng 
    email: 707704459@qq.com
	this program is used to split data by inner index sequence.
 */

#include <stdio.h>
#include <zlib.h>
#include <zconf.h>
#include <stdlib.h>
#include <getopt.h>
#include <wctype.h>
#include "khash.h"
#include "kseq.h"
#define MAXLINE 1000
KHASH_MAP_INIT_STR(strh, int)
KSEQ_INIT(gzFile, gzread)

typedef kh_strh_t strhash;
int MAX_INDEX_LEN = 0;
//unsigned int SUCCESS_READS = 0;

int has_prefix(const char *s1, char *s2){

	if (s1 == NULL || s2 == NULL)
		return -1;
	int i;
	for (i = 0; (*(s1+i)!='\0' && *(s2+i)!='\0');i++){
        if (*(s1+i) != *(s2+i))
			return 0;
	}
	return 1;
}

strhash *read_index(FILE *f, char *index_type)
{
	/* read the index info to a hashtable */
	strhash *h = kh_init(strh);
	khint_t k;
	char buff[MAXLINE+1];
	char *delim = "\t";
	int index_l = 0;

	while(fgets(buff, MAXLINE, f) != NULL){

        buff[strlen(buff)-1] = '\0';
		//printf("%s\t%d\n", buff, strlen(buff));
		// split the buff info
		int count = 0;
		char *s = NULL;
		s = strtok(buff, delim);
		if (strcmp(s, index_type) == 0){
			while (count < 1){
				s = strtok(NULL, delim);
				count ++;
			}
		
	    // put the index info to hashtable	
		char *s1 = strdup(s);
		int ret;
		k = kh_put(strh ,h, s1, &ret);
		index_l = strlen(s1);
		if (index_l > MAX_INDEX_LEN){
            MAX_INDEX_LEN = index_l;
		}
		kh_value(h, k) = index_l;
		
		}
	}
	return h;
}

void splitfastq(FILE *fq1_list, FILE *fq2_list, FILE *fq1_out, FILE *fq2_out, strhash *h, unsigned long long cut_reads_num){
	
	/* use the index of strhash table to find seq */
	unsigned long long  success_reads_num = 0;
	char buff1[MAXLINE];
	char buff2[MAXLINE];
	gzFile *in_fq1, *in_fq2;
	kseq_t *seq1_iter, *seq2_iter;
	khint_t k1, k2;
	
	while (fgets(buff1, MAXLINE, fq1_list)!=NULL && fgets(buff2, MAXLINE, fq2_list)!=NULL) {
		buff1[strlen(buff1)-1] = '\0';
		buff2[strlen(buff2)-1] = '\0';
		if ((in_fq1 = gzopen(buff1, "r")) == NULL) {
			fprintf(stderr, "Can't open file:%s\n", buff1);
			fcloseall();
			exit(1);
			
	    }
		if ((in_fq2 = gzopen(buff2, "r")) == NULL) {
			fprintf(stderr, "Can't open file:%s\n", buff2);
			fcloseall();
			exit(1);
			
		}
		seq1_iter = kseq_init(in_fq1);
		seq2_iter = kseq_init(in_fq2);
		int l1, l2;
		while (1 && (success_reads_num < cut_reads_num)){
			l1 = kseq_read(seq1_iter);
			l2 = kseq_read(seq2_iter);
			if (l1 >= 0 && l2 >= 0) {
				//printf("fastq1:%s\n", seq1_iter->seq.s);
				//printf("fastq2:%s\n", seq2_iter->seq.s);
				for ( k1 = kh_begin(h); k1 != kh_end(h); ++k1){
					if (kh_exist(h ,k1)){
						char *index1 = kh_key(h, k1);
						if (has_prefix(seq1_iter->seq.s, index1)) {

							for ( k2 = kh_begin(h); k2 != kh_end(h); ++k2){
								if (kh_exist(h ,k2)){
								    char *index2 = kh_key(h, k2);
							        if (has_prefix(seq2_iter->seq.s, index2)){
										
										/* 
										 the pair read is splited success,
										  ouput the pair reads 
										*/
										fputc('@', fq1_out);
										fputs(seq1_iter->name.s, fq1_out);
										fputc('#', fq1_out);
										fputs(index1, fq1_out);
										fputc('+', fq1_out);
										fputs(index2, fq1_out);
										fputc('\n', fq1_out);
										fputs(seq1_iter->seq.s+MAX_INDEX_LEN+1, fq1_out);
										fputc('\n', fq1_out);
										fputs("+\n", fq1_out);
										fputs(seq1_iter->qual.s+MAX_INDEX_LEN+1, fq1_out);
										fputc('\n', fq1_out);
										
										fputc('@', fq2_out);
										fputs(seq2_iter->name.s, fq2_out);
										fputc('#', fq2_out);
										fputs(index1, fq2_out);
										fputc('+', fq2_out);
										fputs(index2, fq2_out);
										fputc('\n', fq2_out);
										fputs(seq2_iter->seq.s+MAX_INDEX_LEN, fq2_out);
										fputc('\n', fq2_out);
										fputs("+\n", fq2_out);
										fputs(seq2_iter->qual.s+MAX_INDEX_LEN, fq2_out);
										fputc('\n', fq2_out);

										/*
										 count the success read
										*/
										success_reads_num += 2;
										goto MATCH_INDEX;
										
									}
								}
					    }	
					  }
				   } 
			    }
                MATCH_INDEX:;
			}
            else {
				break;
			} 
	    }
    }
    kseq_destroy(seq1_iter);
    kseq_destroy(seq2_iter);
	gzclose(in_fq1);
	gzclose(in_fq2);
}

int main(int argc, char *argv[])
{

    /* get the argument */

	int ch;
	unsigned long long cut_reads_num = 99999999999999;
	char *index_file = NULL;
	char *input_fq1 = NULL;
	char *input_fq2 = NULL;
	char *out_dir = NULL;
	char *prefix = NULL;
	char *index_type = NULL;

	while ((ch = getopt(argc, argv, "i:l:L:o:p:t:c:h")) != -1 ){
		switch(ch){
			case 'i':
				index_file = optarg;
				break;
			case 'l':
				input_fq1 = optarg;
				break;
			case 'L':
				input_fq2 = optarg;
				break;
			case 'o':
				out_dir = optarg;
				break;
			case 'p':
				prefix = optarg;
				break;
			case 't':
				index_type = optarg;
				break;
			case 'c':
				//printf("%s\n", optarg);
				cut_reads_num = strtoull(optarg, NULL, 0);
				//printf("cut_reads_num is %llud\n", cut_reads_num);
				break;
			case 'h':
				printf("usage %s -i index_file -l fq1.list -L fq2.list -o ourdir -p prefix -t index_type -c cut_reads_num\n", argv[0]);
				printf("-i\tinput index_file path\n");
				printf("-l\tinput read1 fq_list path\n");
				printf("-L\tinput read2 fq_list path\n");
				printf("-o\toutput dir path\n");
				printf("-p\tthe output file prefix\n");
				printf("-c\tthe ouput reads number,default is 99999999999999\n");
				return 0;
		}
	}

	/* get index info */
	FILE *index_f;
	while ((index_f = fopen(index_file, "r")) == NULL) {
		fprintf(stderr, "can't open %s\n", index_file);
		return -1;
	}
	khash_t(strh) *index_h;
	index_h = read_index(index_f, index_type);
	khint_t k;
	//for (k = kh_begin(index_h); k!=kh_end(index_h);k++){
    //    if (kh_exist(index_h, k))
	//	printf("%s -> %d\n",kh_key(index_h,k), kh_value(index_h, k));
	//}
	fclose(index_f);

	/* split fastq data */
	FILE *fq1_list, *fq2_list, *fq1_out, *fq2_out;
	
	char fq1_o_name[MAXLINE], fq2_o_name[MAXLINE];

	strcpy(fq1_o_name, out_dir);
	strcat(fq1_o_name, "/");
	strcat(fq1_o_name, prefix);
	strcat(fq1_o_name, "_combine_R1.fastq");

	strcpy(fq2_o_name, out_dir);
	strcat(fq2_o_name, "/");
	strcat(fq2_o_name, prefix);
	strcat(fq2_o_name, "_combine_R2.fastq");
	
	while ((fq1_list = fopen(input_fq1, "r")) == NULL){
		fprintf(stderr, "can't open %s\n", input_fq1);
		return -1;
	}

	while ((fq2_list = fopen(input_fq2, "r")) == NULL){
		fprintf(stderr, "can't open %s\n", input_fq2);
		return -1;
	}

	while ((fq1_out = fopen(fq1_o_name, "w")) == NULL) {
		fprintf(stderr, "can't Create %s\n", fq1_o_name);
		return -1;
	}

	while ((fq2_out = fopen(fq2_o_name, "w")) == NULL) {
		fprintf(stderr, "can't Create %s\n", fq2_o_name);
		return -1;
	}


	splitfastq(fq1_list, fq2_list, fq1_out, fq2_out, index_h, cut_reads_num);
	kh_destroy(strh, index_h);
	fcloseall();
    return 0;
}
