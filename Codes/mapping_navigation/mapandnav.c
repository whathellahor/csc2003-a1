#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <time.h>

#define MAX_WIDTH 5          // set max width of the maze to 5
#define MAX_HEIGHT 4         // set max height of the maze to 4
#define MOVEMENT_DISTANCE 30 // set default distance to move forward one grid as 30cm
#define MIN_DISTANCE 10       // set default distance to check for wall to 5cm
#define EXIT_DISTANCE 180    // set default distance for exit to be 180cm
#define MOVEMENT_DELAY 10
#define TURNING_DELAY 5

// perform AND or OR operation with these to check for wall
#define DIRECTION_0_WALL 0x01 // 0000 0001b
#define DIRECTION_1_WALL 0x02 // 0000 0010b
#define DIRECTION_2_WALL 0x04 // 0000 0100b
#define DIRECTION_3_WALL 0x08 // 0000 1000b

short int direction = 1;      // initialize default direction as 1, facing front
short int isExit[2] = {MAX_WIDTH * MAX_WIDTH + 1, 4};          // contain value for isExit, [nodeArray index, wall]
short int numberOfNodes = -1; // counter for number of nodes
short int numberOfMoves = 0;  // counter for number of moves made
short int currentXCoord = 0;  // contain current x-coordinate
short int currentYCoord = 0;  // contain current y-coordinate
short int debtCounter = 0;    // counter for debt list
short int shortestPathArray[MAX_WIDTH * MAX_WIDTH];  // stores the shortest path for the navigation
short int shortestPathCounter = 0;                     // counter for the shortest path array
short int adjMatrix[MAX_WIDTH * MAX_HEIGHT][MAX_WIDTH * MAX_HEIGHT];    // adjacency matrix to store each node's adjacent nodes

// create struct for nodes
// contains x-y coord.
// contain char representing wall opening: default 0000 0000 (use last 4 bits to represent)
struct node
{
    short int xCoord;
    short int yCoord;
    char wallOpenings;
};

// create map array size MAX_WIDTH * MAX_HEIGHT to store node structs
struct node nodeArray[MAX_WIDTH * MAX_HEIGHT];


// create debt array size of total number of lines in grid to store debt coord. and wall
short int debtArray[(MAX_WIDTH * (MAX_HEIGHT + 1)) + (MAX_HEIGHT * (MAX_WIDTH + 1))][3];

// delay function
void delay(int number_of_seconds)
{
    // Converting time into milli_seconds
    int milli_seconds = 1000000 * number_of_seconds;
    // Storing start time
    clock_t start_time = clock();
    // looping till required time is not achieved
    while (clock() < start_time + milli_seconds)
        ;
}

// create path array to store coord. of path taken
// add coord. in when visited
// remove coord. when backtracking
// use numberOfMoves as index for array
short int pathArray[MAX_WIDTH * MAX_HEIGHT][2];

// increment direction
void incrementDirection()
{
    ++direction;
    if (direction == 4)
    {
        direction = 0;
    }
}

// decrement direction
void decrementDirection()
{
    --direction;
    if (direction == -1)
    {
        direction = 3;
    }
}

// check if exit
void checkExit(short int direction, float distance)
{
    if (distance > EXIT_DISTANCE)
    {
        isExit[0] = numberOfNodes;
        isExit[1] = direction;
    }
}

