#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

//#include <C:\Users\ECHOZS\Desktop\data\bin\parse.c>

typedef struct
{
    int next[100];
    int totalNext;
    int prev[100];
    int totalPrev;
    int id;

    int taskTime;
} Nodeg;

FILE *pFile;
Nodeg nodesGlobal[200];
int tempStation[80][80];
int tempStation2[80][80];
int setRule = 1;
int idleTime = 0;
char *INPUT = NULL;//"C:\\Users\\ECHOZS\\Desktop\\data\\bin\\ProjMng_Project\\data\\10_jobs_(manning).txt";


void printStations()
{
	for(int i=0;i<80;i++)
	{
		if(tempStation2[i][0] != 0)
			printf("S[%i]: ",i+1);
		for(int j=0;j<80;j++)
		{
			if(tempStation2[i][j]  != 0)
				printf("%i ",tempStation2[i][j]);
				//tempStation2[i][j] = tempStation[i][j];
		}
		printf("  ");
	}
}
void setStations()
{
	for(int i=0;i<80;i++)
	{
		for(int j=0;j<80;j++)
		{
			tempStation2[i][j] = 0;
		}
	}
	for(int i=0;i<80;i++)
	{
		for(int j=0;j<80;j++)
		{
			tempStation2[i][j] = tempStation[i][j];
			tempStation[i][j] = 0;
		}
	}

}

// DONE
Nodeg* buildMap(char * array[],int  taskTime[],int nTask){

	Nodeg nodesGlobal1[100];
    //printf("Building Map: NTask: %i\n",nTask);
    int i = 0,ii=0,j=0;
    int iji;
    for(int ij=0;ij<200-1;ij++)
    {
        if(ij < nTask)
        {
        	//printf("Maybe here [%i]?\n",ij);
        	nodesGlobal[ij].id = ij+1;
        	nodesGlobal[ij].taskTime = taskTime[ij];
        }
        else
        {
        	nodesGlobal[ij].id = -1;
        	nodesGlobal[ij].taskTime = 0;
        }
        nodesGlobal[ij].totalNext = 0;
        nodesGlobal[ij].totalPrev = 0;

        for(iji=0;iji<100;iji++)
        {

        	nodesGlobal[ij].next[iji] = 0;
        	nodesGlobal[ij].prev[iji] = 0;
        }
    }
    //printf("Done init\n");


    // Gather Precedence info. Make new function
    short PrintIj = 0;
    while(i != -1)
    {
        //printf("Are we here?");
        char *token;
        char tempString[200];
        strcpy(tempString,array[ii]);
        /* get the first token */
        token = strtok(tempString,",");
        /* walk through other tokens */
        int pos=0;

        while( token != NULL ) {
            pos++;
            //char *token1 = token;
            token[strcspn(token, "\n")] = 0;
            //printf( " %s -> %i \n", token ,pos );
            if(pos==1)
                i = atoi(token);
			if(pos==2)
			{
                j = atoi(token);
                PrintIj = 1;
			}

			if(PrintIj == 1)
			{
				//printf("i&j %i %i\n",i,j);
				PrintIj = 0;

				if(j != 0)
				{
					int nexti = 0;

					while(nodesGlobal[i-1].next[nexti] != 0)
					{
						//printf("Checking nodes[%i-1]: %i\n",i,nodes[i-1].next[nexti]);
						//
						nexti++;
					}
					if(i != -1)
					{
						nodesGlobal[i-1].next[nexti] = j;
						nodesGlobal[i-1].totalNext += 1;
					}
				}

			}


            token = strtok(NULL, ",");



        }
        /*if(j != 0)
        {
        	int nexti = 0;

			while(nodesGlobal[i-1].next[nexti] != 0)
			{
				//printf("Checking nodes[%i-1]: %i\n",i,nodes[i-1].next[nexti]);
				nexti++;
			}
        }*/
        //if(j != 0)
        	//nodesGlobal[i-1].next[nexti] = j;
        //printf("[%i] Final nodes[%i-1]: %i  j: %i\n",ii,ii,nodes[i-1].next[nexti],j);

        if(i == -1)
        	break;
        ii++;
    }


    // Iterate once more to set previous tasks
    int tempPrev = 0;
    for(int p_i = 0;p_i<nTask;p_i++)
    {
    	tempPrev = 0;
    	for(int p_j = 0;p_j<nTask;p_j++)
    	{
    		if(nodesGlobal[p_i].next[p_j] != 0)
    		{
    			tempPrev = nodesGlobal[nodesGlobal[p_i].next[p_j]-1].totalPrev;
    			nodesGlobal[nodesGlobal[p_i].next[p_j]-1].prev[tempPrev] = nodesGlobal[p_i].id;
    			nodesGlobal[nodesGlobal[p_i].next[p_j]-1].totalPrev += 1;
    			//tempPrev++;
    		}
    	}
    }


    /*int boolHasNext = 0;
    for(int testi=0;testi<nTask;testi++)
    {
    	boolHasNext = 0;
    	for(int testj=0;testj<100;testj++)
    	{
    		if(nodesGlobal[testi].next[testj] != 0)
    		{
    			//printf("ID: %i Next: %i [%i][%i]\n",nodesGlobal[testi].id,nodesGlobal[testi].next[testj],testi,testj);
    			boolHasNext = 1;
    		}
    		/*if(nodesGlobal[testi].prev[testj] != 0)
    			printf("   Prev: %i \n",nodesGlobal[testi].prev[testj]);

    	}
    	if(boolHasNext == 0)
    		printf("ID: %i Next: None \n",nodesGlobal[testi].id);
    }*/

    Nodeg nodess[10];
    return nodess;
}

