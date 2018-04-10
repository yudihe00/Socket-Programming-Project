// searcha.c
// simple version of servera, only string manipulation
#include <stdio.h>
#include <string.h>

// parse line
// return word's last character index
int parseLine(char line[101]){
    int i=0;
    while(line[i]!=' '){
        i++;
    }
    return i-1;
}

// search function
// in function return to aws that definition, similar words number and similar words
void search(char word[],FILE* file)
{
    char line[101];
    int wordLength = strlen(word);
    //printf("debug:word length is %d\n",wordLength);
    int findSame=0;
    char definition[100];
    char definition2[100];
    char similarWord[100][20];
    //char* similarWord= (char *) malloc(100);
    //char* s = (char *) malloc(15); 
    int similarWordNumber=0;
    while (fgets(line, sizeof(line), file)) {
        /* note that fgets don't strip the terminating \n, checking its
           presence would allow to handle lines longer that sizeof(line) */
        //printf("%s", line); 
        int lastIndex=parseLine(line);
        //printf("debug:last index is %d\n",lastIndex);

        if (lastIndex+1==wordLength) //have same length
        {
            int same=1;
            int diffNumber=0;
            char dicWord[20];
            for(int i=0; i<=lastIndex; i++) //judge is every characters are same
            {   
                dicWord[i]=line[i];
                if(line[i]!=word[i]) {
                    same=0;
                    diffNumber++;
                }
            }
            dicWord[lastIndex+1]='\0';
            //printf("debug:same is %d\n",same);
            if (same==1)  
            {
                if (findSame!=1) // if find same word, only return the first definition
                {
                    findSame=1;
                    strncpy(definition,line+lastIndex+5,101-lastIndex);
                    //printf("definition is %s\n",definition);
                }
                
            } else if (diffNumber==1){
                strcpy(similarWord[similarWordNumber],dicWord);
                strncpy(definition2,line+lastIndex+5,101-lastIndex); //return the last definition for long slimilar words
                //printf("debug: similarWord[%d] the similar word is %s\n",similarWordNumber,similarWord[similarWordNumber] );
                //printf("debug: similarWord[0] the similar word is %s\n",similarWord[0] );
                similarWordNumber++;
            }
        }
    }
    printf("debug: similar word number is %d\n", similarWordNumber);
    if (findSame ==1){
        printf("definition is <%s",definition);
    } else {
        printf("No same words in this server\n");
    }
    int i;
    for (i=0; i<similarWordNumber; i++) {
        printf("debug: i=%d, the final similar word is %s\n", i, similarWord[i]);
    }
    if (similarWordNumber!=0){
        printf("One edit distance match is <%s>, definition is <%s",similarWord[i-1], definition2);
    }
    //printf("debug:word is %s\n",word);
}

// prefix function
// in function return to aws that words number and words
void prefix(char word[],FILE* file)
{
    char line[101];
    int wordLength = strlen(word);
    // printf("debug:word length is %d\n",wordLength);
    int findSame=0;
    char prefixWord[100][20];
    //char* similarWord= (char *) malloc(100);
    //char* s = (char *) malloc(15); 
    int prefixWordNumber=0;
    while (fgets(line, sizeof(line), file)) {
        /* note that fgets don't strip the terminating \n, checking its
           presence would allow to handle lines longer that sizeof(line) */
        //printf("%s", line); 
        int lastIndex=parseLine(line);
        // printf("debug:last index is %d\n",lastIndex);

        int same=1;
        
        char dicWord[20];
        for(int i=0; i<wordLength; i++) //judge every characters are same for word given
        {   
            dicWord[i]=line[i];
            if(line[i]!=word[i]) {
                same=0;
            }
        }
        for (int i=wordLength; i<=lastIndex; i++)
        {
            dicWord[i]=line[i];
        }
        dicWord[lastIndex+1]='\0';
        // printf("debug:dicWord is %s\n",dicWord);
        // printf("debug:same is %d\n",same);
        if (same==1)  
        {
            strcpy(prefixWord[prefixWordNumber],dicWord);
            //printf("debug: similarWord[%d] the similar word is %s\n",similarWordNumber,similarWord[similarWordNumber] );
            //printf("debug: similarWord[0] the similar word is %s\n",similarWord[0] );
            prefixWordNumber++;
        }
        
    }
    printf("debug: prefix word number is %d\n", prefixWordNumber);
    
    int i;
    for (i=0; i<prefixWordNumber; i++) {
        printf("debug: i=%d, the final similar word is %s\n", i, prefixWord[i]);
    }

    if (prefixWordNumber==0) {
        printf("debug: no prefix word find in serverA\n");
    }
    // if (similarWordNumber!=0){
    //     printf("One edit distance match is %s, definition is %s",similarWord[i-1], definition2);
    // }
    //printf("debug:word is %s\n",word);
}

