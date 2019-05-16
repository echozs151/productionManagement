#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <windows.h>
#include <time.h>

//#include <C:\Users\ECHOZS\Desktop\data\bin\parse.c>

int previousNode(int node,int followed,int jump);
int nextNode(int node,int followed);
void printStations();
void setStations();
void buildMap(char * array[],int  taskTime[],int nTask);
int findSolution(int m, int c,int rule,int nTask);
void alpbe(int mmin,int mmax, char * array[]);
void vns(int mmin,int mmax, char * array[]);
const char *readLine(FILE *file);
void readFileLine(int option);


typedef struct
{
    int next[100];
    int totalNext;
    int prev[100];
    int totalPrev;
    int id;

    int followingTasks;

    int taskTime;
} Nodeg;

FILE *pFile;
Nodeg nodesGlobal[600];
int tempStation[80][80];
int tempStation2[80][80];
int setRule = 1;
int idleTime = 0;
char *INPUT_DIR=NULL;//"C:\\Users\\ECHOZS\\Desktop\\data\\bin\\ProjMng_Project\\data\\10_jobs_(manning).txt";
char previousGraph[100];
char currentGraph[100];
float xTable[70];
float xTable2[70];
short firstBuild = 1;
int globalM,globalC;

// Used for Following task priority
int previousNode(int node,int followed,int jump)
{
	int i;
	Nodeg nodeg = nodesGlobal[node-1];
	int totalPrev = nodeg.totalPrev;

	for(i=0;i<totalPrev;i++)
	{
		previousNode(nodeg.prev[i],followed+1,followed-1);
	}
	if(followed > nodesGlobal[node-1].followingTasks)
		nodesGlobal[node-1].followingTasks = followed;
	//printf("Reverse: Node %i Followed %i\n",node,followed);
	return jump+1;
}

// Used for Following task priority
int nextNode(int node,int followed)
{
	Nodeg nodeg = nodesGlobal[node-1];

	int totalNext = nodeg.totalNext;
	int i;


	if(totalNext == 0){
		//printf("reversing..from %i Followed: %i Jump: %i\n",node,followed-followed,followed);
		nodesGlobal[node-1].followingTasks = 0;
		previousNode(node,0,followed);
	}

	for(i=0;i<totalNext;i++)
	{
		nextNode(nodeg.next[i],followed+1);
	}

	//printf("Node %i Followed %i\n",node,followed);
	return followed;
}
// Prints the stations that were calculated
// Print is targeted to the output file
void printStations()
{
	int i,j;
	for(i=0;i<80;i++)
	{
		if(tempStation2[i][0] != 0)
			fprintf(pFile,"S[%i]: ",i+1);
		for(j=0;j<80;j++)
		{
			if(tempStation2[i][j]  != 0)
				fprintf(pFile,"%i ",tempStation2[i][j]);
				//tempStation2[i][j] = tempStation[i][j];
		}
		fprintf(pFile,"  ");
	}
}

// Initialize arrays holding stations with 0
void setStations()
{
	int i,j;
	for(i=0;i<80;i++)
	{
		for(j=0;j<80;j++)
		{
			tempStation2[i][j] = 0;
		}
	}
	for(i=0;i<80;i++)
	{
		for(j=0;j<80;j++)
		{
			tempStation2[i][j] = tempStation[i][j];
			tempStation[i][j] = 0;
		}
	}

}

// Create a map with precedence requirements
void buildMap(char * array[],int  taskTime[],int nTask){

    //printf("Building Map: NTask: %i\n",nTask);
    int i = 0,ii=0,j=0;
    int ij;
    int iji;
    for(ij=0;ij<600-1;ij++)
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
        nodesGlobal[ij].followingTasks = 0;

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

        if(i == -1)
        	break;
        ii++;
    }


    // Iterate once more to set previous tasks
    int tempPrev = 0;
    int totalPrev = 0;
    int totalNext = 0;
    int tempNextj=0;
    int p_i,p_j;
    for(p_i = 0;p_i<nTask;p_i++)
    {
    	totalNext = 0;
    	tempNextj=0;
    	tempPrev = 0;
    	totalPrev = 0;
    	for(p_j = 0;p_j<100;p_j++)
    	{

    		if(nodesGlobal[p_i].next[p_j] != 0)
    		{
    			//tempNextj = nodesGlobal[p_i].next[totalNext];
    			//totalPrev =
    			tempPrev = nodesGlobal[nodesGlobal[p_i].next[p_j]-1].totalPrev;
    			nodesGlobal[nodesGlobal[p_i].next[p_j]-1].prev[tempPrev] = nodesGlobal[p_i].id;
    			nodesGlobal[nodesGlobal[p_i].next[p_j]-1].totalPrev += 1;
    		}

    	}
    }

    // Find following tasks
    if(setRule == 3 || setRule == 4)
    	nextNode(1,0);




}

