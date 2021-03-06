/*
* MIT License
* Copyright (c) 2017 Carsten Hagemann, Niklas Portmann, Felix Bertsch
* See LICENSE file.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

// Beschreibung der Aufgabe: https://drive.google.com/file/d/0BzRp-cLiZDUJcWNUWDdVN3F3SjQ/view?usp=sharing

typedef struct {
    int x;
    int y;
} coord;

typedef struct {
    coord position;
    int possibleSteps;
} extCoord;

bool *board;
int sizeX, sizeY;
int *steps;

const coord invalid = { .x = -1, .y = -1};

//Algorithmus

/*
 * Returns whether a calculated position is withing the board boundaries.
 *
 * coord pos: The position which is checked.
 */
bool checkValid(coord pos) {
    return pos.x < sizeX && pos.y < sizeY && pos.x >= 0 && pos.y >= 0;
}

/*
 * Returns whether a field has already been visited or not.
 *
 * coord pos: The position which is checked.
 */
bool getFieldVal(coord pos) {
    if (checkValid(pos)) {
        return *(board + pos.y*sizeX + pos.x);
    } else {
        return true;
    }
}

/*
 * Sets the value of a field to true or false.
 *
 * coord pos: The position which is set.
 */
void setFieldVal(coord pos, bool val) {
    *(board + pos.y*sizeX + pos.x) = val;
}

/*
 * Returns a coordinate, which is calculated from the fieldnumber and the
 * given position.
 *
 * coord pos: the given position
 *
 * int fieldNumber: indicates which coordinate relative to the position will
 *                  be returned
 */
coord getFieldByNumber(coord pos, int fieldNumber, int modifier) {
    fieldNumber = (fieldNumber + modifier)%8;
    if (modifier >= 8) fieldNumber = (fieldNumber + 4)%8;
    switch (fieldNumber) {
        case 0:
            pos.x += 2;
            pos.y += 1;
            break;
        case 1:
            pos.x += 2;
            pos.y -= 1;
            break;
        case 2:
            pos.x -= 2;
            pos.y += 1;
            break;
        case 3:
            pos.x -= 2;
            pos.y -= 1;
            break;
        case 4:
            pos.x += 1;
            pos.y += 2;
            break;
        case 5:
            pos.x += 1;
            pos.y -= 2;
            break;
        case 6:
            pos.x -= 1;
            pos.y += 2;
            break;
        case 7:
            pos.x -= 1;
            pos.y -= 2;
            break;
    }
    return pos;
}

/*
 * Adds a step to the step array for the result output.
 *
 * int step: The number of the step which is added.
 *
 * coord pos: Coordinates of the step which is added.
 */
bool addStepToSteps(coord pos, int step) {
    if (step < sizeX * sizeY) {
        *(steps + pos.y*sizeX + pos.x) = step;
        return true;
    } else {
        exit(1);
    }
}

/*
 * Returns how many steps can be performed from this field
 * after having moved one step further.
 *
 * coord initial: The field from which the steps are counted.
 */
int countPossibleSteps(coord initial) {
    int result = 0;
    coord buffer;
    for (int i = 0; i < 8; i++) {
        buffer = getFieldByNumber(initial, i, 0);
        if(!getFieldVal(buffer)) {
            result++;
        }
    }
    return result;
}

int compare (const void * a, const void * b) {
   extCoord aExt = *(extCoord*)a;
   extCoord bExt = *(extCoord*)b;
   int res = aExt.possibleSteps - bExt.possibleSteps;
   
   if (res == 0) {
       // If two positions have the same amount of possible steps,
       // the Euclidian distance between each position and the
       // middlepoint of the board are compared to speed up
       // the algorithm.
       double distA = sqrt( pow((sizeX-1)/2.0 - aExt.position.x,2) + pow((sizeY-1)/2.0 - aExt.position.y,2));
       double distB = sqrt( pow((sizeX-1)/2.0 - bExt.position.x,2) + pow((sizeY-1)/2.0 - bExt.position.y,2));
       return distA - distB;
   } else {
       return res;
   }
}

/*
 * Returns if a knights tour is possible and fills the steps array
 * with the route of the knights tour.
 *
 * coord pos: Current position of the method.
 *
 * final pos: Final position which the method tries to reach, if there
 *            is no given final position it is -1 , -1.
 *
 * int counter: Counts the moved steps to check if the method is finished,
 *           the starting value is 0.
 *
 * int *tries: pointer to an int which displays the left tries, until the
 *             method returns false and terminates.
 *
 * int modifier: Modifies the order in which the next step is calculated.
 */