// Apply the rest rules. Output stations with nodes
int findSolution(int m, int c,int rule,int nTask)
{
	//printf("inside find solution");

	int totalStart=0;
	int nextNodes[200];
	int totalNext = 0;
	short firstRun = 1;
	int counter = 0;
	int min,max,minNode,maxNode;
	int tempTasktime;
	int stationCount = 0;
	int totalStations=1;
	int totalNodesInStation = 0;
	int nodesDone[200];
	int totalDone = 0;
	short doneFound;
	int totalIdle = 0;
	int tempCycle = 0;
	int nextNodePick;
	for(int i=0;i<80;i++)
	{
		for(int j=0;j<80;j++)
		{
			tempStation[i][j] = 0;
		}
	}

	// zero nextNodes
	for(int zeroing=0;zeroing<200;zeroing++)
	{
		if(zeroing < 200)
			nextNodes[zeroing] = 0;
		nodesDone[zeroing] = 0;
	}

	// The good stuff
	min = 9999;
	max = 0;
	for(int i=0;i<nTask;i++)
	{
		if(nodesGlobal[i].totalPrev == 0)
		{
			totalStart++;
			nextNodes[totalNext] = nodesGlobal[i].id;
			totalNext++;
		}

	}

	// Here we gather info from all the nodes
	while(totalNext != 0)
	{
		min = 9999;
		max = 0;
		doneFound = 0;
		if(firstRun == 1)
		{
			firstRun = 0;


		}

		short panicCounter = 0;
		for(int i=0;i<200;i++)
		{
			tempTasktime = nodesGlobal[nextNodes[i]-1].taskTime;
			if(tempTasktime == 0)
			{
				panicCounter++;
				if(panicCounter > 10)
					break;
				continue;
			}

			if(tempTasktime < min)
			{
				min = tempTasktime;
				minNode = nodesGlobal[nextNodes[i]-1].id;
			}
			if(tempTasktime > max)
			{
				max = tempTasktime;
				maxNode = nodesGlobal[nextNodes[i]-1].id;
			}
		}
		int ii=0;

		// Add the chosen node into done, remove it from nextNodes, add to station and calculate if need new station
		if(setRule == 1)
			nextNodePick = maxNode;
		else if(setRule == 2)
			nextNodePick = minNode;
		for(int k =0;k<nTask;k++)
		{
			if(nextNodes[k] == nextNodePick)
			{
				tempStation[totalStations-1][stationCount] = nextNodePick;
				nodesDone[totalDone] = nextNodePick;
				totalNodesInStation++;
				stationCount++;
				nextNodes[k] = 0;
				totalNext--;
				totalDone++;

				tempCycle += nodesGlobal[nextNodePick-1].taskTime;
				if(tempCycle > c)
				{
					totalIdle += (c-(tempCycle-nodesGlobal[nextNodePick-1].taskTime));
					tempStation[totalStations-1][stationCount-1] = 0;
					tempCycle = nodesGlobal[nextNodePick-1].taskTime;

					stationCount = 1;
					totalStations++;
					if(totalStations > m)
						return -1;
					tempStation[totalStations-1][0] = nextNodePick;
				}

				break;
			}
		}


		short emptyl = -1;
		short doneFound1;
		int sumPrevious;
		for(int k =0;k<nodesGlobal[nextNodePick-1].totalNext;k++)
		{
			doneFound1 = 0;
			sumPrevious = 0;
			// Verify that next node does not have prerequisite.
			if(nodesGlobal[nodesGlobal[nextNodePick-1].next[k]-1].totalPrev != 0)
			{
				// iterate next node's previous nodes. make sure all of them are done
				for(int prevDone=0;prevDone<nodesGlobal[nodesGlobal[nextNodePick-1].next[k]-1].totalPrev;prevDone++)
				{
					for(int dNode=0;dNode<totalDone;dNode++)
					{
						if(nodesGlobal[nodesGlobal[nextNodePick-1].next[k]-1].prev[prevDone] == nodesDone[dNode])
						{
							sumPrevious += 1;
						}
					}
				}
				if(sumPrevious == nodesGlobal[nodesGlobal[nextNodePick-1].next[k]-1].totalPrev)
					doneFound1 = 1;
			}


			if(doneFound1 == 0)
				continue;
			// Find free position to place next node
			for(int l=0;l<200;l++)
			{
				if(nextNodes[l] == 0)
				{
					emptyl = l;
					break;
				}

			}
			// set node into nextNode array
			if(emptyl != -1)
			{
				nextNodes[emptyl] = nodesGlobal[nextNodePick-1].next[k];
				totalNext++;
			}


		}



	}
	totalIdle += c-tempCycle;
	return totalIdle;


}

