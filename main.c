#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define DATA_PATH "./Iris.csv"
#define MAX_LINE_LEN 200
#define MAX_LABEL_COUNT 30

char* labelarr[MAX_LABEL_COUNT];
int labelcnt=0;

int labelarrsearch(char* lab) {
    for(int i=0;i<labelcnt;i++) {
        if(!strcmp(labelarr[i],lab)) {
            return i;
        }
    }
    //No matching label was found
    labelarr[labelcnt]=lab;
    labelcnt++;
    return labelcnt-1;
}

int row,col; //number of rows and columns in the csv file

void count_row(const char* filename) {
    FILE* tempfp;
    fopen_s(&tempfp,filename,"r");
    char* line=malloc(MAX_LINE_LEN);
    int rowcount=0;
    while((fgets(line,MAX_LINE_LEN,tempfp))) {
          rowcount++;
    }
    row=rowcount;
}

void count_col(const char* filename) {
    FILE* tempfp;
    fopen_s(&tempfp,filename,"r");
    char* line=malloc(MAX_LINE_LEN);
    fgets(line,MAX_LINE_LEN,tempfp);
    char* token;
    int colcount=1;
    token=strtok(line,",");
    while((token=strtok(NULL,","))) {
        colcount++;
    }
    col=colcount;
}

void print_column(const char* filename) {
    FILE* tempfp;
    fopen_s(&tempfp,filename,"r");
    char* line=malloc(MAX_LINE_LEN);
    fgets(line,MAX_LINE_LEN,tempfp);
    char* token=strtok(line,",");
    printf("%s",token);
    while((token=strtok(NULL,","))) {
        printf("\t%s",token);
    }
    return;
}

char*** read_csv(const char* filename) {
    FILE* fileptr;
    errno_t err;
    if((err=fopen_s(&fileptr,filename,"r"))) {
        printf("Error opening file. Error code: %d\n",err);
        exit(EXIT_FAILURE);
    }
    count_row(filename);
    count_col(filename);

    printf("========== Data ==========\n");
    printf("(Row, Col): (%d, %d)\n",row,col);
    printf("Column names\n");
    print_column(filename);
    printf("========================\n");

    char*** csv=malloc(row*sizeof(char**));
    char line[MAX_LINE_LEN];
    int index=0;
    while((fgets(line,MAX_LINE_LEN,fileptr))) {
            csv[index]=malloc(col*sizeof(char*));
            char* token=strtok(line,",");
            csv[index][0]=strdup(token);
            int row_index=1;
            while((token=strtok(NULL,","))) {
                csv[index][row_index]=strdup(token);
                row_index++;
            }
            index++;
    }
    return csv;
}

double dist_euclidean(double* arr1,double* arr2,int size) {
    double dist=0;
    for(int i=0;i<size;i++) {
        dist+=(arr1[i]-arr2[i])*(arr1[i]-arr2[i]);
    }
    return dist;
}

int comp(const void* arr1,const void* arr2) {
    double arg1=(*(const double**)arr1)[col];
    double arg2=(*(const double**)arr2)[col];
    if(arg1>arg2) return 1;
    else if(arg1<arg2) return -1;
    return 0;
}

int main() {
    char*** data=read_csv(DATA_PATH);
    row--; //first row is for column names
    double** X=malloc(row*sizeof(double*)); //features

    for(int i=0;i<row;i++) {
        X[i]=malloc((col+1)*sizeof(double)); //last element = dist
        for(int j=0;j<col-1;j++) {
            X[i][j]=strtod(data[i+1][j],NULL);
        }
        X[i][col-1]=(double)labelarrsearch(data[i+1][col-1]);
    }

    printf("Enter data point: ");
    double* X_new=malloc((col-1)*sizeof(double));
    for(int i=0;i<col-1;i++) {
        scanf("%lf",X_new+i);
    }

    //Calculate distance
    for(int i=0;i<row;i++) {
        X[i][col]=dist_euclidean(X[i],X_new,col);
    }

    //Sort wrt dist
    qsort(X,row,sizeof(double*),comp);

    int k;
    printf("Enter value of k: ");
    scanf("%d",&k);
    int* labelcnt=calloc(MAX_LABEL_COUNT,sizeof(int));
    for(int i=0;i<k;i++) {
        labelcnt[(int)X[i][col-1]]++;
    }
    int max=-1,maxidx=0;
    for(int i=0;i<MAX_LABEL_COUNT;i++) {
        if(max<labelcnt[i]) {
            max=labelcnt[i];
            maxidx=i;
        }
    }
    print_column(DATA_PATH);
    for(int i=0;i<k;i++) {
        for(int j=0;j<col-1;j++) {
            printf("%lf\t",X[i][j]);
        }
        printf("%s",labelarr[(int)X[i][col-1]]);
    }
    printf("The predicted label of data is: %s\n",labelarr[maxidx]);
    return 0;
}
