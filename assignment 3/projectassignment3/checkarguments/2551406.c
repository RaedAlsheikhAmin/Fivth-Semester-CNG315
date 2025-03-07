//used internet to learn depth first traversal
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

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
int NumOfLine(FILE*);//helper function to count the number of lines
int depthFirstTraversal(graphHead *, Vertex *, char []);//helper function for depth first traversal
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
//------------------------------------------------------------------------------------------------
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
//------------------------------------------------------------------------------------------------
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
//------------------------------------------------------------------------------------------------
graphHead* readTeams(FILE* inFile){
    int i, numofline=0;
    char tempname[81];
    numofline=NumOfLine(inFile);//counting the number of lines
    graphHead* graph=createGraph();
    rewind(inFile);
    for(i=0;i<numofline;i++)
    {
        fscanf(inFile,"%80[^\n]\n",tempname);//reading up to 80 characters or until the new line
        createVertex(graph, tempname);
    }
    return graph;
}
//------------------------------------------------------------------------------------------------
void readMatches(FILE* inFile, graphHead *graph){
    char c, dum1[5], dum2[5], dum3[10], Home[81], Away[81], FTR;
    int i, numofline=0, HomeGoals, AwayGoals,GoalDiff;
    numofline=NumOfLine(inFile);//counting the number of lines
    rewind(inFile);
    while((c=fgetc(inFile))!=EOF)//skipping 1st line
    {
        if(c=='\n')
            break;
    }
    for(i=0;i<numofline;i++)
    {
        fscanf(inFile,"%[^;];%[^;];%[^;];%[^;];%d;%d;%[^;];%c\n",dum1,dum2,dum3,Home,&HomeGoals,&AwayGoals,Away,&FTR);
        if(FTR=='H')
        {
            GoalDiff= (HomeGoals-AwayGoals);
            createEdge(graph, Home, Away, GoalDiff);
        }
        else if(FTR=='A')
        {
            GoalDiff= (AwayGoals-HomeGoals);
            createEdge(graph, Away, Home, GoalDiff);
        }
    }
}
//------------------------------------------------------------------------------------------------
void createEdge(graphHead* graph, char winningTeam[], char losingTeam[], int goalDiff){
    Vertex* WinningVertex, * LosingVertex;
    WinningVertex = graph->first;
    LosingVertex = graph->first;
    while (WinningVertex != NULL)
    {
        if (strcmp(WinningVertex->team, winningTeam) == 0)
        {
            while (LosingVertex != NULL)
            {
                if (strcmp(LosingVertex->team, losingTeam) == 0)
                {
                    Edge* edge = (Edge*)malloc(sizeof(Edge));
                    if (edge == NULL)//check if we allocated edge
                    {
                        printf("Memory allocation failed for edge.\n");
                        exit(1);
                    }
                    edge->weight = goalDiff;
                    edge->destination = LosingVertex;
                    edge->nextArc = NULL;
                    // finding the last edge in WinningVertex
                    Edge* lastEdge = WinningVertex->firstArc;
                    while (lastEdge != NULL && lastEdge->nextArc != NULL)
                    {
                        lastEdge = lastEdge->nextArc;
                    }
                    //inserting the new edge
                    if (lastEdge == NULL)
                    {
                        WinningVertex->firstArc = edge; // if there was no there edge
                    }
                    else
                    {
                        lastEdge->nextArc = edge; // otherwise insert at the end
                    }
                    // updating the outDegree and inDegree
                    WinningVertex->outDegree++;
                    LosingVertex->inDegree++;
                    break; // exit when we find losing team
                }
                LosingVertex = LosingVertex->next;
            }
            break; // exit when we find winning team
        }
        WinningVertex = WinningVertex->next;
    }
}
//------------------------------------------------------------------------------------------------
void printGraph(graphHead* graph)
{
    Vertex* temp;//using temp not to lose the head of graph while traversing
    Edge* tempE;
    temp = graph->first;
    while (temp != NULL)
    {
        printf("%s -> ", temp->team);
        tempE = temp->firstArc;
        while (tempE != NULL)
        {
            printf("%s | %d  ", tempE->destination->team, tempE->weight);
            tempE = tempE->nextArc;
            if(tempE!=NULL)//not to print the NULL pointer at the end
                printf("-> ");
        }
        printf("\n");
        printf("----------------------------------------------------------------------------------------------------------------------\n");
        temp = temp->next;
    }
}
//------------------------------------------------------------------------------------------------
void getMostWins(graphHead *graph){
    int mostWin=0;
    char mostWinTeam[81];
    Vertex *temp;
    temp=graph->first;
    while(temp!=NULL)
    {
        if(temp->outDegree > mostWin)//traversing and checking the other verteces outdegree
        {
            mostWin=temp->outDegree;
            strcpy(mostWinTeam,temp->team);
        }
        temp=temp->next;
    }
    printf("Team with the most victories:\n%s have won %d matches.\n",mostWinTeam,mostWin);
}
//------------------------------------------------------------------------------------------------
void getMostLosses(graphHead *graph){//follows the same idea as the prev function just checks inDegree
    int mostLose;
    char mostLoseTeam[81];
    Vertex *temp;
    temp=graph->first;
    mostLose=temp->inDegree;
    while(temp!=NULL)
    {
        if(temp->inDegree > mostLose)
        {
            mostLose=temp->inDegree;
            strcpy(mostLoseTeam,temp->team);
        }
        temp=temp->next;
    }
    printf("Team with the most losses:\n%s have lost %d matches.\n",mostLoseTeam,mostLose);
}
//------------------------------------------------------------------------------------------------
void getMaxGoals(graphHead *graph){
    Vertex *temp, *temp2;
    Edge *edgeOut, *edgeIn;
    int maxGoalDiff = -1, goalIn, goalOut;
    char maxGoalTeam[81];
    temp = graph->first;
    while (temp != NULL)
    {
        goalOut= 0;
        goalIn = 0;
        //calculating the goals made(out)
        edgeOut = temp->firstArc;
        while (edgeOut != NULL)
        {
            goalOut += edgeOut->weight;
            edgeOut = edgeOut->nextArc;
        }
        //calculating the goal taken(in)
        temp2 = graph->first;
        while (temp2 != NULL)
        {
            if (temp != temp2)
            {
                edgeIn = temp2->firstArc;
                while (edgeIn != NULL)
                {
                    if (strcmp(temp->team, edgeIn->destination->team) == 0)
                    {
                        goalIn += edgeIn->weight;
                    }
                    edgeIn = edgeIn->nextArc;
                }
            }
                temp2 = temp2->next;
        }
        //calculating the goal difference
        int totalGoalDiff = goalOut - goalIn;
        //if there is another higher goal difference we will update maxGoalTeam
        if (totalGoalDiff > maxGoalDiff)
        {
            maxGoalDiff = totalGoalDiff;
            strcpy(maxGoalTeam, temp->team);
        }
            temp = temp->next;
        }
    printf("\n\nTeam with the highest goal difference:\n%s have a goal difference of %d.", maxGoalTeam, maxGoalDiff);
}
//------------------------------------------------------------------------------------------------
void getMinGoals(graphHead *graph){//follows the same idea as the prev function
    Vertex *temp, *temp2;
    Edge *edgeOut, *edgeIn;
    int minGoalDiff = 100, goalIn, goalOut;
    char minGoalTeam[81];
    temp = graph->first;
    while (temp != NULL)
    {
        goalOut = 0;
        goalIn = 0;
        //calculating the goals made(out)
        edgeOut = temp->firstArc;
        while (edgeOut != NULL)
        {
            goalOut += edgeOut->weight;
            edgeOut = edgeOut->nextArc;
        }
        //calculating the goal taken(in)
        temp2 = graph->first;
        while (temp2 != NULL)
        {
            if (temp != temp2)
            {
                edgeIn = temp2->firstArc;
                while (edgeIn != NULL)
                {
                    if (strcmp(temp->team, edgeIn->destination->team) == 0) {
                        goalIn += edgeIn->weight;
                    }
                    edgeIn = edgeIn->nextArc;
                   }
            }
            temp2 = temp2->next;
        }
        //calculating the goal difference
        int totalGoalDiff = goalOut - goalIn;
        //if there is another lower goal difference we will update minGoalDiff
        if (totalGoalDiff < minGoalDiff)
        {
            minGoalDiff = totalGoalDiff;
            strcpy(minGoalTeam, temp->team);
        }
        temp = temp->next;
        }
        printf("\n\nTeam with the lowest goal difference:\n%s have a goal difference of %d.\n", minGoalTeam, minGoalDiff);
}
//------------------------------------------------------------------------------------------------
int checkwinChain(graphHead *graph, char team1[], char team2[]){
    Vertex *tempV1 = NULL, *tempV2 = NULL, *temp, *opponent ;
    Edge *opponentEdge, *tempE;
    temp = graph->first;
    //finding team1 and team2 verteces
    while (temp != NULL)
    {
        if (strcmp(temp->team, team1) == 0)
        {
            tempV1 = temp;
        }
        else if (strcmp(temp->team, team2) == 0)
        {
            tempV2 = temp;
        }
            temp = temp->next;
    }
    //checking if both of the teams exist
    if (tempV1 == NULL || tempV2 == NULL)
    {
        printf("Error: One or both teams not found in the graph.\n");
        return 0;
    }
    tempE = tempV1->firstArc;
    while (tempE != NULL)
    {
        if (tempE->weight > 0)
        {
            //checking if team1 had beat another team
            opponent = tempE->destination;
            opponentEdge = opponent->firstArc;
            while (opponentEdge != NULL)
            {
                if (strcmp(opponentEdge->destination->team, team2) == 0 && opponentEdge->weight > 0)
                {
                    return 1;
                }
                opponentEdge = opponentEdge->nextArc;
            }
        }
        tempE = tempE->nextArc;
    }
    return 0;//if we don't find any win chain we just return 0
}

