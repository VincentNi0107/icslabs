//518021910834 ZHENYANG NI

#include "cachelab.h"
#include "unistd.h"
#include "getopt.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"

typedef struct {
    bool valid;
    int tag;
    int atime;
}cache_line;

int main(int argc,char* argv[])
{
    int opt,s,E,b;
    char t[50];
    bool verbose=0;
    //parsing command
    while(-1!=(opt=getopt(argc,argv,"s:E:b:t:"))){
        switch(opt){
            case 's':
                s=atoi(optarg);
                break;
            case 'E':
                E=atoi(optarg);
                break;
            case 'b':
                b=atoi(optarg);
                break;
            case 't':
                strcpy(t,optarg);
                break;
            case 'v':
                verbose=1;
                break;
            default:
                break;
        }
    }

    if(s<=0||E<=0||b<=0){
        printf("Wrong cache size!");
        exit(-1);
    }
    FILE* trace_file=fopen(t,"r");
    if(trace_file==NULL){
        printf("Wrong trace file!");
        exit(-1);
    }

    int num_sets=1<<s;
   //initialize cache
   cache_line** cache=(cache_line**)calloc(num_sets,sizeof(cache_line*));
   for(int i=0; i < num_sets; ++i){
       cache[i]=(cache_line*)calloc(E,sizeof(cache_line));
   }

   for(int i=0;i<num_sets;++i){
       for(int j=0;j<E;++j){
           cache[i][j].valid=0;
           cache[i][j].atime=-1;
       }
   }
    char op;
    unsigned long addr;
    int size;
    int hit_cnt=0,miss_cnt=0,evic_cnt=0;

    while(fscanf(trace_file," %c %lx,%d\n",&op,&addr,&size)>0){
        if(op=='I')continue;
        int set_idx=(addr<<(64-b-s))>>(64-s);
        int  tag=addr>>(b+s);
        int i;
        if(verbose)printf("%c  ",op);
        for(i=0;i<E;++i){
            if(cache[set_idx][i].valid)cache[set_idx][i].atime++;
        }
        for(i=0;i<E;++i){
            if(cache[set_idx][i].tag==tag&&cache[set_idx][i].valid){//hit
                cache[set_idx][i].atime=0;
                hit_cnt++;
                if(op=='M')hit_cnt++;
                if(verbose)printf("hit ");
                break;
            }
        }
        if(i==E){//miss
            int max_atime=-1,lru;
            if(verbose)printf("miss");
            miss_cnt++;
            if(op=='M')hit_cnt++;
            for(i=0;i<E;++i){
                if(!cache[set_idx][i].valid){
                        cache[set_idx][i].valid=1;
                        cache[set_idx][i].atime=0;
                        cache[set_idx][i].tag=tag;
                        break;
                }
                else{//evic
                    if(cache[set_idx][i].atime>max_atime){
                        max_atime=cache[set_idx][i].atime;
                        lru=i;
                    }
                }
            }
            if(i==E){
                evic_cnt++;
                if(verbose)printf("evic");
                cache[set_idx][lru].atime=0;
                cache[set_idx][lru].tag=tag;
            }
        }
        if(verbose)printf("\n");
    }
    fclose(trace_file);
    for (int i=0;i<num_sets;++i)free(cache[i]);
    free(cache);
    printSummary(hit_cnt, miss_cnt, evic_cnt);
    return 0;
}
