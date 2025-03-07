#include<stdio.h>
#include<stdlib.h>
#include<string.h>

//Name: Shayan Nadeem Bhutta
//Student_ID: 2542413

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
int DFS(graphHead*, Vertex*, Vertex*); //Helper function to perform Depth-First-Traversal of the Graph.

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
	
	int i;
	char c;
	char name[81]; //81 size as declared in the given structure...not assumed.

	graphHead *graph = NULL;
	graph = createGraph();
	
    while (fscanf(inFile, "%80[^\n]\n", name) != EOF) { //%80 To make sure any spaces in the team names do not cause a problem.
    		createVertex(graph, name);
	}
		
	return graph;
}

void readMatches(FILE* inFile, graphHead *graph){
    
    int seasonEndYear, week, homeGoals, awayGoals, goalDiff;
	char date[20], homeTeam[81], awayTeam[81], result, winningTeam[81], losingTeam[81];
	
	fscanf(inFile, "%*[^\n]%*c"); //Skip the first header line.
	
 	while (fscanf(inFile, "%d;%d;%[^;];%[^;];%d;%d;%[^;];%c", &seasonEndYear, &week, date, homeTeam, &homeGoals, &awayGoals, awayTeam, &result) != EOF) {
 		goalDiff = abs(homeGoals - awayGoals);
		if (result != 'D') {
 			if (homeGoals > awayGoals) {
 				strcpy(winningTeam, homeTeam);
 				strcpy(losingTeam, awayTeam);
		 	}
		 	else {
 				strcpy(winningTeam, awayTeam);
 				strcpy(losingTeam, homeTeam);
			 }
			createEdge(graph, winningTeam, losingTeam, goalDiff);
		}
	}
}

void createEdge(graphHead* graph, char winningTeam[], char losingTeam[], int goalDiff) {
    Vertex* vtempptr = graph->first;
    Edge* gtempptr = NULL;
    int winningfoundflag = 0;
    int losingfoundflag = 0;

    // Find the winning team vertex
    while (winningfoundflag != 1) {
        if (strcmp(vtempptr->team, winningTeam) == 0) {
            winningfoundflag = 1;
        } else {
            vtempptr = vtempptr->next;
        }
    }
	
	vtempptr->outDegree++;
	
    // Initialize gtempptr to the firstArc of the winning team
    gtempptr = vtempptr->firstArc;

    // Allocate memory for the new edge
    Edge* nextEdge = malloc(sizeof(Edge));
    nextEdge->weight = goalDiff;
    nextEdge->nextArc = NULL;

    // Connect the new edge to the winning team's edge list
    if (gtempptr == NULL) {
        // Update the firstArc if no edge exists yet
        vtempptr->firstArc = nextEdge;
    } else {
        // Traverse to the end of the linked list and add the new edge
        while (gtempptr->nextArc != NULL) {
            gtempptr = gtempptr->nextArc;
        }
        gtempptr->nextArc = nextEdge;
    }

    // Find the losing team vertex
    vtempptr = graph->first;
    while (losingfoundflag != 1) {
        if (strcmp(vtempptr->team, losingTeam) == 0) {
            losingfoundflag = 1;
        } else {
            vtempptr = vtempptr->next;
        }
    }
    
    vtempptr->inDegree++;

    // Connect the new edge to the losing team
    nextEdge->destination = vtempptr;
}


void printGraph(graphHead* graph){
	int i;
	Vertex *vtempptr = NULL;
	vtempptr = graph->first;
	Edge *gtempptr = NULL;
	
	
	for (i = 0; i < graph->count; i++) {
		printf("%s -> ", vtempptr->team);
		gtempptr = vtempptr->firstArc;
		while (gtempptr != NULL) {
			printf("%s | %d", gtempptr->destination->team, gtempptr->weight);
			if (gtempptr->nextArc != NULL) {
				printf(" -> ");
			}
			gtempptr = gtempptr->nextArc;
		}
		printf("\n----------------------------------------------------------------------------------------------------------------------\n");
		vtempptr = vtempptr->next;
	}
}