// DONE. Format pretty output
void alpbe(int mmin,int mmax, char * array[])
{


    int nTask = atoi(array[0]);
    int taskTime[100];
    int taskTimeCounter = 0,taskTimeSum = 0,taskTimeMax = 0;
    int tempSolution = -1;
    int lbc,c;
    int maxint;
    int m;
    int mStation,cStation;
    int idle = -2;
    m = mmin;
    int bm,bc;
    //printf("inside alpbe2 nTask:%s",array[0]);
    // Place task times into taskTime array, starting from 0
    for(int i=1;i<=nTask;i++)
    {

    	//printf("i:%i",i);
        taskTime[taskTimeCounter] = atoi(array[i]);
        taskTimeCounter++;
    }
    taskTime[taskTimeCounter] = -1;
    taskTimeCounter =0;
    buildMap(array,taskTime,nTask);
    while(taskTime[taskTimeCounter] != -1)
    {
        //printf("Task Time[%i]: %i",taskTimeCounter,taskTime[taskTimeCounter]);
        taskTimeSum+=taskTime[taskTimeCounter];
        taskTimeCounter++;
        if(taskTimeMax < taskTime[taskTimeCounter])
            taskTimeMax = taskTime[taskTimeCounter];
    }

    if(taskTimeMax > taskTimeSum/nTask )
        lbc = taskTimeMax;
    else
        lbc = taskTimeSum / nTask;

    c = lbc;





    for(int i=m;i<=mmax;i++)
    {
    	//printf("Iteration: %i\n",i);
    	tempSolution = -1;
        cStation = 0;
        mStation = 0;
        c = lbc;

        while(tempSolution == -1)
        {
        	tempSolution = findSolution(i,c,1,nTask);
        	if(tempSolution == -1)
        		c++;

        }

		//printf("Found solution: m: %i  c: %i  idle: %i\n",i,c,tempSolution);
        // Print out only best solution
		if(idle == -2)
		{
       		idle = tempSolution;
       		bm = i;
			bc = c;
			setStations();
		}
		if(idle > tempSolution)
		{
			idle = tempSolution;
			bm = i;
			bc = c;
			setStations();
		}



    }
    //printf("Best Solution: m: %i  c: %i  idle: %i\n",bm,bc,idle);
    printf("%i \t%i \t%i \tcpu \t ",idle,bm,bc);
    printStations();
    printf("\n");


}

