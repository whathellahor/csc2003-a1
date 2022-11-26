#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>

#define MAX_WIDTH 5          // set max width of the maze to 5
#define MAX_HEIGHT 4         // set max height of the maze to 4
#define MOVEMENT_DISTANCE 30 // set default distance to move forward one grid as 30cm
#define MIN_DISTANCE 5       // set default distance to check for wall to 5cm
#define EXIT_DISTANCE 180    // set default distance for exit to be 180cm

// perform AND or OR operation with these to check for wall
#define DIRECTION_0_WALL 0x01 // 0000 0001b
#define DIRECTION_1_WALL 0x02 // 0000 0010b
#define DIRECTION_2_WALL 0x04 // 0000 0100b
#define DIRECTION_3_WALL 0x08 // 0000 1000b

#define TRUE 1
#define FALSE 0

short int direction = 1;      // initialize default direction as 1, facing front
short int isExit[2] = {MAX_WIDTH * MAX_WIDTH + 1, 4};          // contain value for isExit, [nodeArray index, wall]
short int numberOfNodes = -1; // counter for number of nodes
short int numberOfMoves = 0;  // counter for number of moves made
short int currentXCoord = 0;  // contain current x-coordinate
short int currentYCoord = 0;  // contain current y-coordinate
short int debtCounter = 0;    // counter for debt list
short int shortestPathArray[MAX_WIDTH * MAX_WIDTH];  // stores the shortest path for the navigation
short int shortestPathCounter = 0;                     // counter for the shortest path array
short int adjMatrix[MAX_WIDTH * MAX_HEIGHT][MAX_WIDTH * MAX_HEIGHT];

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


// create path array to store coord. of path taken
// add coord. in when visited
// remove coord. when backtracking
// use numberOfMoves as index for array
short int pathArray[MAX_WIDTH * MAX_HEIGHT][2];

// check if exit
void checkExit(short int direction, float distance)
{
    if (distance > EXIT_DISTANCE)
    {
        isExit[0] = numberOfNodes;
        isExit[1] = direction;
    }
}

// peripheral checking, mask and set here
void peripheralChecking()
{
    // flag to check if node has been visited
    short int flag = TRUE; // true means it is a new node

    for (short int i = 0; i < numberOfNodes + 1; i++)
    {
        // check if this node has been mapped into node array
        if ((nodeArray[i].xCoord == currentXCoord) && (nodeArray[i].yCoord == currentYCoord))
        {
            // set flag to false if it has been mapped
            flag = FALSE;
        }
    }
    
    // if flag is true, the node has not been mapped, start mapping 4 sides of the wall
    if (flag == TRUE)
    {
        // save current coord.
        numberOfNodes++;
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
                    openingCounter++;                          // increment opening counter
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
                        debtCounter++;
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
                        debtCounter++;
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
                    openingCounter++;                          // increment opening counter
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
                        debtCounter++;
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
                        debtCounter++;
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
                    openingCounter++;                          // increment opening counter
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
                        debtCounter++;
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
                        debtCounter++;
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
                    openingCounter++;                          // increment opening counter
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
                        debtCounter++;
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
                        debtCounter++;
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
        // // make a u-turn
        // MOTOR.TURN_RIGHT();
        // incrementDirection();
        // MOTOR.TURN_RIGHT();
        // incrementDirection();
        // // call debt visiting
        // debtVisit();
    }
}

int main(){
    peripheralChecking();

    for (int i = 0; i < numberOfNodes; i++)
    {
        printf("x,y, coords: %d, %d\n", nodeArray[i].xCoord,nodeArray[i].yCoord);
        printf("wall openings: %d\n", nodeArray[i].wallOpenings & 0x02 == 1);
    }
}