// I used geeksforgeeks website to learn rabin-karp algorithm and also insertion sort
// https://www.geeksforgeeks.org/rabin-karp-algorithm-for-pattern-searching/
// https://www.geeksforgeeks.org/insertion-sort/

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
int NumOfLine(FILE*);// helper function to count the number of lines

int main(int argc, char *argv[]){
    char filename[81];
    int i, numberOfPatterns=0;
    struct node * head = NULL;
    struct pattern *patterns = NULL;

    printf("Enter the file path containing the DNA sequences: ");
    scanf("%s",filename);
    head = readSequences(filename);

    printf("Enter the number of patterns: ");
    scanf("%d",&numberOfPatterns);
    patterns = (struct pattern *)malloc(numberOfPatterns * sizeof(struct pattern));// dynamically allocating memory for patterns
    if (patterns == NULL) // checking if memory allocation was successful
    {
        printf("Memory allocation error.\n");
        exit(1);
    }
    for (i = 0; i < numberOfPatterns; i++)
    {
        printf("Enter the pattern: ");
        scanf("%s", patterns[i].p); // reading the patterns into the array
        patterns[i].times = 0;// initializing the other struct elements as 0
        patterns[i].sequences = 0;
    }
    searchPatterns(head,patterns,numberOfPatterns);
    sortPatterns(patterns, numberOfPatterns);
    printPatterns(patterns,numberOfPatterns);
    return 0;
}
//------------------------------------------------------------------------------------
struct node * readSequences(char filename[]){
    struct node * head = NULL, *temp, *newNode;
    FILE *inFile;
    int i, numofline;

    inFile = fopen(filename, "r");// opening the file for reading
    if (inFile == NULL)// checking if the file was opened successfully
    {
        printf("File could not be opened.\n");
        exit(1);
    }
    numofline = NumOfLine(inFile); // counting the number of lines
    rewind(inFile);

    for (i = 0; i < numofline; i++)
    {
        newNode = (struct node *)malloc(sizeof(struct node)); // dynamically allocating memory for a new node for each line in DNAsequences.txt
        if (newNode == NULL)
        {
            printf("Memory allocation error.\n");// checking if memory allocation was successful
            exit(1);
        }

        fscanf(inFile, "%[^\n]\n", newNode->sequence); // reading characters until the new line
        newNode->next = NULL;

        if (head == NULL)// if it's the first node
        {
            head = newNode; // setting head to new node
            temp = head;    // setting temp to head for subsequent nodes
        }
        else// if it's not the first node
        {
            temp->next = newNode; // linking new node to the current node
            temp = newNode;       // moving temp to new node for next iteration
        }
    }
    fclose(inFile);
    return head;
}
//------------------------------------------------------------------------------------
void searchPatterns(struct node *head, struct pattern *patterns, int numberOfPatterns){
    char *sequence ;
    struct node *temp;
    int d = 4, q = 11, i, j, s, n, m, h, p, t, isMatch;
    for (i = 0; i < numberOfPatterns; i++)
    {
        m = strlen(patterns[i].p);// using strlen to get the length of the string
        h = (int)(pow(d, m-1)) % q;// type casting so that we could do mod operation (pow returns double)
        p = 0;

        for (j = 0; j < m; j++) // calculating p
        {
            p = (d * p + patterns[i].p[j]) % q;
        }

        temp = head;// setting temp to head so that we don't lose head while traversing

        while (temp != NULL) // traversing the linked list of DNA sequences
        {
            isMatch=0; // flag for incrementing pattern->sequences
            sequence = temp->sequence;
            n = strlen(sequence);

            t = 0;
            for (j = 0; j < m; j++) // calculating t
            {
                t = (d * t + sequence[j]) % q;
            }

            for (s = 0; s <= n - m; s++) // searching for the pattern in the sequence
            {
                if (p == t)
                {
                    if (strncmp(patterns[i].p, sequence + s, m) == 0)// checking if the pattern matches the substring
                    {
                        patterns[i].times++;
                        if(!isMatch)
                        {
                            patterns[i].sequences++;
                            isMatch=1;
                        }
                    }
                }
                if (s < n - m)// calculating t for next substring
                {
                    t = (d * (t - sequence[s] * h) + sequence[s + m]) % q;
                    if (t < 0)// ensuring t remains non negative
                    {
                        t += q;
                    }
                }
            }
            temp = temp->next;
        }
    }
}
//------------------------------------------------------------------------------------
void sortPatterns(struct pattern *patterns, int numberOfPatterns){
    int i, j;
    struct pattern temp;

    for (i = 1; i < numberOfPatterns; i++)
    {
        temp = patterns[i];
        j = i - 1;

        while (j >= 0 && patterns[j].times < temp.times) // moving elements that are greater than temp to one position ahead
        {
            patterns[j + 1] = patterns[j];
            j = j - 1;
        }
        patterns[j + 1] = temp;
    }
}
//------------------------------------------------------------------------------------
void printPatterns(struct pattern *patterns, int numberOfPatterns){
    int i;
    for (i = 0; i < numberOfPatterns; i++)
    {
        printf("%s is detected %d times in %d DNA sequences\n", patterns[i].p, patterns[i].times, patterns[i].sequences);
    }

}
//------------------------------------------------------------------------------------
int NumOfLine(FILE* inFile)//helper function to count the number of lines
{
    char c;
    int count=0;
    while((c=fgetc(inFile))!=EOF)//counting the number of lines
    {
        if(c=='\n')
            count++;
    }
    count++;
    rewind(inFile);
    return count;
}