// suffix function
// in function return to aws that words number and words
void suffix(char word[],FILE* file)
{
    char line[101];
    if(word[0]>='A' && word[0]<='Z') word[0]=word[0]-('A'-'a');
    if(line[0]>='a' && line[0]<='z') line[0]=line[0]+('A'-'a');

    int wordLength = strlen(word);
    //printf("debug:word length is %d\n",wordLength);
    int findSame=0;
    char suffixWord[100][20];
    //char* similarWord= (char *) malloc(100);
    //char* s = (char *) malloc(15); 
    int suffixWordNumber=0;
    while (fgets(line, sizeof(line), file)) {
        /* note that fgets don't strip the terminating \n, checking its
           presence would allow to handle lines longer that sizeof(line) */
        //printf("%s", line); 
        int lastIndex=parseLine(line);
        // printf("debug:last index is %d\n",lastIndex);

        int same=1;
        
        char dicWord[20];
        int sub = lastIndex - wordLength + 1;
        int i;
        for(i=wordLength-1; i>=0; i--) //judge every characters are same for word given
        {   
            dicWord[i+sub]=line[i+sub];
            if(line[i+sub]!=word[i]) {
                same=0;
            }
        }
        for (i=i+sub; i>=0; i--)
        {
            dicWord[i]=line[i];
        }
        dicWord[lastIndex+1]='\0';
        //printf("debug:dicWord is %s\n",dicWord);
        // printf("debug:same is %d\n",same);
        if (same==1 && (dicWord[0]>='A' && dicWord[0]<='Z'))  
        {
            strcpy(suffixWord[suffixWordNumber],dicWord);
            //printf("debug: suffixWord[%d] the suffix word is %s dicWord is %s\n",suffixWordNumber,suffixWord[suffixWordNumber],dicWord );
            //printf("debug: similarWord[0] the similar word is %s\n",similarWord[0] );
            suffixWordNumber++;
        }
        
    }
    printf("debug: suffix word number is %d\n", suffixWordNumber);
    
    int i;
    for (i=0; i<suffixWordNumber; i++) {
        printf("debug: i=%d, the final similar word is %s\n", i, suffixWord[i]);
    }

    if (suffixWordNumber==0) {
        printf("debug: no suffix word find in serverA\n");
    }
}

int main()
{
    
    char *fileName="backendA.txt";
    FILE* file = fopen(fileName, "r"); 
    char function[10]="suffix";
    char word[20]="ble";

    // change first letter of word to big case
    if(word[0]>='a' && word[0]<='z') word[0]=word[0]+('A'-'a');
    //search(word,file);

    if (strcmp(function,"search")==0) search(word, file);
    else if (strcmp(function,"prefix")==0) prefix(word, file);
    else if (strcmp(function,"suffix")==0) suffix(word,file);
    
    /* may check feof here to make a difference between eof and io failure -- network
       timeout for instance */
    fclose(file);
    return 0;
}