// first grid mapping
// different from mapping the rest of the grids as it requires the car to turn and map the back wall
void firstGridMapping()
{
    // save current coord.
    ++numberOfNodes;
    nodeArray[numberOfNodes].xCoord = currentXCoord;
    nodeArray[numberOfNodes].yCoord = currentYCoord;

    // check left, front, right and save
    if ((ULTRASONIC.FRONT_DISTANCE() < MIN_DISTANCE))
    {
        nodeArray[numberOfNodes].wallOpenings = 0x00 | DIRECTION_1_WALL;
    }
    else
    {
        checkExit(1, ULTRASONIC.FRONT_DISTANCE()); // check if it is an exit
    }
    // check if direction 2 has a wall
    if (ULTRASONIC.RIGHT_DISTANCE() < MIN_DISTANCE)
    {
        nodeArray[numberOfNodes].wallOpenings |= DIRECTION_2_WALL;
    }
    else
    {
        checkExit(2, ULTRASONIC.RIGHT_DISTANCE()); // check if it is an exit
    }
    // check if direction 0 has wall
    if (ULTRASONIC.LEFT_DISTANCE() < MIN_DISTANCE)
    {
        nodeArray[numberOfNodes].wallOpenings |= DIRECTION_0_WALL;
    }
    else
    {
        checkExit(0, ULTRASONIC.LEFT_DISTANCE()); // check if it is an exit
    }

    // after mapping left, front, right wall, turn right to map the last wall
    MOTOR.TURN_RIGHT();
    delay(TURNING_DELAY);   // delay while car turns
    incrementDirection(); // increment direction when turn right
    // checks if direction 3 has a wall
    if (ULTRASONIC.RIGHT_DISTANCE() < MIN_DISTANCE)
    {
        nodeArray[numberOfNodes].wallOpenings |= DIRECTION_3_WALL; // set bitmask with 0000 1000
    }
    else
    {
        checkExit(3, ULTRASONIC.RIGHT_DISTANCE()); // check if it is an exit
    }

    // turn to face nearest open wall that is not an exit
    // check current facing direction first (2)
    // if not, then check direction 0
    // if not, then check direction 2
    // if not, then check direction 1
    // save the other open walls as debt only if they are not an exit

    // there is an opening in the front
    if ((nodeArray[numberOfNodes].wallOpenings & DIRECTION_2_WALL) == 0)
    { 
        // if it is not an exit, save the other open walls into debt
        if (!((isExit[0] == numberOfNodes) && (isExit[1] == 2)))
        {
            
            if ((nodeArray[numberOfNodes].wallOpenings & DIRECTION_3_WALL) == 0)
            {
                if (!((isExit[0] == numberOfNodes) && (isExit[1] == 3))){
                    debtArray[debtCounter][0] = currentXCoord;
                    debtArray[debtCounter][1] = currentYCoord;
                    debtArray[debtCounter][2] = 3;
                    ++debtCounter;
                }
            }
            else if ((nodeArray[numberOfNodes].wallOpenings & DIRECTION_1_WALL) == 0)
            {
                if (!((isExit[0] == numberOfNodes) && (isExit[1] == 1))){
                    debtArray[debtCounter][0] = currentXCoord;
                    debtArray[debtCounter][1] = currentYCoord;
                    debtArray[debtCounter][2] = 1;
                    ++debtCounter;
                }
            }
            else if ((nodeArray[numberOfNodes].wallOpenings & DIRECTION_0_WALL) == 0)
            {
                if (!((isExit[0] == numberOfNodes) && (isExit[1] == 0))){
                    debtArray[debtCounter][0] = currentXCoord;
                    debtArray[debtCounter][1] = currentYCoord;
                    debtArray[debtCounter][2] = 0;
                    ++debtCounter;
                }
            }
        }
    }
    else if ((nodeArray[numberOfNodes].wallOpenings & DIRECTION_3_WALL) == 0)
    { 
        if (!((isExit[0] == numberOfNodes) && (isExit[1] == 3)))
        {
            // there is an opening on the right, turn to face right
            MOTOR.TURN_RIGHT();
            delay(TURNING_DELAY);   // delay while car turns
            incrementDirection();

            if ((nodeArray[numberOfNodes].wallOpenings & DIRECTION_0_WALL) == 0)
            {
                if (!((isExit[0] == numberOfNodes) && (isExit[1] == 0))){
                    debtArray[debtCounter][0] = currentXCoord;
                    debtArray[debtCounter][1] = currentYCoord;
                    debtArray[debtCounter][2] = 0;
                    ++debtCounter;
                }
            }
            else if ((nodeArray[numberOfNodes].wallOpenings & DIRECTION_2_WALL) == 0)
            {
                if (!((isExit[0] == numberOfNodes) && (isExit[1] == 2))){
                    debtArray[debtCounter][0] = currentXCoord;
                    debtArray[debtCounter][1] = currentYCoord;
                    debtArray[debtCounter][2] = 2;
                    ++debtCounter;
                }
            }
        }
    }
    else if ((nodeArray[numberOfNodes].wallOpenings & DIRECTION_1_WALL) == 0)
    { 
        if (!((isExit[0] == numberOfNodes) && (isExit[1] == 1)))
        {
            // there is an opening on the left, turn to face left
            MOTOR.TURN_LEFT();
            delay(TURNING_DELAY);   // delay while car turns
            decrementDirection();
            if ((nodeArray[numberOfNodes].wallOpenings & DIRECTION_0_WALL) == 0)
            {
                if (!((isExit[0] == numberOfNodes)  && (isExit[1] == 0))){
                    debtArray[debtCounter][0] = currentXCoord;
                    debtArray[debtCounter][1] = currentYCoord;
                    debtArray[debtCounter][2] = 0;
                    ++debtCounter;
                }
            }
        }
    }
    else
    { // nearest opening is behind the car, so u-turn
        MOTOR.TURN_RIGHT();
        delay(TURNING_DELAY);   // delay while car turns
        incrementDirection();
        MOTOR.TURN_RIGHT();
        delay(TURNING_DELAY);   // delay while car turns
        incrementDirection();
    }

    // call car movement method to add to pathArray, increment no. of moves, move car forward, change x-y coord.
    carMovement();
};


