#include <stdio.h>
#include <stdlib.h>

int main() {
    char **m = (char**)malloc(sizeof(char*));
    long *ct = (long*)malloc(sizeof(long));

    long len = 0;
    while(1) {
        char *o = (char*)malloc(8195);
        if(scanf("%s", o) == EOF) break;
        
        int indicator = 0;
        if(len != 0) {
            int i = len-1;
            int tf = 1;
            int ind = 0;
            while((m[i][ind] != '\0') && (o[ind] != '\0')) {
                if(m[i][ind] != o[ind]) {
                    tf = 0;
                    break;
                }
                ind++;
            }
            if((m[i][ind] == '\0') ^ (o[ind] == '\0')) tf = 0;

            if(tf) {
                ct[i]++;
                indicator = 1;
            }
        
        }

        if(!indicator) {
            m[len] = o;
            ct[len] = 1;
            len++;
            char **tempArr = (char**)malloc((len+1)*sizeof(char*));
            long *tempCt = (long*)malloc((len+1)*sizeof(long));
            for(int i = 0; i < len; i++) {
                tempArr[i] = m[i];
                tempCt[i] = ct[i];

            }
            ct = tempCt;
            m = tempArr;
        }
        else free(o);
    }

    for(int i = 0; i < len; i++) {
        printf("%ld %s\n",ct[i],m[i]);
    }
    free(ct);
    for(int i = 0; i <= len; i++) {
        free(m[i]);
    }
    
    return 0;
}