//Compare the maximum of all the outDegrees of all vertices to get the team that had the most wins.
void getMostWins(graphHead *graph){
	int i;
	int max = -1;
	int count = 0;
	char winningTeam[81];
	Vertex *vtempptr = graph->first;
	Edge *gtempptr;
	
	while (vtempptr != NULL) {
		if (vtempptr->outDegree > max) {
			max = vtempptr->outDegree;
			strcpy(winningTeam, vtempptr->team);
		}
		vtempptr = vtempptr->next;
	}
		
	printf("Team with the most victories:\n%s have won %d matches.", winningTeam, max);
}

//Compare the maximum of all the inDegrees of all vertices to get the team that had the most losses.
void getMostLosses(graphHead *graph){
	int i;
	int max = -1;
	int count = 0;
	char losingTeam[81];
	Vertex *vtempptr = graph->first;
	Edge *gtempptr;
	
	while (vtempptr != NULL) {
		if (vtempptr->inDegree > max) {
			max = vtempptr->inDegree;
			strcpy(losingTeam, vtempptr->team);
		}
		vtempptr = vtempptr->next;
	}
		
	printf("\n\nTeam with the most losses:\n%s have lost %d matches.", losingTeam, max);

}

void getMaxGoals(graphHead *graph) {
    int max = -1; 
    int goalsMade = 0; // Accumulator for goals made by a team
    int goalsTaken = 0; // Accumulator for goals taken by a team
    int totalGoalDiff = 0; // Total goal difference for a team
    char maxGoalTeam[81]; // Team with the maximum goal difference
    char tempname[81]; // Temporary variable to store the team name during traversal
    Vertex *vtempptr1 = graph->first; // Pointer to traverse the vertices
    Vertex *vtempptr2 = graph->first; // Secondary pointer for nested traversal
    Edge *gtempptr; // Pointer to traverse the edges

    while (vtempptr1 != NULL) { // Loop through each team
        gtempptr = vtempptr1->firstArc; // Set the edge pointer to the first edge of the current team
        strcpy(tempname, vtempptr1->team); // Copy the team name for comparison

        while (gtempptr != NULL) { // Loop through each edge of the current team
            goalsMade = goalsMade + gtempptr->weight; // Accumulate goals made
            gtempptr = gtempptr->nextArc; // Move to the next edge
        }

        while (vtempptr2 != NULL) { // Nested loop to compare with other teams
            gtempptr = vtempptr2->firstArc; // Set the edge pointer to the first edge of the comparison team

            while (gtempptr != NULL) { // Loop through each edge of the comparison team
                if (strcmp(tempname, gtempptr->destination->team) == 0) { // Check if the teams match
                    goalsTaken = goalsTaken + gtempptr->weight; // Accumulate goals taken
                }
                gtempptr = gtempptr->nextArc; // Move to the next edge
            }

            vtempptr2 = vtempptr2->next; // Move to the next comparison team
        }

        totalGoalDiff = goalsMade - goalsTaken; // Calculate the total goal difference for the current team

        if (totalGoalDiff > max) { // Check if the current team has a higher goal difference
            max = totalGoalDiff; // Update the maximum goal difference
            strcpy(maxGoalTeam, tempname); // Update the team with the maximum goal difference
        }

        goalsMade = 0; // Reset goalsMade for the next team
        goalsTaken = 0; // Reset goalsTaken for the next team
        totalGoalDiff = 0; // Reset totalGoalDiff for the next team
        vtempptr1 = vtempptr1->next; // Move to the next team
        vtempptr2 = graph->first; // Reset the comparison team pointer
    }

    printf("\n\nTeam with the highest goal difference:\n%s have a goal difference of %d.", maxGoalTeam, max);

}