// peripheral checking, mask and set here
void peripheralChecking()
{
    // flag to check if node has been visited
    short int flag = 1; // true means it is a new node

    for (short int i = 0; i < numberOfNodes + 1; i++)
    {
        // check if this node has been mapped into node array
        if ((nodeArray[i].xCoord == currentXCoord) && (nodeArray[i].yCoord == currentYCoord))
        {
            // set flag to false if it has been mapped
            flag = 0;
        }
    }
    
    // if flag is true, the node has not been mapped, start mapping 4 sides of the wall
    if (flag == 1)
    {
        // save current coord.
        ++numberOfNodes;
        nodeArray[numberOfNodes].xCoord = currentXCoord; 
        nodeArray[numberOfNodes].yCoord = currentYCoord;

        // set opening counter to 0
        short int openingCounter = 0;

        switch (direction)
        {
        case 0: // facing direction 0
            // check if direction 3 has wall
            if (ULTRASONIC.FRONT_DISTANCE() < MIN_DISTANCE)
            {
                nodeArray[numberOfNodes].wallOpenings = 0x00 | DIRECTION_0_WALL;
            }
            else
            {
                checkExit(0, ULTRASONIC.FRONT_DISTANCE()); // check if it is an exit
                if (!((isExit[0] == numberOfNodes) && (isExit[1] == 0))){
                    ++openingCounter;                          // increment opening counter
                }
            }
            // check if direction 0 has a wall
            if (ULTRASONIC.RIGHT_DISTANCE() < MIN_DISTANCE)
            {
                nodeArray[numberOfNodes].wallOpenings |= DIRECTION_1_WALL;
            }
            else
            {
                checkExit(1, ULTRASONIC.RIGHT_DISTANCE()); // check if it is an exit
                // if there is more than 1 opening
                if (!((isExit[0] == numberOfNodes) && (isExit[1] == 1))) {
                    if (openingCounter >= 1)
                    {
                        debtArray[debtCounter][0] = currentXCoord;
                        debtArray[debtCounter][1] = currentYCoord;
                        debtArray[debtCounter][2] = 1;
                        ++debtCounter;
                    }
                }
            }
            // check if direction 2 has wall
            if (ULTRASONIC.LEFT_DISTANCE() < MIN_DISTANCE)
            {
                nodeArray[numberOfNodes].wallOpenings |= DIRECTION_3_WALL;
            }
            else
            {
                checkExit(3, ULTRASONIC.LEFT_DISTANCE()); // check if it is an exit
                // if there is more than 1 opening
                if (!((isExit[0] == numberOfNodes) && (isExit[1] == 3)))
                {
                    if (openingCounter >= 1)
                    {
                        debtArray[debtCounter][0] = currentXCoord;
                        debtArray[debtCounter][1] = currentYCoord;
                        debtArray[debtCounter][2] = 3;
                        ++debtCounter;
                    }
                }
            }
            break;
        case 1: // facing direction 1
            // check if direction 1 has wall
            if (ULTRASONIC.FRONT_DISTANCE() < MIN_DISTANCE)
            {
                nodeArray[numberOfNodes].wallOpenings = 0x00 | DIRECTION_1_WALL;
            }
            else
            {
                checkExit(1, ULTRASONIC.FRONT_DISTANCE()); // check if it is an exit
                if (!((isExit[0] == numberOfNodes) && (isExit[1] == 1))){
                    ++openingCounter;                          // increment opening counter
                }
            }
            // check if direction 2 has a wall
            if (ULTRASONIC.RIGHT_DISTANCE() < MIN_DISTANCE)
            {
                nodeArray[numberOfNodes].wallOpenings |= DIRECTION_2_WALL;
            }
            else
            {
                checkExit(2, ULTRASONIC.RIGHT_DISTANCE()); // check if it is an exit
                // if there is more than 1 opening
                if (!((isExit[0] == numberOfNodes) && (isExit[1] == 2)))
                {
                    if (openingCounter >= 1)
                    {
                        debtArray[debtCounter][0] = currentXCoord;
                        debtArray[debtCounter][1] = currentYCoord;
                        debtArray[debtCounter][2] = 2;
                        ++debtCounter;
                    }
                }
            }
            // check if direction 0 has wall
            if (ULTRASONIC.LEFT_DISTANCE() < MIN_DISTANCE)
            {
                nodeArray[numberOfNodes].wallOpenings |= DIRECTION_0_WALL;
            }
            else
            {
                checkExit(0, ULTRASONIC.LEFT_DISTANCE()); // check if it is an exit
                // if there is more than 1 opening
                if (!((isExit[0] == numberOfNodes) && (isExit[1] == 0)))
                {
                    if (openingCounter >= 1)
                    {
                        debtArray[debtCounter][0] = currentXCoord;
                        debtArray[debtCounter][1] = currentYCoord;
                        debtArray[debtCounter][2] = 0;
                        ++debtCounter;
                    }
                }
            }
            break;
        case 2: // facing direction 2
            // check if direction 2 has wall
            if (ULTRASONIC.FRONT_DISTANCE() < MIN_DISTANCE)
            {
                nodeArray[numberOfNodes].wallOpenings = 0x00 | DIRECTION_2_WALL;
            }
            else
            {
                checkExit(2, ULTRASONIC.FRONT_DISTANCE()); // check if it is an exit
                if (!((isExit[0] == numberOfNodes) && (isExit[1] == 2))){
                    ++openingCounter;                          // increment opening counter
                }
            }
            // check if direction 3 has a wall
            if (ULTRASONIC.RIGHT_DISTANCE() < MIN_DISTANCE)
            {
                nodeArray[numberOfNodes].wallOpenings |= DIRECTION_3_WALL;
            }
            else
            {
                checkExit(3, ULTRASONIC.RIGHT_DISTANCE()); // check if it is an exit
                // if there is more than 1 opening
                if (!((isExit[0] == numberOfNodes) && (isExit[1] == 3)))
                {
                    if (openingCounter >= 1)
                    {
                        debtArray[debtCounter][0] = currentXCoord;
                        debtArray[debtCounter][1] = currentYCoord;
                        debtArray[debtCounter][2] = 3;
                        ++debtCounter;
                    }
                }
            }
            // check if direction 1 has wall
            if (ULTRASONIC.LEFT_DISTANCE() < MIN_DISTANCE)
            {
                nodeArray[numberOfNodes].wallOpenings |= DIRECTION_1_WALL;
            }
            else
            {
                checkExit(1, ULTRASONIC.LEFT_DISTANCE()); // check if it is an exit
                // if there is more than 1 opening
                if (!((isExit[0] == numberOfNodes) && (isExit[1] == currentYCoord) && (isExit[1] == 1)))
                {
                    if (openingCounter >= 1)
                    {
                        debtArray[debtCounter][0] = currentXCoord;
                        debtArray[debtCounter][1] = currentYCoord;
                        debtArray[debtCounter][2] = 1;
                        ++debtCounter;
                    }
                }
            }
            break;
        case 3: // facing direction 3
            // check if direction 3 has wall
            if (ULTRASONIC.FRONT_DISTANCE() < MIN_DISTANCE)
            {
                nodeArray[numberOfNodes].wallOpenings = 0x00 | DIRECTION_3_WALL;
            }
            else
            {
                checkExit(3, ULTRASONIC.FRONT_DISTANCE()); // check if it is an exit
                if (!((isExit[0] == numberOfNodes) && (isExit[1] == 3))){
                    ++openingCounter;                          // increment opening counter
                }
            }
            // check if direction 0 has a wall
            if (ULTRASONIC.RIGHT_DISTANCE() < MIN_DISTANCE)
            {
                nodeArray[numberOfNodes].wallOpenings |= DIRECTION_0_WALL;
            }
            else
            {
                checkExit(0, ULTRASONIC.RIGHT_DISTANCE()); // check if it is an exit
                // if there is more than 1 opening
                if (!((isExit[0] == numberOfNodes) && (isExit[1] == 0)))
                {
                    if (openingCounter >= 1)
                    {
                        debtArray[debtCounter][0] = currentXCoord;
                        debtArray[debtCounter][1] = currentYCoord;
                        debtArray[debtCounter][2] = 0;
                        ++debtCounter;
                    }
                }
            }
            // check if direction 2 has wall
            if (ULTRASONIC.LEFT_DISTANCE() < MIN_DISTANCE)
            {
                nodeArray[numberOfNodes].wallOpenings |= DIRECTION_2_WALL;
            }
            else
            {
                checkExit(2, ULTRASONIC.LEFT_DISTANCE()); // check if it is an exit
                // if there is more than 1 opening
                if (!((isExit[0] == numberOfNodes) && (isExit[1] == 2)))
                {
                    if (openingCounter >= 1)
                    {
                        debtArray[debtCounter][0] = currentXCoord;
                        debtArray[debtCounter][1] = currentYCoord;
                        debtArray[debtCounter][2] = 2;
                        ++debtCounter;
                    }
                }
            }
            break;

        default:
            break;
        }
    }
    else
    {
        // make a u-turn
        MOTOR.TURN_RIGHT();
        delay(TURNING_DELAY);   // delay while car turns
        incrementDirection();
        MOTOR.TURN_RIGHT();
        delay(TURNING_DELAY);   // delay while car turns
        incrementDirection();
        // call debt visiting
        debtVisit();
    }
}

