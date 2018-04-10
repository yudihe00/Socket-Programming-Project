// read txt line by line
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

int main()
{
    // char const* const fileName = argv[1]; /* should check that argc > 1 */
    char *fileName="backendA.txt";
    FILE* file = fopen(fileName, "r"); /* should check the result */
    char line[101];
    char function[10]="search";
    char word[20]="Bumble";
    int wordLength = strlen(word);
    printf("debug:word length is %d\n",wordLength);
    int findSame=0;
    char definition[100];
    char similarWord[100][20];
    //char* similarWord= (char *) malloc(100);
    //char* s = (char *) malloc(15); 
    int similarWordNumber=0;

    // change first letter of word to big case
    if(word[0]>='a' && word[0]<='z') word[0]=word[0]+('A'-'a');

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
                    strncpy(definition,line+lastIndex+4,101-lastIndex);
                    printf("definition is %s\n",definition);
                }
                
            } else if (diffNumber==1){
                strcpy(similarWord[similarWordNumber],dicWord);
                //similarWord[similarWordNumber]=dicWord;

                
                printf("debug: similarWord[%d] the similar word is %s\n",similarWordNumber,similarWord[similarWordNumber] );
                //printf("debug: similarWord[0] the similar word is %s\n",similarWord[0] );
                similarWordNumber++;
            }
        }
        
        //parseLine
        //search
    }
    printf("debug: similar word number is %d\n", similarWordNumber);
    for (int i=0; i<similarWordNumber; i++) {
        printf("debug: i=%d, the final similar word is %s\n", i, similarWord[i]);
    }
    //printf("debug:word is %s\n",word);
    /* may check feof here to make a difference between eof and io failure -- network
       timeout for instance */

    fclose(file);

    return 0;
}