//------------------------------------------------------------------------------------------------
int checkPath(graphHead *graph, char team1[], char team2[]){
     Vertex* temp;
     int isPath;
     if (strcmp(team1, team2) == 0)//it means we have the same team so we don't need to traverse
     {
        return 1;
     }
     temp = graph->first;
     //finding team1 vertex
     while (temp != NULL)
     {
         if (strcmp(temp->team, team1) == 0)
         {
             break;
         }
         temp = temp->next;
     }
     if (temp == NULL)
     {
         return 0;
     }
     isPath = depthFirstTraversal(graph, temp, team2);
     return isPath;
}
//------------------------------------------------------------------------------------------------
int NumOfLine(FILE* inFile)//helper function to count the number of lines
{
    char c;
    int count=0;
    while((c=fgetc(inFile))!=EOF)//counting the number of lines
    {
        if(c=='\n')
            count++;
    }
    rewind(inFile);
    return count;
}
//------------------------------------------------------------------------------------------------
int depthFirstTraversal(graphHead *graph, Vertex *tempV, char team2[])// depth first traversal helper function
{
    Vertex* nextTeam;
    tempV->isVisited = 1;
    Edge* tempE = tempV->firstArc;
    while (tempE != NULL)
    {
        nextTeam = tempE->destination;
        //there is a path if the next team is team2 so we return 1
        if (strcmp(nextTeam->team, team2) == 0)
        {
            return 1;
        }
        // continuing depth first traversal recursively if next team is not visited yet
        if (!nextTeam->isVisited) {
            if (depthFirstTraversal(graph, nextTeam, team2)) {
                return 1;
            }
        }
        tempE = tempE->nextArc;
    }
    return 0;//if in the end we don't find any path it will return 0
}
