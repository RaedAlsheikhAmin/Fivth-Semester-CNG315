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
/*
it will loop through the file to count the number of line until the End_Of_File then set the crosser at the beginning
then it will create the graph head, after that using the loop it will go through the file until numoflines, and read the teams and set them to the graph using createVertex.
*/
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
   for(;i<numoflinesteams;i++)
   {
       fscanf(inFile,"%[^\n]\n",teamName);//read up to 80 char or until new line. or I could user fgets, strtok, many other mechanism works.
       createVertex(graph,teamName);
   }

    return graph;


}
/*
it will count the number of lines of the file, then set the crosser to the beginning, after that it will skip the first line using the loop until reaching end of line.
then it will read the data of the file using fscanf that reads and skips  ';', and according to the winner it will call createEdge function to set the edges.
*/

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
    for(;i<numoflinesmatches;i++)
    {
        fscanf(inFile, "%[^;];%[^;];%[^;];%[^;];%d;%d;%[^;];%c\n", dummy, dummy, dummy, Home, &homegoals, &awaygoals, Away, &FTR);
       // printf("%s %d %d %s %c\n",Home,homegoals,awaygoals,Away,FTR);
        if(homegoals>awaygoals)//OR i could user FTR to check the condition.
            createEdge(graph,Home,Away,homegoals-awaygoals);
        else if(homegoals<awaygoals)
            createEdge(graph,Away,Home,awaygoals-homegoals);
    }


}
/*
Locate winning and losing teams in the graph
Create an edge from the winning team to the losing team with the goal difference as weight
Update outDegree for winning team and inDegree for losing team

*/
void createEdge(graphHead* graph, char winningTeam[], char losingTeam[], int goalDiff){
Vertex* current = graph->first;
    Vertex* winning = NULL;
    Vertex* losing = NULL;

    // Find the winning team and set it.
    while (current != NULL && winning==NULL) {
        if (strcmp(current->team, winningTeam) == 0) {
            winning = current;
        }

        current = current->next;
    }
    current=graph->first;//restarting the list of teams from the beginning.
    while(current != NULL && losing==NULL){//find the losing team and set it
        if(strcmp(current->team,losingTeam)==0){
            losing= current;
        }
        current = current->next;
    }

    if (winning != NULL && losing != NULL) {//that means both teams are found.
        // Create an edge from winning team to losing team with goal difference as weight
        Edge* newEdge = (Edge*)malloc(sizeof(Edge));//create an edge
        if (newEdge == NULL) {//checking for memory allocation
            printf("Memory allocation failed for newEdge in createEdge function.\n");
            exit(1);
        }

        newEdge->destination = losing;//connecting the edge->destination to the losing team.
        newEdge->weight = goalDiff;//the weight of the edge will be the goal difference.
        newEdge->nextArc = NULL; // Initialize nextArc as NULL for the new edge

        // if winning team has no existing edges, set new edge as firstArc
        if (winning->firstArc == NULL) {
            winning->firstArc = newEdge;
        } else {
            // Traverse to the end of the edge list of the winning team
            Edge* temp = winning->firstArc;
            while (temp->nextArc != NULL) {
                temp = temp->nextArc;
            }
            temp->nextArc = newEdge; // add new edge at the end
        }

        // Update outDegree for winning team and inDegree for losing team
        winning->outDegree++;
        losing->inDegree++;
    } else {//if one of the team is missing from the data.
        printf("One or both teams not found in the graph.\n");
    }

}
/*
Iterate through the vertices (teams) in the graph
Print each team and its connected teams with respective goal differences
*/
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
/*
Find the team with the highest outDegree (most victories) in the graph
Display the team name and the number of victories
*/
void getMostWins(graphHead *graph){

    Vertex* current = graph->first;
    int maxWins = 0;
    char teammaxwins[81];

    while (current != NULL) {
        if (current->outDegree > maxWins) {//since the out_degree is the number of winning matches, the loop will check and compare
            maxWins = current->outDegree;
            strcpy(teammaxwins, current->team);//storing the teamname that it can be printd it later.
        }
        current = current->next;//iterating through the teams
    }

    printf("\nTeam with the most victories:\n");
    printf("%s have won %d matches.\n", teammaxwins, maxWins);

}
/*
Find the team with the highest inDegree (most losses) in the graph
Display the team name and the number of losses
*/
void getMostLosses(graphHead *graph){

Vertex* current = graph->first;
    int maxLosses = 0;
    char teammaxlosses[81];

    while (current != NULL) {
        if (current->inDegree > maxLosses) {//since the in_degree is the number of losing matches, the loop will check and compare
            maxLosses = current->inDegree;
            strcpy(teammaxlosses, current->team);
        }
        current = current->next;//iterating through the teams
    }

    printf("\nTeam with the most losses:\n");
    printf("%s have lost %d matches.\n", teammaxlosses, maxLosses);

}