// Apply the rest rules. Output stations with nodes
int findSolution(int m, int c,int rule,int nTask)
{
	//printf("inside find solution");

	int totalStart=0;
	int nextNodes[600];
	int totalNext = 0;
	short firstRun = 1;
	int counter = 0;
	int min,max,minNode,maxNode;
	int tempTasktime;
	int stationCount = 0;
	int totalStations=1;
	int totalNodesInStation = 0;
	int nodesDone[600];
	int totalDone = 0;
	short doneFound;
	int totalIdle = 0;
	int tempCycle = 0;
	int nextNodePick;
	int mftNode,lftNode,ft,mft,lft;
	int miftNode,liftNode,tn,mift,lift;
	float highPriority=0;
	int highPriorityNode;
	int i,j,zeroing;
	for( i=0;i<80;i++)
	{
		for(j=0;j<80;j++)
		{
			tempStation[i][j] = 0;
		}
	}

	// zero nextNodes
	for(zeroing=0;zeroing<600;zeroing++)
	{
		if(zeroing < 600)
			nextNodes[zeroing] = 0;
		nodesDone[zeroing] = 0;
	}


	min = 9999;
	max = 0;
	lft = 9999;
	mft = 0;
	lift = 9999;
	mift = 0;
	for(i=0;i<nTask;i++)
	{
		if(nodesGlobal[i].totalPrev == 0)
		{
			totalStart++;
			nextNodes[totalNext] = nodesGlobal[i].id;
			totalNext++;
		}

	}

	// gather info from all the nodes
	while(totalNext != 0)
	{
		min = 9999;
		max = 0;
		lft = 9999;
		mft = 0;
		lift = 9999;
		mift = 0;
		doneFound = 0;
		if(firstRun == 1)
		{
			firstRun = 0;


		}

		short panicCounter = 0;
		int i;
		for( i=0;i<600;i++)
		{
			tempTasktime = nodesGlobal[nextNodes[i]-1].taskTime;
			ft = nodesGlobal[nextNodes[i]-1].followingTasks;
			tn = nodesGlobal[nextNodes[i]-1].totalNext;
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
			if(ft >= mft)
			{
				mft = ft;
				mftNode = nodesGlobal[nextNodes[i]-1].id;
			}
			if(ft < lft)
			{
				lft = ft;
				lftNode = nodesGlobal[nextNodes[i]-1].id;
			}
			if(tn > mift)
			{
				mift = tn;
				miftNode = nodesGlobal[nextNodes[i]-1].id;

			}
			if(tn < lift)
			{
				lift = tn;
				liftNode = nodesGlobal[nextNodes[i]-1].id;
			}

			if(setRule == 8 || i < nTask)
			{
				if(highPriority < xTable[nodesGlobal[nextNodes[i]-1].id - 1])
				{
					highPriority = xTable[nodesGlobal[nextNodes[i]-1].id - 1];
					highPriorityNode = nodesGlobal[nextNodes[i]-1].id;
				}
			}
		}
		int ii=0;

		// Add the chosen node into done, remove it from nextNodes, add to station and calculate if need new station
		if(setRule == 1)
			nextNodePick = maxNode;
		else if(setRule == 2)
			nextNodePick = minNode;
		else if(setRule == 3)
			nextNodePick = mftNode;
		else if(setRule == 4)
			nextNodePick = lftNode;
		else if(setRule == 5)
			nextNodePick = miftNode;
		else if(setRule == 6)
			nextNodePick = liftNode;
		else if(setRule == 8)
			nextNodePick = highPriority;
		int k;
		for(k =0;k<nTask;k++)
		{
			// Search nextNode within nextNodes to remove it and update statistics
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
		int prevDone;
		for(k =0;k<nodesGlobal[nextNodePick-1].totalNext;k++)
		{
			doneFound1 = 0;
			sumPrevious = 0;
			// Verify that next node does not have prerequisite.
			if(nodesGlobal[nodesGlobal[nextNodePick-1].next[k]-1].totalPrev != 0)
			{
				// iterate next node's previous nodes. make sure all of them are done
				for(prevDone=0;prevDone<nodesGlobal[nodesGlobal[nextNodePick-1].next[k]-1].totalPrev;prevDone++)
				{
					int dNode;
					for(dNode=0;dNode<totalDone;dNode++)
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
			int l;
			for(l=0;l<200;l++)
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
    int taskTime[2000];
    int taskTimeCounter = 0,taskTimeSum = 0,taskTimeMax = 0;
    int tempSolution = -1;
    int lbc,c;
    int maxint;
    int m;
    int mStation,cStation;
    int idle = -2;
    m = mmin;
    int bm,bc;
    double bcpu;
    clock_t start, end;
    double cpu_time_used;
    start = clock();

    //printf("inside alpbe2 nTask:%s",array[0]);
    // Place task times into taskTime array, starting from 0
    int i;
    for(i=1;i<=nTask;i++)
    {

    	//printf("i:%i",i);
        taskTime[taskTimeCounter] = atoi(array[i]);
        taskTimeCounter++;
    }
    taskTime[taskTimeCounter] = -1;
    taskTimeCounter =0;
    if(strcmp(currentGraph,previousGraph) || firstBuild == 1)
    {
    	buildMap(array,taskTime,nTask);
    	firstBuild = 0;
    }
    strcpy(previousGraph,currentGraph);

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




	// Algorithm's steps
    for(i=m;i<=mmax;i++)
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

        // idle == -2 is the first iteration needed to initialize
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
			globalM = bm;
			globalC = bc;
			setStations();
		}



    }
    end = clock();
    bcpu = ((double) (end - start)) / CLOCKS_PER_SEC;
    //printf("Best Solution: m: %i  c: %i  idle: %i\n",bm,bc,idle);
    fprintf(pFile,"%i \t%i \t%i \t%.4f \t ",idle,bm,bc,bcpu);
    printStations();
    fprintf(pFile,"\n");


}

// Implementation of the VNS priority
void vns(int mmin,int mmax, char * array[])
{
	int r;
	int k;
	int totalJobs,i;
	int nm,nc;
	int counter= 0;
	int nTask = atoi(array[0]);
	int kmax = nTask;
	totalJobs = 10;
	// step 1
	for(i=0;i<=totalJobs;i++)
	{
		int num = (rand() %  (100 + 1));
		float randomValue =  (float)num / 100;
		xTable[i] = randomValue;
		if(i==totalJobs)
			xTable[i] = -1;
	}
	i = 0;
	//srand(time(0));
	while(xTable[i] != -1)
	{
		printf("Num: %i %f\n",i,xTable[i]);
		i++;
	}


	// step 2
	while(counter < totalJobs)
	{

		alpbe(mmin,mmax,array);
		k = 1;
		while(k != kmax)
		{


		}
		counter++;
	}

}

const char *readLine(FILE *file) {

    if (file == NULL) {
        printf("Error: file pointer is null.");
        exit(1);
    }

    int maximumLineLength = 128;
    char *lineBuffer = (char *)malloc(sizeof(char) * maximumLineLength);

    if (lineBuffer == NULL) {
        printf("Error allocating memory for line buffer.");
        exit(1);
    }

    char ch = getc(file);
    int count = 0;

    while ((ch != '\n') && (ch != EOF)) {
        if (count == maximumLineLength) {
            maximumLineLength += 128;
            lineBuffer = realloc(lineBuffer, maximumLineLength);
            if (lineBuffer == NULL) {
                printf("Error reallocating space for line buffer.");
                exit(1);
            }
        }
        lineBuffer[count] = ch;
        count++;

        ch = getc(file);
    }

    lineBuffer[count] = '\0';
    char line[count + 1];
    strncpy(line, lineBuffer, (count + 1));
    free(lineBuffer);
    const char *constLine = line;
    return constLine;
}

// Parse Files
void readFileLine(int option)
{

    //char *input = "C:\\Users\\ECHOZS\\Desktop\\data\\bin\\ProjMng_Project\\data\\10_jobs_(manning).txt";

    FILE * fp;
    char line[256];
    size_t len = 0;
    ssize_t read;
    int lineCount =0;
    char * previousGraph = "first";

	fprintf(pFile,"Parsing Station Ranges.. Rule: %i\n",setRule);
	INPUT_DIR = "data-sets\\stations_range.txt";

	



    fp = fopen(INPUT_DIR, "r");
    //readLine(fp);
    if (fp == NULL){
    	printf("Failed");
    	exit(EXIT_FAILURE);
	}
    
    


    fprintf(pFile,"Prec. graph \tmMin \tmMax\tIdle*\tm*\tc*\tCPU\tTasks\n");
    //while ((read = getline(&line, &len, fp)) != -1) {
    while (fgets(line, sizeof line, fp)) {
        //printf("Retrieved line of length %zu:\n", read);
    	//if(lineCount>=1)
        //printf("%s",line);
		
        int i = 0;
        char *p = strtok (line, "�\t");
        char *array[2000];
        
        int maximumLineLength = 128;
    	char *lineBuffer = (char *)malloc(sizeof(char) * maximumLineLength);
		if (lineBuffer == NULL) {
	        printf("Error allocating memory for line buffer.");
	        exit(1);
	    }

        //char *lineStore = &line;

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
                        p = strdup(strtok (NULL, "�\t"));
                    }
                    //printf("%s \n",strupr(array[0]));
                    char graphDirectory[100] = "data-sets\\precedence graphs\\";
                    strupr(array[0]);
                    char graphName[40];// = array[0];
                    strcpy(graphName,array[0]);
                    strcat(graphName,".IN2");
                    strcat(graphDirectory,graphName);

                    strcpy(currentGraph,array[0]);

                    //char *graphDirectory = strcat(,graphName);
                    //printf("%s\n",graphDirectory); // Prints the path of the graph data
                    printf("%s\t",array[0]);
                    fprintf(pFile,"%s ",array[0]);
                    FILE * fp2;
                    char line2[256];
                    size_t len2 = 0;
                    ssize_t read2;

                    fp2 = fopen(graphDirectory, "r");
                    if (fp2 == NULL)
                    {
                    	fprintf(pFile,"\n");
                    	printf("FAILED\n");
                        continue;
                    }else
                    {
                    	fprintf(pFile,"\t%i\t %i\t",atoi(array[1]),atoi(array[2]));
                    	printf("OK\n");
                    }
                        //exit(EXIT_FAILURE);

                    //char *array2[800];

                    char** array2;
                    array2 = malloc(len * sizeof(char*)+2048);
                    int ii = 0;
                    //if(!strcmp(graphName,previousGraph) ){
						//while ((read2 = getline(&line2, &len2, fp2)) != -1) {
						while (fgets(line2, sizeof line2, fp2)) {
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
                    


                    //printf("before alpbe");
                    if(setRule != 6)
                    	alpbe(atoi(array[1]),atoi(array[2]),array2);
                    else{
                    	vns(atoi(array[1]),atoi(array[2]),array2);
                    }
                    //printf("\nread2 finished..Free array2 with ii: %i Graph: %s\n",ii,previousGraph);




                }

            //char *precedenceGraphName = array[0];

        lineCount++;
        //sleep(1);
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



	//setbuf(stdout, NULL);



	printf("1. LTT\n2. STT\n3. MFT\n4. LFT\n5. MIFT\n6. LIFT\n8. VNS\n");
	printf( "Option :");
    for(;;)
    {

        int opt;

        scanf("%d",&opt);
        setRule = opt;
        if(setRule == 1)
        	pFile=fopen("LTT.txt", "a");
        else if(setRule == 2)
			pFile=fopen("STT.txt", "a");
        else if(setRule == 3)
			pFile=fopen("MFT.txt", "a");
        else if(setRule == 4)
			pFile=fopen("LFT.txt", "a");
        else if(setRule == 5)
			pFile=fopen("MIFT.txt", "a");
        else if(setRule == 6)
			pFile=fopen("LIFT.txt", "a");
        else if(setRule == 8)
			pFile=fopen("VNS.txt", "a");

		printf("%i",opt);

        readFileLine(opt);
        getchar();
        fclose(pFile);
        printf( "\nOption :");

    }


    return 0;

}




