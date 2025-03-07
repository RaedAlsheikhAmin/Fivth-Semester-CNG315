#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

typedef struct graphVertex Vertex;
struct graphVertex
{
    Vertex* next;
    char team[81];
    int inDegree;
    int outDegree;
    int isVisited;
    struct graphEdge* firstArc;
};

typedef struct graphEdge Edge;
struct graphEdge
{
    Vertex* destination;
    int weight;
    Edge* nextArc;
};

typedef struct graphHead graphHead;
struct graphHead
{
    int count;
    Vertex* first;
};

graphHead* readTeams(FILE*);
void readMatches(FILE*, graphHead*);
graphHead* createGraph(void);
void createVertex(graphHead*, char[]);
void createEdge(graphHead*, char[], char[], int);
void printGraph(graphHead*);
void getMostWins(graphHead*);
void getMostLosses(graphHead*);
void getMaxGoals(graphHead*);
void getMinGoals(graphHead*);
int checkwinChain(graphHead*, char[], char[]);
int checkPath(graphHead*, char[], char[]);

int main(int argc, char *argv[]){
    FILE *inFile;
    graphHead *graph;
    inFile = fopen("teams.txt", "r");
    if (inFile == NULL){
        printf("File could not be opened.\n");
        exit(1);
    }
    printf("Teams File has been opened successfully; the graph with no edges can be seen below:\n");
    graph = readTeams(inFile);
    printGraph(graph);
    printf("***************************************************************************************************************\n");

    fclose(inFile);
    inFile = fopen("matches.txt", "r");
    if (inFile == NULL){
        printf("File could not be opened.\n");
        exit(1);
    }
    printf("Matches File has been opened successfully; the graph with edges can be seen below:\n");
    readMatches(inFile, graph);
    printGraph(graph);
    printf("***************************************************************************************************************\n");

    getMostWins(graph);
    getMostLosses(graph);
    getMaxGoals(graph);
    getMinGoals(graph);
    printf("***************************************************************************************************************\n");
    if (argc < 3){
        printf("Two teams are not given in the command line arguments!");
    }
    else{
        char str1[81], str2[81];
        int i = 0;
        while (argv[1][i] != '\0'){
            if (argv[1][i] == '_')
                str1[i] = ' ';
            else
                str1[i] = argv[1][i];
            i++;
        }
        str1[i] = '\0';
        i = 0;
        while (argv[2][i] != '\0'){
            if (argv[2][i] == '_')
                str2[i] = ' ';
            else
                str2[i] = argv[2][i];
            i++;
        }
        str2[i] = '\0';

        int result = checkwinChain(graph, str1, str2);
        if (result)
            printf("%s have beaten a team that beat %s.\n", str1, str2);

        else
            printf("%s have NOT beaten a team that beat %s.\n", str1, str2);

        int result2 = checkPath(graph, str1, str2);
        if (result2)
            printf("There is a path from %s to %s", str1, str2);
        else
            printf("There is no path from %s to %s", str1, str2);
    }
    return 0;
}

graphHead* createGraph(void){
    graphHead *graph;
    graph = (graphHead*)malloc(sizeof(graphHead));
    if(graph == NULL)
    {
        printf("Memory allocation is failed.");
        exit(1);
    }
    graph->count = 0;
    graph->first = NULL;
    return graph;
}