void getMaxGoals(graphHead *graph){

   // Initialize variables for tracking the team with the maximum goal difference
int maxGoalDiff = -100;
char teamMaxGoalDiff[81];

// Pointers for traversing vertices and edges
Vertex *currentV = graph->first;
Vertex *compareV;
Edge *currentE;

// Iterate through each team
while (currentV != NULL) {
    // Accumulators for goals made and goals taken
    int goalsScored = 0;
    int goalsConceded = 0;

    // Store the current team's name
    char currentTeamName[81];
    strcpy(currentTeamName, currentV->team);

    // Calculate goals made by the current team
    currentE = currentV->firstArc;
    while (currentE != NULL) {
        goalsScored += currentE->weight;
        currentE = currentE->nextArc;
    }

    // Reset compareV for each iteration
    compareV = graph->first;

    // Compare with other teams to calculate goals taken
    while (compareV != NULL) {
        currentE = compareV->firstArc;
        while (currentE != NULL) {
            if (strcmp(currentTeamName, currentE->destination->team) == 0) {
                goalsConceded += currentE->weight;
            }
            currentE = currentE->nextArc;//traversing the edges
        }
        compareV = compareV->next;//next compare team
    }

    // Calculate the total goal difference for the current team
    int totalGoalDiff = goalsScored - goalsConceded;

    // Check for the team with the highest goal difference
    if (totalGoalDiff > maxGoalDiff) {
        maxGoalDiff = totalGoalDiff;
        strcpy(teamMaxGoalDiff, currentTeamName);
    }

    // Move to the next team
    currentV = currentV->next;
}

// Display the team with the highest goal difference
printf("\nTeam with the highest goal difference:\n%s has a goal difference of %d.\n\n", teamMaxGoalDiff, maxGoalDiff);

}

