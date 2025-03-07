#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//DO NOT CHANGE DATA STRUCTURE
struct Customer {
    char name[81];
    int transactions;
    int items_purchased;
    float amount_paid;
};

typedef struct Customer customer;

//DO NOT CHANGE FUNCTION PROTOTYPES
void menu();
void printCustomers(customer *, int);
int countCustomers(FILE*);
void readTransactions(FILE*, customer* ,int);
void heapify(customer*, int, int, int);
void heapSort(customer*, int,int);
void swap(customer* , customer* );//extra helper function to swap the info of customer


//DO NOT CHANGE THE main FUNCTION
int main() {
    FILE *inFile;
    int no_of_customers = 0;
    customer *customers;

    inFile = fopen("transactions.txt", "r");
    if (inFile == NULL){
        printf("File could not be opened.\n");
        exit(1);
    }
    no_of_customers = countCustomers(inFile);
    printf("There are %d unique customers\n", no_of_customers);

    customers = (customer*) malloc(no_of_customers * sizeof (customer));
    fclose(inFile);
    inFile = fopen("transactions.txt", "r");
    if (inFile == NULL){
        printf("File could not be opened.\n");
        exit(1);
    }

    readTransactions(inFile, customers, no_of_customers);
    if (customers == NULL) {
        printf("\nCustomers have NOT been read successfully!\n");
        exit(1);
    }
    printCustomers(customers, no_of_customers);

    char command = 0;
    int exit = 0;
    while (exit != 1) {
        menu();
        printf("\nCommand: ");
        fflush(stdin);
        scanf("%c", &command);

        if (command == '1' || command == '2' || command == '3' || command == '4') {
            heapSort(customers, no_of_customers, command - '0');
            printCustomers(customers, no_of_customers);
        }
        else if (command == '5'){
            exit = 1;
            printf("Goodbye!\n");
        }
        else{
            printf("Please enter a valid command!\n");
        }
    }
    free(customers);
    return 0;
}

//DO NOT CHANGE THE menu FUNCTION
void menu () {
    printf("Please choose one of the following options:\n"
           "(1) Display customers sorted by number of transactions\n"
           "(2) Display customers sorted by number of items purchased\n"
           "(3) Display customers sorted by total amount paid\n"
           "(4) Display customers sorted by average amount paid per transaction\n"
           "(5) Exit\n");
}

//DO NOT CHANGE THE printCustomers FUNCTION

void printCustomers (customer* customers, int no_of_customers) {
    printf("List of customers:\n");
    int i;
    for (i = 0; i < no_of_customers; ++i) {
        printf("Name: %s, number of transactions = %d, number of items = %d, total amount paid = %.2f,"
               " average amount paid per transaction = %.2f\n",
               customers[i].name, customers[i].transactions, customers[i].items_purchased, customers[i].amount_paid,
               customers[i].amount_paid/customers[i].transactions);
    }
}

int countCustomers (FILE *inFile){
    char c,**name,tempname[8];
    int countlines=0,i=0,max_col=8,j=0,uniquecust=0;

    inFile=fopen("transactions.txt","r");
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
    //allocating memory to the multi-dimension name array of char.
    name=(char**)malloc(sizeof(char*)*(countlines));
    for(i;i<countlines;i++)//allocating memory to the array of char.
        name[i]=(char*)malloc(sizeof(char)*max_col);
    if(name==NULL){
        printf("memory is not allocated correctly");
        exit(1);
        }
    rewind(inFile);//to get the crosser to the beginning of the file.
    //to skip the first line of the file
    while((c=fgetc(inFile))!=EOF){
        if(c =='\n')//we consider only the first new line
            break;

    }
    for(i=0;i<countlines;i++)//looping the entire file and checking for duplicates and counting them.
    {
        fscanf(inFile,"%[^;];%*[^\n]\n",tempname);//reading from the file only the name and skipping the rest of the line.
        int isDuplicate = 0;//flag to raise when duplication appears.
        for (j = 0; j < uniquecust; j++) {
            if (strcmp(name[j], tempname) == 0) {//check if the name is already there or not, if the name is there, I just skip.
                isDuplicate = 1;
                break;
            }
        }

        // If it's not a duplicate, add it to the array and increase unique customer counter.
        if (!isDuplicate) {
            strcpy(name[uniquecust], tempname);
            uniquecust++;
    }

    }

    fclose(inFile);
    return uniquecust;

}

