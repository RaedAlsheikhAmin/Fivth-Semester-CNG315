#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<ctype.h>
#include<math.h>

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
customer* createHashTable();
customer* addCustomer(customer*, customer, int, int*, int);
customer* rehash(customer*, int*, int);
void printTable(customer*, int);
void searchTable(customer*, int, char[], int);

//DO NOT CHANGE THE main FUNCTION
int main() {

    FILE *inFile;
    int no_of_customers = 0;
    customer *customers;
    customer *hashTable;

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

    hashTable = createHashTable();
    int hashTableSize = 11;
    char criteria;

    printf("Enter your hashing criteria: ");
    fflush(stdin);
    scanf("%c", &criteria);

    while (criteria != '1' && criteria != '2' && criteria != '3'){
        printf("Error! Please enter a valid hashing criteria: ");
        fflush(stdin);
        scanf("%c", &criteria);
    }
    fflush(stdin);

    int i;
    for (i = 0; i < no_of_customers; i++){
        hashTable = addCustomer(hashTable, customers[i], i, &hashTableSize, criteria - '0');
        printf("%s has been added to the table, the hash table now can be seen below:", customers[i].name);
        printTable(hashTable, hashTableSize);
    }

    char command = 0;
    int exit = 0;
    char searchName[81];
    while (exit != 1) {
        menu();
        printf("\nCommand: ");
        fflush(stdin);
        scanf("%c", &command);

        if (command == '1') {
            printf("Enter the customer's name: ");
            fflush(stdin);
            scanf("%s", searchName);
            searchTable(hashTable, hashTableSize, searchName, criteria - '0');
        }
        else if (command == '2'){
            printTable(hashTable, hashTableSize);
        }
        else if (command == '3'){
            exit = 1;
            printf("Goodbye!\n");
        }
        else{
            printf("Please enter a valid command!\n");
        }
    }
    free(customers);
    free(hashTable);
    return 0;
}