bool backTrackingAlgorithm(coord pos, coord final, int counter, int modifier) {
    setFieldVal(pos, true);

    if (counter == (sizeX * sizeY - 1)) {
    	for (int i = 0; i < 8; i++) {
    		coord buffer = getFieldByNumber(pos, i, 0);
    		if ((buffer.x == final.x && buffer.y == final.y) 
    		    || (final.x == -1 && final.y == -1)) {
    	        addStepToSteps(pos, counter);
    	        return true;
    		}
    	}
    	setFieldVal(pos, false);
    	return false;
    }

    extCoord followingSteps[8] =
    {
        {.possibleSteps = -1}, {.possibleSteps = -1},
        {.possibleSteps = -1}, {.possibleSteps = -1},
        {.possibleSteps = -1}, {.possibleSteps = -1},
        {.possibleSteps = -1}, {.possibleSteps = -1}
    };

    // Tries to execute all possible moves from the current field.
    for (int i = 0; i < 8; i++) {
        coord buffer = getFieldByNumber(pos, i, modifier);

        if (!getFieldVal(buffer)) {
            followingSteps[i].possibleSteps = countPossibleSteps(buffer);
            followingSteps[i].position = buffer;
        }
    }
    
    // Sort the positions from where the least amount
    // of steps are possible to speed up the algorithm.
    qsort(followingSteps, 8, sizeof(extCoord), compare);

    for (int j = 0; j < 8; j++) {
        if (followingSteps[j].possibleSteps != -1) {
            if (backTrackingAlgorithm(followingSteps[j].position, final,
                                       counter + 1, modifier)) {
                addStepToSteps(pos, counter);
                return true;
            }
        }
    }

    setFieldVal(pos, false);
    return false;
}

/* Returns if a solution is possible, and fills the steps array with the
 * needed steps to perform the knights tour. Tries different modifier for 
 * the backtracking method.
 *
 * coord pos: Current position of the method.
 *
 * final pos: Final position which the method tries to reach, if there
 */
bool backTracking(coord initial, coord final) {
    /*
    int maxTries = sizeX * sizeY - 1;
    while (true) {
        for (int i = 0; i < 8; i++) {
            int tries = maxTries;
            if (backTrackingAlgorithm(initial, final, 0, &tries, i)) return true;
            resetBoardAndSteps();

        }
        maxTries *= 2;
    }
    */
    return backTrackingAlgorithm(initial, final, 0 , 0);
}

/*
 * Returns if there is a possible solution for the knights tour
 * on the current field size.
 * If found, the solution is saved in steps.
 *
 * coord initial: Coordinate at which the knights tour starts.
 */
bool startBackTracking(coord initial) {
    if( (sizeX % 2 != 0 && sizeY % 2 != 0) && (initial.x + initial.y) % 2 != 0 ) {
        printf("This position combined with this specific board size has no solution.\n");
        return false;
    }
    return backTracking(initial, invalid);
}

/*
 * Returns true if there is a possible solution for a closed knights tour
 * on the current field size.
 * If found, the solution is saved in steps.
 *
 * coord initial: Coordinate at which the knights tour starts and ends.
 */
bool startBackTrackingClosed(coord initial) {
	// https://de.wikipedia.org/wiki/Springerproblem#Schwenksches_Theorem
	if( (sizeX % 2 != 0 && sizeY % 2 != 0) || (sizeX==1||sizeX==2||sizeX==4) 
	    || (sizeX==3 && (sizeY==4||sizeY==6||sizeY==8) ) ) {
	    printf("This specific board size does not have a closed solution.\n");
	    return false;
	}
    return backTracking(initial, initial);
}

// Input & Output

/*
 * (Re-)Initialize board and step array with the values 0.
 */
void resetBoardAndSteps() {
    board = (bool *)calloc(sizeX * sizeY, sizeof(bool));
    steps = (int *)calloc(sizeX * sizeY, sizeof(int));
}

/*
 * Returns how many digits one number has.
 *
 * int num: The number of which the digits are counted.
 */
int lengthInt(int num) {
    int result = 0;
    while (num != 0 ) {
        result++;
        num /= 10;
    }
    return result;
}

/*
 * Prints out a grid of the order with which the Knight moved.
 */
