#include <stdio.h>
#define DIFF 32
#define L_UPPER 65
#define R_UPPER 90

int toLowerCase(char **source, size_t n, char **dest){
    //for(int i = 0; ; i++){
    char *str = *source;
    size_t i = 0;
    //LowerCase : 97..122
    //UpperCase : 65.. 90
    // \0       : 0
    // \n       : 10
    while(str[i] != '\n' && str[i] != '\0'){
        char c = (char) (str[i] >= L_UPPER && str[i] <= R_UPPER ? str[i] + DIFF : str[i]);
        printf("%c", c);

        i++;
    }
    printf("%d", str[i]);
    //}
}


int main(){
    char *s = "HeLlO wOrLd";
    toLowerCase(&s, 5, &s);



    return 0;
}