// change coord.
void changeCoord()
{
    switch (direction)
    {
    case 0:
        // facing the left, decrement x coord
        --currentXCoord;
        break;
    case 1:
        // facing the front, increment y coord
        ++currentYCoord;
        break;
    case 2:
        // facing the right, increment x coord
        ++currentXCoord;
        break;
    case 3:
        // facing the back, decrement y coord
        --currentYCoord;
        break;
    }
}

// car movement
void carMovement()
{
    // save coord. into pathArray
    pathArray[numberOfMoves][0] = currentXCoord;
    pathArray[numberOfMoves][1] = currentYCoord;

    numberOfMoves++; // increment number of moves

    MOTOR.MOVE_FORWARD();
    delay(MOVEMENT_DELAY);   // delay while car moves

    // change x-y coord.
    changeCoord();
}

// car turning
// if forward is not blocked, remain
// if forward is blocked, turn right and increment direction variable
// if forward and right is blocked, turn left and decrement direction variable
// push current coords. to path stack
// call car move forward
// if all 3 sides are blocked, turn right twice and increment direction variable by 2
// call car to visit latest debt
void carTurning()
{
    if ((ULTRASONIC.FRONT_DISTANCE() > MIN_DISTANCE) && (ULTRASONIC.FRONT_DISTANCE() < EXIT_DISTANCE))
    { // there is an opening in the front
        // call car movement
        carMovement();
    }
    // check if direction 3 has a wall, if it is empty, move forward
    else if ((ULTRASONIC.RIGHT_DISTANCE() > MIN_DISTANCE) && (ULTRASONIC.RIGHT_DISTANCE() < EXIT_DISTANCE))
    { //
        MOTOR.TURN_RIGHT();
        delay(TURNING_DELAY);   // delay while car turns
        incrementDirection();
        // call car movement
        carMovement();
    }
    // check if direction 1 has a wall, if it is empty, move forward
    else if ((ULTRASONIC.LEFT_DISTANCE() > MIN_DISTANCE) && (ULTRASONIC.LEFT_DISTANCE() < EXIT_DISTANCE))
    {
        MOTOR.TURN_LEFT();
        delay(TURNING_DELAY);   // delay while car turns
        decrementDirection();
        // call car movement
        carMovement();
    }
    // else, make a u-turn
    else
    {
        MOTOR.TURN_RIGHT();
        delay(TURNING_DELAY);   // delay while car turns
        incrementDirection();
        MOTOR.TURN_RIGHT();
        delay(TURNING_DELAY);   // delay while car turns
        incrementDirection();
        // call debt visiting
        debtVisit();
    }
}