void readFileLine(int option)
{

    //char *input = "C:\\Users\\ECHOZS\\Desktop\\data\\bin\\ProjMng_Project\\data\\10_jobs_(manning).txt";

    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int lineCount =0;
    char * previousGraph = "first";

	printf("Parsing Station Ranges.. Rule: %i\n",setRule);
	INPUT = "data-sets\\stations_range.txt";




    fp = fopen(INPUT, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);


    printf("Prec. graph \tmMin \tmMax\tIdle*\tm*\tc*\tCPU\tTasks\n");
    while ((read = getline(&line, &len, fp)) != -1) {
        //printf("Retrieved line of length %zu:\n", read);
    	//if(lineCount>=1)
        //printf("%s",line);

        int i = 0;
        char *p = strtok (line, " \t");
        char *array[1000];


        char *lineStore = &line;


            //Manning

                if(lineCount>=1)
                {
                    //printf("%s	%s", array[0],array);
                    //printf("%s",line);
                    //char *charArray[20];
                    //charArray = *strtok(line,"\t");
                    while (p != NULL)
                    {
                        array[i++] = p;
                        p = strdup(strtok (NULL, " \t"));
                    }
                    //printf("%s \n",strupr(array[0]));
                    char graphDirectory[100] = "data-sets\\precedence graphs\\";
                    strupr(array[0]);
                    char graphName[40];// = array[0];
                    strcpy(graphName,array[0]);
                    strcat(graphName,".IN2");
                    strcat(graphDirectory,graphName);

                    //char *graphDirectory = strcat(,graphName);
                    //printf("%s\n",graphDirectory); // Prints the path of the graph data
                    printf("%s ",array[0]);
                    FILE * fp2;
                    char * line2 = NULL;
                    size_t len2 = 0;
                    ssize_t read2;

                    fp2 = fopen(graphDirectory, "r");
                    if (fp2 == NULL)
                    {
                    	printf("\n");
                        continue;
                    }else
                    {
                    	printf("\t%i\t %i\t",atoi(array[1]),atoi(array[2]));
                    }
                        //exit(EXIT_FAILURE);

                    //char *array2[800];

                    char** array2;
                    array2 = malloc(len * sizeof(char*)+2048);
                    int ii = 0;
                    //if(!strcmp(graphName,previousGraph) ){
						while ((read2 = getline(&line2, &len2, fp2)) != -1) {
							//printf("%s",line2); // Print line individually
							//printf("/t-->%s\n", line2);

							//array2[ii] = malloc(strlen(line2) + 1);
							//strcpy(array2[ii], line2);
							array2[ii] = strdup(line2);
							ii++;

						}
                    //}


                    //double totalNodes = atoi(array2[0]);
                    fclose(fp2);
                    if (line2)
						free(line2);


                    //printf("before alpbe");
                    alpbe(atoi(array[1]),atoi(array[2]),array2);
                    //printf("\nread2 finished..Free array2 with ii: %i Graph: %s\n",ii,previousGraph);




                }

            //char *precedenceGraphName = array[0];

        lineCount++;
        //read only 5 lines for debug
        /*if(lineCount>5)
            break;*/
    }

    fclose(fp);
    if (line)
        free(line);
    //exit(EXIT_SUCCESS);

}


int main()
{



	setbuf(stdout, NULL);

	pFile=fopen("myfile.txt", "a");

	printf("1. LTT\n2. STT\n3. MFT\n4. LFT\n");
	printf( "Option :");
    for(;;)
    {

        int opt;

        scanf("%d",&opt);
        setRule = opt;

        readFileLine(opt);
        getchar();

        printf( "Option :");

    }

    fclose(pFile);
    return 0;

}