//this function does the exact same thing as getMaxGoals but retrives the minimum of the total goal difference accumulated.
void getMinGoals(graphHead *graph){
    int i;
	int min = 9999;
	int count = 0;
	int goalsMade = 0;
	int goalsTaken = 0;
	int totalGoalDiff = 0;
	char maxGoalTeam[81];
	char tempname[81];
	Vertex *vtempptr1 = graph->first;
	Vertex *vtempptr2 = graph->first;
	Edge *gtempptr;
	
	while (vtempptr1 != NULL) {
		gtempptr = vtempptr1->firstArc;
		strcpy(tempname, vtempptr1->team);
		while (gtempptr != NULL) {
			goalsMade = goalsMade + gtempptr->weight;
			gtempptr = gtempptr->nextArc;
		}
			while (vtempptr2 != NULL) {
				gtempptr = vtempptr2->firstArc;
				while (gtempptr != NULL) {
					if (strcmp(tempname, gtempptr->destination->team) == 0) {
						goalsTaken = goalsTaken + gtempptr->weight;
					}
					gtempptr = gtempptr->nextArc;
				}
				vtempptr2 = vtempptr2->next;
			}
		totalGoalDiff = goalsMade - goalsTaken;
		if (totalGoalDiff < min) {
			min = totalGoalDiff;
			strcpy(maxGoalTeam, tempname);
		}
		goalsMade = 0;
		goalsTaken = 0;
		totalGoalDiff = 0;	
		vtempptr1 = vtempptr1->next;
		vtempptr2  = graph->first;
	}
	
	printf("\n\nTeam with the lowest goal difference:\n%s have a goal difference of %d.\n", maxGoalTeam, min);

}


int checkwinChain(graphHead *graph, char team1[], char team2[]){
    Vertex* vtempptr = graph->first;
    Edge* gtempptr;

    // Find the vertex corresponding to team1
    while (vtempptr != NULL) {
        if (strcmp(vtempptr->team, team1) == 0) {
            // Team1 vertex found
            gtempptr = vtempptr->firstArc;

            // Check if team1 beat any team
            while (gtempptr != NULL) {
                Vertex* intermediateTeam = gtempptr->destination;

                // Check if the intermediate team beat team2
                Edge* edgeToTeam2 = intermediateTeam->firstArc;
                while (edgeToTeam2 != NULL) {
                    if (strcmp(edgeToTeam2->destination->team, team2) == 0) {
                        // There is a win chain
                        return 1;
                    }
                    edgeToTeam2 = edgeToTeam2->nextArc;
                }

                gtempptr = gtempptr->nextArc;
            }

            // If we are here, no win chain is found
            return 0;
        }

        vtempptr = vtempptr->next;
    }

    // If we reach here, team1 is not in the graph
    return 0;
}

int checkPath(graphHead* graph, char team1[], char team2[]) {
    Vertex* startVertex = NULL;
    Vertex* endVertex = NULL;

    // Find the vertices corresponding to team1 and team2
    Vertex* vtempptr = graph->first;
    while (vtempptr != NULL) {
        if (strcmp(vtempptr->team, team1) == 0) {
            startVertex = vtempptr;
        }
        if (strcmp(vtempptr->team, team2) == 0) {
            endVertex = vtempptr;
        }

        vtempptr = vtempptr->next;
    }

    if (startVertex == NULL || endVertex == NULL) {
        // One or both teams not found in the graph
        return 0;
    }

    // Reset the visited flags before starting DFS
    vtempptr = graph->first;
    while (vtempptr != NULL) {
        vtempptr->isVisited = 0;
        vtempptr = vtempptr->next;
    }

    // Perform DFS to check for a path
    return DFS(graph, startVertex, endVertex);
}

int DFS(graphHead* graph, Vertex* current, Vertex* target) {
    if (current == target) {
        // Found the target, there is a path
        return 1;
    }

    current->isVisited = 1; // Mark the current vertex as visited

    Edge* edge = current->firstArc;
    while (edge != NULL) {
        Vertex* nextVertex = edge->destination;
        if (!nextVertex->isVisited) {
            if (DFS(graph, nextVertex, target)) {
                return 1; // Path found
            }
        }
        edge = edge->nextArc;
    }

    return 0; // No path found
}