void readTransactions (FILE* inFile, customer* customers, int no_of_customers) {

    char c,tempname[8],faketoskip[100];
    int countlines=0,i=0,j=0,uniquecust=0,NumberOfItemsPurchased=0;
    float CostPerItem=0.0,amountPaid=0.0;


    inFile=fopen("transactions.txt","r");
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

    rewind(inFile);//to get the crosser to the beginning of the file.
    //to skip the first line of the file
    while((c=fgetc(inFile))!=EOF){
        if(c =='\n')//we consider only the first new line
            break;

    }
    /*this loop will traverse the entire txt file that it can manage all the process inside it
    then there is fscanf that will take the important parts of the file and skip the others using %[^;] to a dummy varriable.
    then we have a flag that checks for dulpicate and it is 0 at the beginning that means no duplicate.
    the second loop will check for uniqueness, again. it will start from j=0
    the if condition will compare tempname that reads the file with my structure customer array,
    if they are the same, that means duplicate and will overwrite the previous values, other wise
        it will go to the second condition and add new values to customer structure array.
    */
    for(i=0;i<countlines;i++)
    {
        fscanf(inFile,"%[^;];%[^;];%[^;];%[^;];%d;%f;%*[^\n]\n",tempname,faketoskip,faketoskip,faketoskip,&NumberOfItemsPurchased,&CostPerItem);
        amountPaid=NumberOfItemsPurchased*CostPerItem;
        int isDuplicate = 0;//flag to know duplication.
        for (j = 0; j < uniquecust; j++) {
            if (strcmp(customers[j].name, tempname) == 0) {//check if the name is already there or not
                isDuplicate = 1;
                customers[j].transactions++;
                customers[j].items_purchased+=NumberOfItemsPurchased;
                customers[j].amount_paid+=amountPaid;
                break;
            }
        }

        // If it's not a duplicate, add it to the array
        if (!isDuplicate) {//this will be done for one time for each customer.
            strcpy(customers[uniquecust].name, tempname);
            customers[uniquecust].transactions=1;
            customers[uniquecust].items_purchased=NumberOfItemsPurchased;
            customers[uniquecust].amount_paid=amountPaid;
            uniquecust++;
    }

    }

    fclose(inFile);


}

/*
https://www.boardinfinity.com/blog/heap-sort-algorithm/, I used this link to understand how the algorithm works and how to apply it in C.
*/
void heapify(customer* customers, int no_of_customers, int i, int criteria) {
    // let's assume that current root is largest
    int largest = i;
    // initialize left and right child of the root element (left and right denotes the position of children)
    int left= 2*i+1;
    int right= 2*i+2;

switch(criteria){//switch statement to handle the criteria that the user wants to sort according to.
case 1:

    //find the largest among root, left and right child

    if(left<no_of_customers && customers[left].transactions>customers[largest].transactions){
        largest=left;
    }
    if(right<no_of_customers && customers[right].transactions>customers[largest].transactions){
        largest=right;
    }
    // swap the root(current element) with the largest root
    if(i!=largest ){
        swap(&customers[i], &customers[largest]);
        //heapify the new root again to get maximum on the top position
        heapify(customers, no_of_customers, largest,criteria);
    }
    break;

case 2:
     if(left<no_of_customers && customers[left].items_purchased>customers[largest].items_purchased){
        largest=left;
    }
    if(right<no_of_customers && customers[right].items_purchased>customers[largest].items_purchased){
        largest=right;
    }
    // swap the root(current element) with the largest root
    if(i!=largest ){
        swap(&customers[i], &customers[largest]);
        //heapify the new root again to get maximum on the top position
        heapify(customers, no_of_customers, largest,criteria);
    }
    break;
case 3:
    if(left<no_of_customers && customers[left].amount_paid>customers[largest].amount_paid){
        largest=left;
    }
    if(right<no_of_customers && customers[right].amount_paid>customers[largest].amount_paid){
        largest=right;
    }
    // swap the root(current element) with the largest root
    if(i!=largest ){
        swap(&customers[i], &customers[largest]);
        //heapify the new root again to get maximum on the top position
        heapify(customers, no_of_customers, largest,criteria);
    }
    break;
case 4:
    if(left<no_of_customers && (customers[left].amount_paid/customers[left].transactions)>(customers[largest].amount_paid/customers[largest].transactions)){
        largest=left;
    }
    if(right<no_of_customers && (customers[right].amount_paid/customers[right].transactions)>(customers[largest].amount_paid/customers[largest].transactions)){
        largest=right;
    }
    // swap the root(current element) with the largest root
    if(i!=largest ){
        swap(&customers[i], &customers[largest]);
        //heapify the new root again to get maximum on the top position
        heapify(customers, no_of_customers, largest,criteria);
    }
    break;
}
}

void heapSort(customer* customers, int no_of_customers, int criteria) {


        //build the max heap using heapify
        //always start from ((size/2)-1)th node
            for(int i= (no_of_customers/2)-1;i>=0;i--){
            heapify(customers, no_of_customers, i, criteria);
    }

        //extract elements from heap and store at last position of heap i.e. arr[0]
            for(int i=no_of_customers-1;i>=1;i--){
                swap(&customers[0], &customers[i]);
        //heapify the root element again to get maximum element on top
                heapify(customers, i, 0,criteria );

            }

}
void swap(customer *customer1, customer *customer2){//swap helper to help me to swap the values.
    customer temp1=*customer1;
   *customer1=*customer2;
   *customer2=temp1;




}