// debt car turning
void debtCarTurning(short int target)
{
    while (direction != target)
    {
        if (target == 3 && direction == 0)
        {
            MOTOR.TURN_LEFT();
            delay(TURNING_DELAY);   // delay while car turns
            decrementDirection();
        }
        else if (target == 0 && direction == 3)
        {
            MOTOR.TURN_RIGHT();
            delay(TURNING_DELAY);   // delay while car turns
            incrementDirection();
        }
        else
        {
            if (target > direction)
            {
                MOTOR.TURN_RIGHT();
                delay(TURNING_DELAY);   // delay while car turns
                incrementDirection();
            }
            else
            {
                MOTOR.TURN_LEFT();
                delay(TURNING_DELAY);   // delay while car turns
                decrementDirection();
            }
        }
    }
}

// car visiting debt
// check latest debt coord. in debt list, save into variable
// while loop to see if reached debt coord.
// check last pathArray for previous coord.
// compare x-y coord. of current grid to previous coord., see which direction to turn from there
// call car move forward
// remove last pathArray coords. from path stack
// once reached, remove debt from debt list
// move forward
// call peripheral checking
// go back to main while loop to explore
void debtVisit()
{
    // check latest debt coord. in debt list, save into variable
    short int debt_coord[3] = {debtArray[debtCounter][0], debtArray[debtCounter][1], debtArray[debtCounter][2]};
    // while loop to see if reached debt coord.
    while ((currentXCoord != debt_coord[0]) && (currentYCoord != debt_coord[1]))
    {
        //
        if (currentXCoord != pathArray[numberOfMoves][0])
        {
            if (currentXCoord > pathArray[numberOfMoves][0])
            {
                // call function to turn
                debtCarTurning(0);
            }
            else
            {
                // call function to turn
                debtCarTurning(2);
            }
        }

        else if (currentYCoord != pathArray[numberOfMoves][1])
        {
            if (currentYCoord > pathArray[numberOfMoves][1])
            {
                // call function to turn
                debtCarTurning(3);
            }
            else
            {
                // call function to turn
                debtCarTurning(1);
            }
        }

        // make car move forward
        MOTOR.MOVE_FORWARD();
        delay(MOVEMENT_DELAY);   // delay while car moves
        changeCoord();
        // remove from pathArray as we are backtracking
        pathArray[numberOfMoves][0] = NULL;
        pathArray[numberOfMoves][1] = NULL;
        ++numberOfMoves;
    }
    // turn car to face debt open wall
    debtCarTurning(debt_coord[2]);
    // remove debt
    debtArray[debtCounter][0] = NULL;
    debtArray[debtCounter][1] = NULL;
    --debtCounter;
    // make car move forward
    MOTOR.MOVE_FORWARD();
    delay(MOVEMENT_DELAY);   // delay while car moves
    // change coord.
    changeCoord();
}