//DO NOT CHANGE THE menu FUNCTION
void menu () {
    printf("Please choose one of the following options:\n"
           "(1) Search a customer\n"
           "(2) Display hashtable\n"
           "(3) Exit\n");
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

//DO NOT CHANGE THE countCustomers FUNCTION
int countCustomers (FILE *inFile){
    int no_of_customers = 0;
    char **names;
    char **temp;
    char userName[100];
    int transactionId;
    char transactionTime[100];
    char itemDescription[100];
    int numberOfItemsPurchased;
    float costPerItem;
    char country[100];
    int exists = 0;
    char c;
    while ((c = fgetc(inFile)) != EOF) {
        if (c == '\n') {
            break;
        }
    }
    fscanf(inFile, "%[^;];%d;%[^;];%[^;];%d;%f;%[^\n]\n",
           userName, &transactionId, transactionTime,  itemDescription,
           &numberOfItemsPurchased, &costPerItem, country);
    no_of_customers++;
    names = (char**) malloc (sizeof(char*) * (no_of_customers));
    names[no_of_customers-1] = (char*) malloc(100 * sizeof (char));
    strcpy(names[no_of_customers - 1], userName);

    while (fscanf(inFile, "%[^;];%d;%[^;];%[^;];%d;%f;%[^\n]\n",
                  userName, &transactionId, transactionTime, itemDescription,
                  &numberOfItemsPurchased, &costPerItem, country) != EOF) {
        exists = 0;
        for (int i = 0; i < no_of_customers; ++i) {
            if (strncmp(names[i], userName, strlen(userName)) == 0){
                exists = 1;
                break;
            }
        }
        if (exists)
            continue;
        temp = (char**) malloc (sizeof(char*) * (no_of_customers));
        for (int i = 0; i < no_of_customers; ++i) {
            temp[i] = (char*) malloc(100 * sizeof (char));
            strcpy(temp[i], names[i]);
        }
        free(names);

        names = (char**) malloc (sizeof(char*) * (no_of_customers + 1));
        for (int i = 0; i < no_of_customers; ++i) {
            names[i] = (char*) malloc(100 * sizeof (char));
            strcpy(names[i], temp[i]);
        }
        free(temp);
        names[no_of_customers] = (char*) malloc(100 * sizeof (char));
        strcpy(names[no_of_customers], userName);
        no_of_customers++;

    }
    return no_of_customers;
}

//DO NOT CHANGE THE readTransactions FUNCTION
void readTransactions (FILE* inFile, customer* customers, int no_of_customers) {
    char userName[100];
    int transactionId;
    char transactionTime[100];
    char itemDescription[100];
    int numberOfItemsPurchased;
    float costPerItem;
    char country[100];
    int exists = 0;
    int filled = 0;
    char c;

    for (int i = 0; i < no_of_customers; ++i) {
        strcpy(customers[i].name, "unassigned");
        customers[i].amount_paid = 0;
        customers[i].items_purchased = 0;
        customers[i].transactions = 0;
    }

    while ((c = fgetc(inFile)) != EOF) {
        if (c == '\n') {
            break;
        }
    }

    while (fscanf(inFile, "%[^;];%d;%[^;];%[^;];%d;%f;%[^\n]\n",
                  userName, &transactionId, transactionTime, itemDescription,
                  &numberOfItemsPurchased, &costPerItem, country) != EOF) {
        exists = 0; // n
        int i; // n
        for (i = 0; i < no_of_customers; ++i) {
            if(strcmp(userName, customers[i].name) == 0){
                exists = 1;
                break;
            }
        }
        if (exists){
            customers[i].transactions ++;
            customers[i].items_purchased += numberOfItemsPurchased;
            customers[i].amount_paid += (numberOfItemsPurchased * costPerItem);
        }
        else{
            strcpy(customers[filled].name, userName);
            customers[filled].transactions ++;
            customers[filled].items_purchased += numberOfItemsPurchased;
            customers[filled].amount_paid += (numberOfItemsPurchased * costPerItem);
            filled++;
        }
    }
}
/*creating HashTable with specified tablesize and dynamic allocation using malloc.
then initializing the data to 0 or unassigned*/
customer* createHashTable(){
    customer *hashTable;
    int TableSize=11;
    int i;
    hashTable=(customer*)malloc(sizeof(customer)*TableSize);
    if(hashTable==NULL)
    {
        printf("Memory is not allocated");
        exit(1);
    }
    for(i=0;i<TableSize;i++)
    {
        hashTable[i].amount_paid=0.0;
        hashTable[i].items_purchased=0;
        hashTable[i].transactions=0;
        strcpy(hashTable[i].name,"unassigned");

    }
    return hashTable;

}
/*
it will add an customer according to the given criteria.
the key will be calculated then according to the key the spot in the hashtable will be checked.
*/
customer* addCustomer(customer* hashTable, customer addedCustomer, int hashTableCapacity, int *hashTableSize, int criteria){
    if(hashTable==NULL)
    {
        printf("memory was not allocated correctly!");
        exit(1);
    }
int key=abs(tolower(addedCustomer.name[strlen(addedCustomer.name)-1])-tolower(addedCustomer.name[0]));
int i,function,hashfull;
int hash=key%(*hashTableSize);
int hash2=7-(key%7);
switch (criteria){
case 1:
    for(i=0;i<*hashTableSize;i++)
{
    function=i;
    hashfull=(hash+function)%(*hashTableSize);
    if(strcmp(hashTable[hashfull].name,"unassigned")==0)
    {
        hashTable[hashfull].amount_paid=addedCustomer.amount_paid;
        hashTable[hashfull].items_purchased=addedCustomer.items_purchased;
        hashTable[hashfull].transactions=addedCustomer.transactions;
        strcpy(hashTable[hashfull].name,addedCustomer.name);
        break;//no need to continue if we added the customer already to hashtable.
    }

}
    break;

case 2:
    for(i=0;i<*hashTableSize;i++)
{
    function=i*i;
    hashfull=(hash+function)%(*hashTableSize);
    if(strcmp(hashTable[hashfull].name,"unassigned")==0)
    {
        hashTable[hashfull].amount_paid=addedCustomer.amount_paid;
        hashTable[hashfull].items_purchased=addedCustomer.items_purchased;
        hashTable[hashfull].transactions=addedCustomer.transactions;
        strcpy(hashTable[hashfull].name,addedCustomer.name);
        break;//no need to continue if we added the customer already to hashtable.
    }


}

    break;
case 3:
     for(i=0;i<*hashTableSize;i++)
{

    function=i*hash2;
    hashfull=(hash+function)%(*hashTableSize);
    if(strcmp(hashTable[hashfull].name,"unassigned")==0)
    {
        hashTable[hashfull].amount_paid=addedCustomer.amount_paid;
        hashTable[hashfull].items_purchased=addedCustomer.items_purchased;
        hashTable[hashfull].transactions=addedCustomer.transactions;
        strcpy(hashTable[hashfull].name,addedCustomer.name);
        break;//no need to continue if we added the customer already to hashtable.
    }

}
    break;
default:
        printf("Invalid criteria.\n");
}

float loadfactor=(hashTableCapacity*1.0)/(*hashTableSize*1.0);
if(loadfactor>0.5)//to check the load factor, and do rehashing accordingly.
{
    printf("\nRehashing is needed!\n");
    hashTable=rehash(hashTable,hashTableSize,criteria);//calling rehash since the loadfactor is bigger than the efficient ratio.

    }
return hashTable;

}
/*
rehash function which will take the old hash table, and the old size, and calculate the new size by multiplying the size by 2 and rounding it to the next prime number using the helper function isprime.
again the criteria will be called as well and according to the criteria the rehashing will be done.
the newtable will be initialized to 0 values that it will be easier to calculate later.
two loops will handling doing the rehashing, the first one will check if the oldtable has a name or not, if it has a name, an inner loop will be activated to do the rehashing by checking if it is empty or not.

*/

customer* rehash(customer* hashTable, int *hashTableSize, int criteria){
    int newhashtablesize = (*hashTableSize) * 2; // Double the old size
    int i,hash2;

    while (!isPrime(newhashtablesize)) {
        newhashtablesize++; // Round it to the next prime number
    }

    customer* newhashTable = (customer*)malloc(sizeof(customer) * newhashtablesize);
    if (newhashTable == NULL) {
        printf("Cannot allocate memory to the new hash table\n");
        exit(1);
    }

    for (i = 0; i < newhashtablesize; i++) {
        newhashTable[i].amount_paid = 0.0;
        newhashTable[i].items_purchased = 0;
        newhashTable[i].transactions = 0;
        strcpy(newhashTable[i].name, "unassigned");
    }

    int key, function, hashfull, hash;

    switch (criteria) {
        case 1:
            for (i = 0; i < (*hashTableSize); i++) {
                if (strcmp(hashTable[i].name, "unassigned") != 0) {
                    key = abs(tolower(hashTable[i].name[strlen(hashTable[i].name) - 1]) - tolower(hashTable[i].name[0]));
                    function = 0;
                    hash = key % newhashtablesize;
                    hashfull = hash; // At the beginning f(0) is 0;

                    while (strcmp(newhashTable[hashfull].name, "unassigned") != 0) {
                        function++;
                        hashfull = (hash + function) % newhashtablesize;
                    }

                    newhashTable[hashfull].amount_paid = hashTable[i].amount_paid;
                    newhashTable[hashfull].items_purchased = hashTable[i].items_purchased;
                    newhashTable[hashfull].transactions = hashTable[i].transactions;
                    strcpy(newhashTable[hashfull].name, hashTable[i].name);
                }
            }
            break;

        case 2:
             for (i = 0; i < (*hashTableSize); i++) {
                if (strcmp(hashTable[i].name, "unassigned") != 0) {
                    key = abs(tolower(hashTable[i].name[strlen(hashTable[i].name) - 1]) - tolower(hashTable[i].name[0]));
                    function = 0;
                    hash = key % newhashtablesize;
                    hashfull = hash; // At the beginning f(0) is 0;

                    while (strcmp(newhashTable[hashfull].name, "unassigned") != 0) {
                        function=(function+1)*(function+1);
                        hashfull = (hash + function) % newhashtablesize;
                    }

                    newhashTable[hashfull].amount_paid = hashTable[i].amount_paid;
                    newhashTable[hashfull].items_purchased = hashTable[i].items_purchased;
                    newhashTable[hashfull].transactions = hashTable[i].transactions;
                    strcpy(newhashTable[hashfull].name, hashTable[i].name);
                }
            }
            break;
        case 3:
            for (i = 0; i < (*hashTableSize); i++) {
                if (strcmp(hashTable[i].name, "unassigned") != 0) {
                    key = abs(tolower(hashTable[i].name[strlen(hashTable[i].name) - 1]) - tolower(hashTable[i].name[0]));
                    function = 0;
                    hash = key % newhashtablesize;
                    hash2=7-(key%7);
                    hashfull = hash; // At the beginning f(0) is 0;

                    while (strcmp(newhashTable[hashfull].name, "unassigned") != 0) {
                        function++;
                        hashfull = (hash + function*hash2) % newhashtablesize;
                    }

                    newhashTable[hashfull].amount_paid = hashTable[i].amount_paid;
                    newhashTable[hashfull].items_purchased = hashTable[i].items_purchased;
                    newhashTable[hashfull].transactions = hashTable[i].transactions;
                    strcpy(newhashTable[hashfull].name, hashTable[i].name);
                }
            }
            break;
        default:
            printf("Invalid criteria.\n");
    }



    *hashTableSize = newhashtablesize;//the the old size will refer to the new size.
    free(hashTable);
    return newhashTable;
}
/*
a normal print function that will loop the hashtable and print the info.
*/
void printTable(customer *hashTable, int hashTableSize) {
    printf("\n***************************************************************************\n");
    printf("Index     Name         Transactions    Items    Paid_Amount\n");

    for(int i = 0; i < hashTableSize; i++) {
        printf("%-8d  %-14s  %-14d  %-8d  %.2f\n", i, hashTable[i].name, hashTable[i].transactions, hashTable[i].items_purchased, hashTable[i].amount_paid);
    }
    printf("***************************************************************************\n");
}
/*
it will hash create a key out of the name and hash it, if the name is not found and it is not unassigned, that means we go for linear hashing in criteria one.
same goes with the other criteria, but using quadratic and double hashing search.
*/
void searchTable(customer *hashTable, int hashTableSize, char searchName[], int criteria){

    int key, function, hashfull, hash, hash2;

    switch(criteria) {
        case 1: // Linear Probing to find the specified name
            key = abs(tolower(searchName[strlen(searchName) - 1]) - tolower(searchName[0]));
            hash = key % hashTableSize;
            hashfull = hash;//at the beggining no need for f(i)
            while (strcmp(hashTable[hashfull].name, "unassigned") != 0 && strcmp(hashTable[hashfull].name, searchName) != 0) {
                hashfull = (hashfull + 1) % hashTableSize;
                if (hashfull == hash) {
                    printf("Customer '%s' not found.\n", searchName);
                }
            }
            break;

        case 2: // Quadratic Probing
            key = abs(tolower(searchName[strlen(searchName) - 1]) - tolower(searchName[0]));
            hash = key % hashTableSize;
            function = 0;
            hashfull = hash;
            while (strcmp(hashTable[hashfull].name, "unassigned") != 0 && strcmp(hashTable[hashfull].name, searchName) != 0) {
                function++;
                hashfull = (hash + function * function) % hashTableSize;
                if (hashfull == hash) {
                    printf("Customer '%s' not found.\n", searchName);
                }
            }
            break;

        case 3: // Double Hashing
            key = abs(tolower(searchName[strlen(searchName) - 1]) - tolower(searchName[0]));
            hash = key % hashTableSize;
            hash2 = 7 - (key % 7);
            hashfull = hash;
            function=1;
            while (strcmp(hashTable[hashfull].name, "unassigned") != 0 && strcmp(hashTable[hashfull].name, searchName) != 0) {
                hashfull = (hash + function*hash2) % hashTableSize;
                function++;
                if (hashfull == hash) {
                    printf("Customer '%s' not found.\n", searchName);
                }
            }
            break;

        default:
            printf("Invalid criteria.\n");
    }

    if (strcmp(hashTable[hashfull].name, searchName) == 0) {//that means we found the name in the table according to the provided hashfunction.
        printf("Information for customer %s:\n", searchName);
        printf("\tNumber of transactions = %d\n", hashTable[hashfull].transactions);
        printf("\tItems purchased = %d\n", hashTable[hashfull].items_purchased);
        printf("\tTotal Amount Paid = %.2f\n", hashTable[hashfull].amount_paid);
    } else {
        printf("User does not exist in the table!\n", searchName);
    }
}


int isPrime(int n) {//to check if the number is prime or not
    if (n <= 1) {
        return 0; // Numbers less than or equal to 1 are not prime
    }
    for (int i = 2; i <= sqrt(n); i++) {//according to algorithm we took in cng 140, we check only the square root of the number.
        if (n % i == 0) {
            return 0; // Not a prime number
        }
    }
    return 1; // Prime number
}

