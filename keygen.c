#include <stdio.h>
#include <stdlib.h>

char generateChar(){
    int value = (rand()%27)+65;
    if(value == 91){
        value = 32;
    }
    return (char)(value);
}

int main(int argc, char* argv[]){
    //Error check for a supplied keylength
     if (argc != 2) {
        fprintf(stderr, "Error, Must supply a keylength.\n");
        return 1;
    }
    //Get keylength from cmd line
    int keylength = atoi(argv[1]);
    //Error check for positive int
       if (keylength <= 0) {
        fprintf(stderr, "Invalid key length: %s\n", argv[1]);
        return 1;
    }
    //generate key for keylength
    for(int i = 0; i < keylength; i++){
        printf("%c", generateChar());
    }
    //Last output should be a newline
    printf("\n");
    return 0;
}