void printSteps() {
    int stepsAmount = sizeX*sizeY;
    int magnitude = lengthInt(stepsAmount);
    
    char printfFormat[30] = "%0";
    char magnitudeString[10];
    
    /*
     * printfFormat = "%0Xd\n"
     * X is the number of digits that the largest step contains
     * Used to print out leading 0's so the grid stays aligned
     */
    
    sprintf(magnitudeString, "%d", magnitude);
    strcat(printfFormat, magnitudeString);
    strcat(printfFormat, "d  ");
     
    for(int x = 0; x < sizeX; x++) {
        for(int y = 0; y < sizeY; y++) {
            printf(printfFormat, *(steps + y*sizeX + x) + 1);
        }
        printf("\n");
        printf("\n");
    }

    printf("\n");
}


/*
 * Flushes the standard input stream to "clean" unused input
 * 
 * Source: http://stackoverflow.com/a/27281028/3991578
 */
void flushStdIn() {
  int ch;
  while(((ch = getchar()) !='\n') && (ch != EOF));
}

/*
 * Prompts for an integer between 0 and an upper Limit
 * 
 * char prompt[]: String that is displayed to the user as a prompt
 * 
 * int upperLimit: Limits the integer which can be input
 */
int promptForDigitsWithLimit(char prompt[], int upperLimit) {
    while(true) {
        int input;
        printf("%s: ", prompt);
        if (scanf("%d", &input) <= 0) {
            printf("Input must be a number.\n");
        } else if (input > upperLimit && upperLimit != -1) {
            printf("Number can't be greater than %d.\n", upperLimit);
        } else if (input <= 0) {
            printf("Number must be greater than 0.\n");
        } else {
            return input;
        }
        flushStdIn();
    }
}

/*
 * Prompt for an integer larger than 0
 * 
 * char prompt[]: String that is displayed to the user as a prompt
 */
int promptForDigits(char prompt[]) {
    return promptForDigitsWithLimit(prompt, -1);
}

/*
 * Prompts the user for the board size and sets the according variables
 */
void setupBoardSize() {
    sizeX = promptForDigits("Board Size (x)");
    sizeY = promptForDigits("Board Size (y)");
}

/*
 * Prompts the user for the Initial or Destination X and Y Positions
 * and returns a coord variable with these values
 *
 * char option[]: Prefixes the prompt to indicateIntitial/Destination positions
 */
coord setupPosition(char option[]) {
    int length = strlen(option) + 12;
    char *stringXPos = malloc(length * sizeof(char));
    char *stringYPos = malloc(length * sizeof(char));
    strcat(stringXPos, option);
    strcat(stringXPos, " X Position");
    strcat(stringYPos, option);
    strcat(stringYPos, " Y Position");
    
    coord setup = {
        // Subtract 1 from the given input to go from 1-indexed to 0-indexed.
        .x = promptForDigitsWithLimit(stringXPos, sizeX) - 1,
        .y = promptForDigitsWithLimit(stringYPos, sizeY) - 1
    };
    return setup;
}


int main() {
    
    printf("1: Startfeld wird vom Programm gewählt.\n");
    printf("2: Startfeld wird vom Anwender frei gewählt.\n");
    printf("3: Startfeld wird vom Anwender frei gewählt, der Springer geht einen geschlossenen Pfad.\n");
    
    int option = promptForDigitsWithLimit("Wählen Sie bitte zwischen den Optionen 1, 2 und 3 aus", 3);
    bool result = false;
    coord initial;
    
    setupBoardSize();
    resetBoardAndSteps();
    
    switch (option) {

        case 1: { // open tour with initial values pre-defined
            printf("Starting at Position (1,1).\n");
            coord initial = { .x = 0 , .y = 0 };
            result = startBackTracking(initial);
            break;
        }
        case 2: { // open tour
            initial = setupPosition("Initial");
            result = startBackTracking(initial);
            break;
        }
        case 3: { // closed tour
            initial = setupPosition("Initial");
            result = startBackTrackingClosed(initial);
            break;
        }
    }

    if(result) {
        printf("\nA solution has been found!\n");
        printf("\nSolution Steps:\n\n");
        printSteps();
    } else {
        printf("No solution could be found, please try other values!\n");
    }
    
    free(board);
    free(steps);
    
    flushStdIn();
    printf("Press enter to exit the program.");
    getchar();
    return 0;
}