void createVertex(graphHead *graph, char teamName[]){
    Vertex *vertex = (struct graphVertex *) malloc(sizeof(struct graphVertex));
    vertex->next = NULL;
    strcpy(vertex->team, teamName);
    vertex->inDegree = 0;
    vertex->outDegree = 0;
    vertex->firstArc = NULL;
    vertex->isVisited = 0;

    graph->count++;

    if(graph->first == NULL){
        graph->first = vertex;
    }
    else {
        struct graphVertex *temp = graph->first;
        while(temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = vertex;
    }
}

graphHead* readTeams(FILE* inFile){

    char c;
    int numoflinesteams=0,i=0;
    while ((c = fgetc(inFile)) != EOF) {
        if (c == '\n') {
            numoflinesteams++;
        }
    }
   rewind(inFile);

   graphHead* graph = createGraph();

   char teamName[81];
   for(i;i<numoflinesteams;i++)
   {
       fscanf(inFile,"%[^\n]\n",teamName);//read up to 80 char or until new line. or I could user fgets, strtok, many other mechanism works.
       createVertex(graph,teamName);
   }

    return graph;


}

void readMatches(FILE* inFile, graphHead *graph){

    char c,dummy[81],Home[81],Away[81],FTR;
    int numoflinesmatches=0,i=0,homegoals,awaygoals;
    while ((c = fgetc(inFile)) != EOF) {
        if (c == '\n') {
            numoflinesmatches++;
        }
    }
    rewind(inFile);
    //to skip the first line of the file
    while((c=fgetc(inFile))!=EOF){
        if(c =='\n')//we consider only the first new line
            break;

    }
    for(i;i<numoflinesmatches;i++)
    {
        fscanf(inFile, "%[^;];%[^;];%[^;];%[^;];%d;%d;%[^;];%c\n", dummy, dummy, dummy, Home, &homegoals, &awaygoals, Away, &FTR);
       // printf("%s %d %d %s %c\n",Home,homegoals,awaygoals,Away,FTR);
        if(homegoals>awaygoals)//OR i could user FTR to check the condition.
            createEdge(graph,Home,Away,homegoals-awaygoals);
        else if(homegoals<awaygoals)
            createEdge(graph,Away,Home,awaygoals-homegoals);
    }


}

void createEdge(graphHead* graph, char winningTeam[], char losingTeam[], int goalDiff){

     Vertex* current = graph->first;
    Vertex* winning = NULL;
    Vertex* losing = NULL;

    // Find the winning team and losing team in the graph
    while (current != NULL && winning == NULL ) {
        if (strcmp(current->team, winningTeam) == 0) {
            winning = current;
        }
        current=current->next;
    }
    current=graph->first;//restarting the list of teams from the beginning.
    while(current != NULL && losing==NULL){
        if(strcmp(current->team,losingTeam)==0){
            losing= current;
        }

        current = current->next;
    }

    if (winning != NULL && losing != NULL) {
        // Create an edge from winning team to losing team with goal difference as weight
        Edge* newEdge = (Edge*)malloc(sizeof(Edge));
        if (newEdge == NULL) {
            printf("Memory allocation failed for newEdge in createEdge function.\n");
            exit(1);
        }

        newEdge->destination = losing;
        newEdge->weight = goalDiff;
        newEdge->nextArc = winning->firstArc;
        winning->firstArc = newEdge;

        // Update outDegree for winning team and inDegree for losing team
        winning->outDegree++;
        losing->inDegree++;
    } else {
        printf("One or both teams not found in the graph.\n");
    }

}

void printGraph(graphHead* graph){

     if (graph == NULL || graph->first == NULL) {
        printf("The graph is empty.\n");
        return;
    }

    Vertex* currentVertex = graph->first;
    while (currentVertex != NULL) {
        printf("%s -> ", currentVertex->team);

        Edge* currentEdge = currentVertex->firstArc;
        while (currentEdge != NULL) {
            printf("%s | %d  ", currentEdge->destination->team, currentEdge->weight);
            currentEdge = currentEdge->nextArc;
            if(currentEdge!=NULL)
                printf("-> ");//that we don't print extra -> after the the difference in goals.
        }
        printf("\n");
        printf("----------------------------------------------------------------------------------------------------------------------\n");


        currentVertex = currentVertex->next;
    }

}

void getMostWins(graphHead *graph){

    Vertex* current = graph->first;
    int maxWins = 0;
    char teammaxwins[81];

    while (current != NULL) {
        if (current->outDegree > maxWins) {//since the out_degree is the number of winning matches, the loop will check and compare
            maxWins = current->outDegree;
            strcpy(teammaxwins, current->team);
        }
        current = current->next;
    }

    printf("Team with the most victories:\n");
    printf("%s have won %d matches.\n", teammaxwins, maxWins);

}

void getMostLosses(graphHead *graph){

Vertex* current = graph->first;
    int maxLosses = 0;
    char teammaxlosses[81];

    while (current != NULL) {
        if (current->inDegree > maxLosses) {//since the out_degree is the number of winning matches, the loop will check and compare
            maxLosses = current->inDegree;
            strcpy(teammaxlosses, current->team);
        }
        current = current->next;
    }

    printf("Team with the most losses:\n");
    printf("%s have lost %d matches.\n", teammaxlosses, maxLosses);

}



void getMaxGoals(graphHead *graph){

    Vertex* current = graph->first;
    char teamWithMaxGoals[81];
    int maxGoalDifference = -1000;
    int hasMatches = 0; // Flag to check if any matches exist

    while (current != NULL) {
        Edge* match = current->firstArc;
        int totalGoalDifference = 0;

        while (match != NULL) {
            // Check matches won and lost
            if (match->weight > 0) {
                // Match won
                totalGoalDifference += match->weight;
            } else if (match->weight < 0) {
                // Match lost
                totalGoalDifference -= abs(match->weight);
            }
            match = match->nextArc;
        }

        if (totalGoalDifference > maxGoalDifference) {
            maxGoalDifference = totalGoalDifference;
            strcpy(teamWithMaxGoals, current->team);
        }

        if (totalGoalDifference != 0) {
            hasMatches = 1;
        }

        current = current->next;
    }

    if (hasMatches) {
        printf("Team with the highest goal difference:\n%s has a goal difference of %d.\n", teamWithMaxGoals, maxGoalDifference);
    } else {
        printf("No matches found in the graph.\n");
    }
}

void getMinGoals(graphHead *graph){

    Vertex* current = graph->first;
    char teamWithMinGoals[81];
    int minGoalDifference = 1000;

    while (current != NULL) {
        Edge* match = current->firstArc;
        int totalGoalDifference = 0;

        while (match != NULL) {
            if (match->weight > 0) {
                // Match won
                totalGoalDifference += match->weight;
            } else if (match->weight < 0) {
                // Match lost
                totalGoalDifference -= abs(match->weight);
            }
            match = match->nextArc;
        }

        if (totalGoalDifference < minGoalDifference) {
            minGoalDifference = totalGoalDifference;
            strcpy(teamWithMinGoals, current->team);
        }

        current = current->next;
    }

    if (minGoalDifference != 1000) {
        printf("Team with the lowest total goal difference:\n%s has a goal difference of %d.\n", teamWithMinGoals, minGoalDifference);
    } else {
        printf("No matches found in the graph.\n");
    }

}

int checkwinChain(graphHead *graph, char team1[], char team2[]){
    Vertex* current = graph->first;

    while (current != NULL) {
        if (strcmp(current->team, team1) == 0) {
            Edge* match = current->firstArc;

            while (match != NULL) {
                // Check if the match leads to team2 directly
                if (strcmp(match->destination->team, team2) == 0) {
                    return 1;
                }

                // Recursively check if the match leads to team2 indirectly
                if (checkwinChain(match->destination, team1, team2)) {
                    return 1;
                }

                match = match->nextArc;
            }
        }
        current = current->next;
    }

    return 0;

}


int checkPath(graphHead *graph, char team1[], char team2[]){

    // WRITE YOUR CODE HERE
}