//------------------------------------------------------------------------------------------------------------
// NAVIGATION

// A utility function to find the vertex with minimum distance
// value, from the set of vertices not yet included in shortest
// path tree

short int saveStartNode (){
    // get from communications module
}

short int saveNavDirection (){
    // get from communications module
}

int minDistance(int dist[], bool sptSet[])
{
    // Initialize min value
    int min = INT_MAX, min_index;
 
    for (int v = 0; v < numberOfNodes + 1; v++)
        if (sptSet[v] == false && dist[v] <= min)
            min = dist[v], min_index = v;
 
    return min_index;
}
 
// Function to print shortest path from source to j
// using parent array
void printPath(int parent[], int j)
{
    // Base Case : If j is source
    if (parent[j]==-1)
        return;
 
    // printPath(parent, parent[j]);
    shortestPathArray[shortestPathCounter] = parent[j];
    shortestPathCounter++;
}
 
// A utility function to print the constructed distance
// array
int printSolution(int dist[], int n, int parent[], short int dst)
{
    int src = 0;
    printf("Vertex\t  Distance\tPath");
    for (int i = 1; i < numberOfNodes + 1; i++)
    {
        if (i == dst)
        {
            // printf("\n%d -> %d \t\t %d\t\t%d ", src, i, dist[i], src);
            printPath(parent, i);
        }
    }
    shortestPathArray[shortestPathCounter] = dst;
}
 
