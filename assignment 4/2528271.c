#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

struct node{
   char sequence[81];
   struct node *next;
};

struct pattern{
    char p[81];
    int times;
    int sequences;
};

struct node * readSequences(char filename[]);
void searchPatterns(struct node *head, struct pattern *patterns, int numberOfPatterns);
void sortPatterns(struct pattern *patterns, int numberOfPatterns);
void printPatterns(struct pattern *patterns, int numberOfPatterns);

int main(int argc, char *argv[]){
    char filename[81];
    struct node * head = NULL;
    struct pattern *patterns = NULL;
    int numberOfPatterns,i=0;
    printf("Enter the file path containing the DNA sequences: ");//DNAsequences.txt
    gets(filename);
    printf("Enter the number of patterns: ");
    scanf("%d",&numberOfPatterns);//getting the number of patterns from the user.
    patterns=(struct pattern*)malloc(sizeof(struct pattern)*numberOfPatterns);//initializing array with size of number of patterns.
    for(i;i<numberOfPatterns;i++)//populating the array with sequences to use later for string matching.
    {
        printf("Enter the pattern: ");//TGGAT GATG TACCACAAA GTAG
        scanf("%s",patterns[i].p);

    }
    printf("\n");//to match the sample run.

    head= readSequences(filename);//reading the file and creating linked list.

    searchPatterns(head, patterns,  numberOfPatterns);//searching for the given sequences in the array in the txt file.

    sortPatterns(patterns, numberOfPatterns);//sorting the array statistics according to the times the sequence appears in the txt file, using insertion sort.

    printPatterns(patterns, numberOfPatterns);//printing the array of patterns.




    return 0;
}

struct node * readSequences(char filename[]){
	struct node * head = NULL, *temp,*current;
	char c;
	FILE* inFile;
	int countlines=0,i=0;
	inFile=fopen(filename,"r");//opening the file for reading.
    if (inFile == NULL){
        printf("File could not be opened.\n");
        exit(1);
    }

	while ((c = fgetc(inFile)) != EOF) {//fgetc stops when it counters end of file(EOF)
        // Increment the count when a newline character is encountered
         if (c == '\n') {
             countlines++;
        }
    }
    countlines++;//the  crosser will not face newline for the last sequence, that is why we increment manually.

    rewind(inFile);//set the crosser at the beginning.

    for(i;i<countlines;i++)//for reading and creating linked list.
    {
        temp=(struct node*)malloc(sizeof(struct node));//allocating memory for temp
        if (temp == NULL) {
            printf("Memory allocation failed\n");
            exit(1);
        }
        temp->next=NULL;
        fscanf(inFile,"%[^\n]\n",temp->sequence);//reading the data from txt file.
        if (head == NULL) {
            head = temp;
            current = temp;
        }
        else {
            current->next = temp;//linking the nodes together
            current = current->next;//moving  current to the next node
        }
    }
    fclose(inFile);//closing the file.

	return head;
}
// i got some help from this website, it explains the algorithm and gives an example in c but different idea. https://www.programiz.com/dsa/rabin-karp-algorithm
/*
explaination of the code:
the function makes sure that it will iterate through both the array that the user gave to check the sequences
and the list taken from the txt files that will hold all the sequences, so we will have too big loops to hold the algorithm.
it will initialize some values according to the Rabin-Karp Algorithm
it will have flag to make sure that only unique sequences are counted(if a pattern appears more than once in a sequence it will be counted once)
it will count the number of times each pattern appeared through the entire list.
*/
void searchPatterns(struct node *head, struct pattern *patterns, int numberOfPatterns){

    int q = 11, d = 4, n, m, i, h, p, t0, j,s,checkflagSequence;
    struct node* currentNode = head;

    while (currentNode != NULL) {//that It goes through the entire txt file.
        n = strlen(currentNode->sequence);//taking the sequence length.

        for (i = 0; i < numberOfPatterns; i++) {//it goes through the entire given sequences to check
            m = strlen(patterns[i].p);//taking the given sequence length.
            h=(int)(pow(d,m-1)) % q;//calculating h, and i had a problem for incompatible double and int, i had to user explicit type casting.
            checkflagSequence = 0; // flag to track if pattern is found in the sequence
            p = 0;
            t0 = 0;

            for (j = 0; j < m; j++) {
                p = (d * p + patterns[i].p[j]) % q;//p[j] to check each letter of the given sequence.
                t0 = (d * t0 + currentNode->sequence[j]) % q;//sequence[j] to check each letter of each sequence in the txt file.
            }

            // compare each pattern with the sequence
            for (s = 0; s <= n - m; s++) {
                if (p == t0) {
                    int k;
                    for (k = 0; k < m; k++) {//it is p[1....m] so it needs to loop through it.
                        if (patterns[i].p[k] != currentNode->sequence[s + k]) {
                            break;
                        }
                    }
                    if (k == m) {
                        // pattern occurs with shift s
                        patterns[i].times++;//increment each time a sequence is found.
                        if(!checkflagSequence)
                        {
                            patterns[i].sequences++;// that means for specific match in specific sequence.
                            checkflagSequence=1;//that means the match has been found in that sequence and no need to count it anymore.
                        }


                    }
                }
                //updating t0 for the next iteration.
                if (s < n - m) {
                    t0 = (d * (t0 - currentNode->sequence[s] * h) + currentNode->sequence[s + m]) % q;
                    if (t0 < 0) {
                        t0 += q; // ensure t0 is non-negative according to the lecture notes
                    }
                }
            }
        }
        currentNode = currentNode->next;//going to the next line in the txt file list..
    }


}
//reference for insertion sort: https://www.geeksforgeeks.org/insertion-sort/
void sortPatterns(struct pattern *patterns, int numberOfPatterns){

	int i, j;
    struct pattern key;//declaring the key as struct pattern
    for (i = 1; i < numberOfPatterns; i++) {//going through the array of patterns.
        key = patterns[i];
        j = i - 1;

        /* Move elements of arr[0..i-1], that are
          smaller than key, to one position ahead
          of their current position */
        while (j >= 0 && patterns[j].times < key.times) {
            patterns[j + 1] = patterns[j];
            j = j - 1;
        }
        patterns[j + 1] = key;
    }

}

void printPatterns(struct pattern *patterns, int numberOfPatterns){

    int i;
	//to print the result after finding the sequences.
    for(i=0;i<numberOfPatterns;i++){
        printf("%s is detected %d times in %d DNA sequences\n",patterns[i].p,patterns[i].times,patterns[i].sequences);
    }
}