void getMinGoals(graphHead *graph){

   // Initialize variables for tracking the team with the minimum goal difference
int minGoalDiff = 100;
char teamMinGoalDiff[81];

// Pointers for traversing vertices and edges
Vertex *currentV = graph->first;
Vertex *compareV;
Edge *currentE;

// Iterate through each team
while (currentV != NULL) {
    // Accumulators for goals made and goals taken
    int goalsScored = 0;
    int goalsConceded = 0;

    // Store the current team's name
    char currentTeamName[81];
    strcpy(currentTeamName, currentV->team);

    // Calculate goals made by the current team
    currentE = currentV->firstArc;
    while (currentE != NULL) {
        goalsScored += currentE->weight;
        currentE = currentE->nextArc;
    }

    // Reset compareV for each iteration
    compareV = graph->first;

    // Compare with other teams to calculate goals taken
    while (compareV != NULL) {
        currentE = compareV->firstArc;
        while (currentE != NULL) {
            if (strcmp(currentTeamName, currentE->destination->team) == 0) {
                goalsConceded += currentE->weight;
            }
            currentE = currentE->nextArc;
        }
        compareV = compareV->next;
    }

    // Calculate the total goal difference for the current team
    int totalGoalDiff = goalsScored - goalsConceded;

    // Check for the team with the minimum goal difference
    if (totalGoalDiff < minGoalDiff) {
        minGoalDiff = totalGoalDiff;
        strcpy(teamMinGoalDiff, currentTeamName);
    }

    // Move to the next team
    currentV = currentV->next;
}

// Display the team with the minimum goal difference
printf("Team with the lowest goal difference:\n%s has a goal difference of %d.\n", teamMinGoalDiff, minGoalDiff);


}
/*
Check if there's a chain of wins between team1 and team2
Return 1 if there's a win chain, otherwise return 0
*/
int checkwinChain(graphHead *graph, char team1[], char team2[]){
    // Check if the graph or team names are invalid
    if (graph == NULL || team1 == NULL || team2 == NULL) {
        return 0; // Invalid inputs, no win chain possible
    }

    // Initialize a pointer to traverse through the graph vertices (teams)
    Vertex* current;
    current = graph->first; // Start by pointing to the first vertex (team) in the graph

    // Iterate through each team in the graph
    while (current != NULL) {
        // Check if the current team matches team1
        if (strcmp(current->team, team1) == 0) {
            // If team1 is found, start examining its connected edges (matches)
            Edge* match = current->firstArc; // Set the edge to the first arc of team1

            // Traverse through the connected matches of team1
            while (match != NULL) {
                // Check if the current match leads directly to team2
                if (strcmp(match->destination->team, team2) == 0) {
                    return 1; // Direct win chain found from team1 to team2
                }

                // Recursively check if the match leads indirectly to team2
                // by exploring subsequent matches from the destination team of the current match
                if (checkwinChain(graph, match->destination->team, team2)) {
                    return 1; // Indirect win chain found from team1 to team2
                }

                match = match->nextArc; // Move to the next connected match (arc) of team1
            }
        }
        current = current->next; // Move to the next team in the graph
    }

    return 0; // No win chain found from team1 to team2
}


int checkPath(graphHead* graph, char team1[], char team2[]) {
    // Find the vertices in the graph corresponding to team1 and team2
    Vertex* vt = graph->first;
    // Initialize pointers to track the start and end vertices
    Vertex* startVertex = NULL;
    Vertex* endVertex = NULL;

    while (vt != NULL) {
        // Check if the current vertex matches team1 or team2
        if (strcmp(vt->team, team1) == 0) {
            startVertex = vt; // Assign the start vertex
        }
        if (strcmp(vt->team, team2) == 0) {
            endVertex = vt; // Assign the end vertex
        }

        vt = vt->next; // Move to the next vertex in the graph
    }

    // Check if either the start or end vertex is missing in the graph
    if (startVertex == NULL || endVertex == NULL) {
        // One or both teams not found in the graph, hence no path exists
        return 0;
    }

    // Reset the visited flags of all vertices before starting DFS
    vt = graph->first;
    while (vt != NULL) {
        vt->isVisited = 0; // Reset the 'isVisited' flag for all vertices
        vt = vt->next;
    }

    // Perform Depth-First Search (DFS) to check for a path from team1 to team2
    return DepthFirstTraversal(graph, startVertex, endVertex);
}
/*
DepthFirstTraversal (Helper function for checkPath)
graph: Pointer to the graph structure containing team information
current: Pointer to the current vertex being checked during DFS
target: Pointer to the destination vertex for the path check
 Returns: 1 if a path is found from 'current' vertex to 'target' vertex, 0 otherwise
 */
int DepthFirstTraversal(graphHead* graph, Vertex* current, Vertex* target) {
    // Check if the current vertex is the target vertex
    if (current == target) {
        // Found the target, there exists a path
        return 1;
    }

    current->isVisited = 1; // Mark the current vertex as visited during traversal

    Edge* edge = current->firstArc; // Access the first connected edge of the current vertex
    while (edge != NULL) {
        Vertex* nextVertex = edge->destination; // Pointer to the next connected vertex
        if (!nextVertex->isVisited) {
            // Check if the next vertex has not been visited yet
            if (DepthFirstTraversal(graph, nextVertex, target)) {
                return 1; // Path found by recursively traversing to the next vertex
            }
        }
        edge = edge->nextArc; // Move to the next connected edge of the current vertex
    }

    return 0; // No path found from the current vertex to the target vertex
}