// Funtion that implements Dijkstra's single source shortest path
// algorithm for a graph represented using adjacency matrix
// representation
void dijkstra(int graph[numberOfNodes + 1][numberOfNodes + 1], short int src, short int dst)
{
    int dist[numberOfNodes + 1];  // The output array. dist[i] will hold
                  // the shortest distance from src to i
 
    // sptSet[i] will true if vertex i is included / in shortest
    // path tree or shortest distance from src to i is finalized
    bool sptSet[numberOfNodes + 1];
 
    // Parent array to store shortest path tree
    int parent[numberOfNodes + 1];
 
    // Initialize all distances as INFINITE and stpSet[] as false
    for (int i = 0; i < numberOfNodes + 1; i++)
    {
        parent[0] = -1;
        dist[i] = INT_MAX;
        sptSet[i] = false;
    }
 
    // Distance of source vertex from itself is always 0
    dist[src] = 0;
 
    // Find shortest path for all vertices
    for (int count = 0; count < numberOfNodes + 1; count++)
    {
  
        // Pick the minimum distance vertex from the set of
        // vertices not yet processed. u is always equal to src
        // in first iteration.
        int u = minDistance(dist, sptSet);

        // Mark the picked vertex as processed
        sptSet[u] = true;

        // Update dist value of the adjacent vertices of the
        // picked vertex.
        for (int v = 0; v < numberOfNodes + 1; v++)
            // Update dist[v] only if is not in sptSet, there is
            // an edge from u to v, and total weight of path from
            // src to v through u is smaller than current value of
            // dist[v]
            if (!sptSet[v] && graph[u][v] &&
                dist[u] + graph[u][v] < dist[v])
            {
                parent[v]  = u;
                dist[v] = dist[u] + graph[u][v];
            }  
        
        
    }
    
    // print the constructed distance array
    printSolution(dist, numberOfNodes + 1, parent, dst);
}
 
// Initialize the matrix to zero
void init(int arr[][numberOfNodes + 1])
{
  int i, j;
  for (i = 0; i < numberOfNodes + 1; i++)
    for (j = 0; j < numberOfNodes + 1; j++)
      arr[i][j] = 0;
}

void saveToMatrix(){
    // create a matrix

    // node array's first item will be vertex 0
    // check the wall opening of current node
    // loop through node array's wall opening
    for (short int i = 0; i < numberOfNodes + 1; i++)
    {
        
        // if wall 0 is open, set that there is a path between current x coord and x-1 coord, y stays the same (to the left)
        if ((nodeArray[i].wallOpenings & DIRECTION_0_WALL) == 0){
            for (short int j = 0; j < numberOfNodes + 1; j++)
            {   
                if ((nodeArray[j].xCoord == nodeArray[i].xCoord - 1) && (nodeArray[j].yCoord == nodeArray[i].yCoord)){
                    adjMatrix[i][j] = 1;
                }
            }           
        }
        // if wall 1 is open, set that there is a path between current y coord and y+1 coord, x stays the same (up)
        else if ((nodeArray[i].wallOpenings & DIRECTION_1_WALL) == 0){
            for (short int j = 0; j < numberOfNodes + 1; j++)
            {   
                if ((nodeArray[j].xCoord == nodeArray[i].xCoord) && (nodeArray[j].yCoord == nodeArray[i].yCoord + 1)){
                    adjMatrix[i][j] = 1;
                    
                }
            }           
        }
        // if wall 2 is open, set that there is a path between current x coord and x+1 coord, y stays the same (to the right)
        else if ((nodeArray[i].wallOpenings & DIRECTION_2_WALL) == 0){
            for (short int j = 0; j < numberOfNodes + 1; j++)
            {   
                if ((nodeArray[j].xCoord == nodeArray[i].xCoord + 1) && (nodeArray[j].yCoord == nodeArray[i].yCoord)){
                    adjMatrix[i][j] = 1;
                }
            } 
        }
        // if wall 3 is open, set that there is a path between current y coord and y-1 coord, x stays the same (down)
        else if ((nodeArray[i].wallOpenings & DIRECTION_3_WALL) == 0){
            for (short int j = 0; j < numberOfNodes + 1; j++)
            {   
                if ((nodeArray[j].xCoord == nodeArray[i].xCoord) && (nodeArray[j].yCoord == nodeArray[i].yCoord - 1)){
                    adjMatrix[i][j] = 1;
                }
            }      
        } 
    }
}



// with the calculated shortest path, do navigation using x-y coord. to know which direction to turn for each path node
// turn to saved exit node's wall and move forward
// take in shortest path array as parameter (array of vertices), count of shortest path array, and current facing direction
void navToExit(short int shortestPathArray[], short int shortestPathCounter, short int navDirection){
    // set direction to navDirection
    direction = navDirection;
    
    // loop through shortestPath array
    for (short int i = 0; i < shortestPathCounter; i++)
    {
        // set current x-y coord. to first item in shortestPath array
        short int currentXCoord = nodeArray[shortestPathArray[i]].xCoord;
        short int currentYCoord = nodeArray[shortestPathArray[i]].yCoord;

        short int targetXCoord = nodeArray[shortestPathArray[i + 1]].xCoord;
        short int targetYCoord = nodeArray[shortestPathArray[i + 1]].yCoord;

        if (currentXCoord != targetXCoord){
            if (currentXCoord < targetXCoord){
                debtCarTurning(2);
            }
            else{
                debtCarTurning(0);
            }
        }
        if (currentYCoord != targetYCoord){
            if (currentYCoord < targetYCoord){ 
                debtCarTurning(1);
            }
            else{
                debtCarTurning(3);
            }
        }

        // make car move forward
        MOTOR.MOVE_FORWARD();
        delay(MOVEMENT_DELAY);   // delay while car moves
    }
    
    // turn to face exit direction
    short int exitDirection = isExit[1];
    while (direction != exitDirection)
    {
        if (exitDirection == 3 && direction == 0)
        {
            MOTOR.TURN_LEFT();
            delay(TURNING_DELAY);   // delay while car turns
            decrementDirection();
        }
        else if (exitDirection == 0 && direction == 3)
        {
            MOTOR.TURN_RIGHT();
            delay(TURNING_DELAY);   // delay while car turns
            incrementDirection();
        }
        else
        {
            if (exitDirection > direction)
            {
                MOTOR.TURN_RIGHT();
                delay(TURNING_DELAY);   // delay while car turns
                incrementDirection();
            }
            else
            {
                MOTOR.TURN_LEFT();
                delay(TURNING_DELAY);   // delay while car turns
                decrementDirection();
            }
        }
    }

    // make car move forward, and you have exited the maze
    MOTOR.MOVE_FORWARD();
    delay(MOVEMENT_DELAY);   // delay while car moves
}


// MAIN FUNCTION TO START MAPPING

int startMapping(){
    firstGridMapping(); // call method to map first grid

    // while loop until it is fully mapped
    while (1)
    {
        // call method to check peripheral with sensors
        peripheralChecking();

        // if number of nodes mapped == total map grids, then mapping is complete
        if (numberOfNodes == ((MAX_HEIGHT * MAX_WIDTH) - 1))
        {
            printf("\nMapping completed.");
            print("\nSize of map (node array): %d", sizeof(nodeArray));
            return 0;
        }

        // call method to make car turn and move
        carTurning();
    }

    return 0;
}


// MAIN FUNCTION TO START NAVIGATING
int startNavigating(){
    // save nodeArray to matrix
    saveToMatrix();

    // start point given by communication
    dijkstra(adjMatrix, saveStartNode(), isExit[0]);  //return shortest path, shortest path counter

    // navDirection given by communication
    navToExit(shortestPathArray, shortestPathCounter, saveNavDirection());  

    printf("\nNavigation completed.");
}


int main()
{
    // call main function to start mapping
    startMapping();

    //call main function to start navigating
    startNavigating();

    return 0;
}