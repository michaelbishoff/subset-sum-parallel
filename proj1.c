#include <stdio.h>
#include <omp.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define NUM_TARGETS 30

// offset is set in getNumColumns()
// numCols is set when we return from getNumColumns()
// numRows is set in readSet()
int offset, numCols, numRows;

int getNumColumns(int* I);
void clearQMatrix(int** Q);
void printPretty(int* I, int** Q);
int findSolution(int* I, int** Q, int target);
void reconstructSolution(int* I, int** Q, int target);

int* readSet(char *filename);
int* readTargets(char *filename);

/**
 * Given a set of values, times how long it takes to find if a specified target
 * value can be made by summing a subset of the given set of values.
 * 
 * Command line args:
 * argv[1] - a file of numbers that are in our set of values. The first line of
 *           the file is the number of values in the file.
 * argv[2] - a file of 30 numbers that are the targets that we try to reach
 *           with a subset of the values in argv[1]
 * 
 * Compile: gcc proj1.c -fopenmp -o proj1
 * Run: ./proj1 set_I_filename targets_filename
 *
 * Authors: Gaurav Luthria <gluthr1@umbc.edu>
 *          Rajan Patel <rajanp1@umbc.edu>
 *          Michael Bishoff <bishoff1@umbc.edu>
 */
int main(int argc, char* argv[]) {
  // Check if command line args are valid
  if (argc != 3) {
    printf("ERROR: Command line arguments are <set_I_filename> ");
    printf("<targets_filename>\n");
    return 1;
  }
  
  // Initializes I, the set of values, and the list of targets that we use
  // to see if a subset of I sums to the value of a target
  int* I = readSet(argv[1]);
  int* targets = readTargets(argv[2]);
  
  // Determines the number of columns needed in the Q table
  numCols = getNumColumns(I);

  // Allocates space for the Q table
  int** Q = malloc(sizeof(*Q)*numRows);
  for (int i = 0; i < numRows; i++) {
    Q[i] = malloc(sizeof(*(Q[i]))*numCols);
  }
  
  double totalTime = 0;

  for (int i = 0; i < NUM_TARGETS; i++) {
    clearQMatrix(Q);
    
    double startTime = omp_get_wtime();
    //clock_t startTime = clock();
    int solutionExists = findSolution(I, Q, targets[i]);
    double endTime = omp_get_wtime();
    //clock_t endTime = clock();
    
    totalTime += (double) (endTime - startTime);
    
    // Uncomment this to see the reconstructed solution!
    /*
    printf("%d: ", targets[i]);
    // If there's a solution, print it
    if (solutionExists) {
      // Uncomment this to see the Q table!
      // printPretty(I, Q);
      reconstructSolution(I, Q, targets[i]);
      
    } else {
      printf("NO SOUTION EXISTS! ");
    }
    */
  }

  printf("Average Time: %f\n", (totalTime / NUM_TARGETS));
  printf("Total Time: %f\n", totalTime);
  
  
  // Freeeee
  free(I);

  for (int i = 0; i < numRows; i++) {
    free(Q[i]);
  }
  free(Q);
  
  return 0;
}


/**
 * Gets the number of columns in the Q table.
 * Sets the global variable "offset" which is the offset from index 0 to the
 * number at index 0.
 */
int getNumColumns(int* I) {
  // Finds the highest and lowest possible values that can be reached
  int maxSum = 0, minSum = 0;
  // Used to check if the number zero is in the array
  int hasZero = 0;
  
  // Calculates the maximum and minimum possible values as well as sets a flag
  // that helps us determine if 0 is the smallest number in the set of values
  for (int i = 0; i < numRows; i++) {
    if (I[i] < 0) {
      minSum += I[i];

    } else if (I[i] > 0) {
      maxSum += I[i];

    } else {
      hasZero = 1;
    }
  }

  // Handles the special case when there are only positive number
  if (minSum == 0) {
    // I[] includes zero, then the columns starts at number 0 & index 0
    if (hasZero) {
      offset = 0;

    // I[] doesn't include zero
    } else {
      // Offset is the smallest possible sum
      // So it's the first number because all the numbers are positive
      offset = I[0];
    }

    // Return the max sum and include zero in the max sum if it's there
    return maxSum + hasZero;

    // If maxSum has a value, then the range spans zero,
    // because we know minSum has a value
  } else if (maxSum != 0) {
    hasZero = 1;
  }

  // Note: There is no "special case" for only negative numbers because
  // the offset will start at the smallest possible sum, which is always
  // all of the negative numbers in the case of all negative and the
  // case with neg. & pos.

  // Values start at a negative number
  offset = minSum;

  // The negative number range + positive num range +
  return (-1 * minSum) + maxSum + hasZero;
}


/**
 * Zeros out the Q Matrix
 */
void clearQMatrix(int** Q) {
  // row is shared by the outer loop. col is shared by the inner loop.
  // but each outer loop has it's own col
  // variables definied in the parallel statement aren't shared
  int row = 0;

  #pragma omp parallel for
  for (row = 0; row < numRows; row++) {
    int col = 0;

    #pragma omp parallel for
    for (col = 0; col < numCols; col++) {
      Q[row][col] = 0;
    }
  }
}


/**
 * Prints the Q table like a table with the columns and rows labeled
 */
void printPretty(int* I, int** Q) {
  printf("    ");
  // Prints out the column header
  int maxNum = numCols + offset; // Converts 18 to 13 in the example
  for (int colHeader = offset; colHeader < maxNum; colHeader++) {
    printf("%4d", colHeader);
  }
  printf("\n");

  // Print the row headers and the Q table
  for (int row = 0; row < numRows; row++) {
    printf("%4d", I[row]);

    for (int col = 0; col < numCols; col++) {
      printf("%4d", Q[row][col]);
    }
    printf("\n");
  }
  printf("\n");
}


/**
 * Returns true if a subset of I sums to the target value
 * Returns false otherwise
 */
int findSolution(int* I, int** Q, int target) {

  // If the target is less than the lowest possible value or higher
  // than the largests possible value, return false
  if (target < offset || target >= numCols + offset) {
    return 0;
  }

  // Initializes the frist row's value
  Q[0][I[0] - offset] = 1;

  // If the first element of I is the target
  if (I[0] == target) {
    return 1;
  }

  int targetFound = 0;

  // Loop through I[]
  for (int prevRow = 0, row = 1; row < numRows; row++, prevRow++) {
    // If the number we are looking for is in I
    if (I[row] == target) {
      // Set the value at the current row and the
      // target's column index to 1
      Q[row][target - offset] = 1;
      return 1;
    }

    // Sets this row's values
    // Each thread shares col, so define it before the statement.
    int col = 0;
    #pragma omp parallel for
    for (col = 0; col < numCols; col++) {

      // If there's a 1 in the row above the current row
      if (Q[prevRow][col]) {
        // Copy the 1 to this row because that value is
        // also valid at this row
        Q[row][col] = 1;

        // Add the element of I to the value at this column
        // to make a new valid sum
        int newValidSumIndex = col + I[row];
        Q[row][newValidSumIndex] = 1;

        // If we just placed a 1 in the column that corresponds to
        // the number that we are looking for, return true
        if (newValidSumIndex + offset == target) {
          targetFound = 1;
        }
      }
    }
    
    // Returns true here rather than in the if statment because the if
    // statement is parallelized
    if (targetFound) {
      return 1;
    }

    // Set's this I[i]'s value as valid in the Q table
    Q[row][I[row] - offset] = 1;
  }

  return 0;
}


/**
 * Given a Q table, the set of values I, and a target, prints out the numbers
 * in I used to reach the target value.
 * Precondition: There is guarenteed solution and a constructed Q table.
 */
void reconstructSolution(int* I, int** Q, int target) {
  int targetCol = target - offset;
  int row = numRows;

  // Skip the rows that weren't used in the solution
  // numRows is out of bounds, so decrement
  // Each iteration also needs to decrement, so we're good! :)
  while (Q[--row][targetCol] == 0) { }

  // if the target was in I[]
  if (target == I[row]) {
    printf("%d ", I[row]);

    // the target was constructed from I[]
  } else {
    // Keep track of the sum to avoid conditions where the target is zero
    int sum = 0;

    // do-while because if the target is 0, then sum == target
    // and also because we know there is at least one value in the subset
    do {

      // Skip the rows that weren't used in the solution
      while (row - 1 >= 0 && Q[row - 1][targetCol] == 1) { row--; }

      printf("%d ", I[row]);

      // Moves the target column to be a part of the sum that made the target
      targetCol -= I[row];
      sum += I[row];
      row--;
      
    // while we're not out of bounds and the row has a 1 in it
    } while (sum != target && row >= 0 && targetCol >= 0 && Q[row][targetCol]);

  }
  printf("\n");
}


/* File IO Functions */

/**
 * Reads the set of numbers I from a file. The first line of the file is the
 * number of values in the file.
 * Sets the value of the global variable numRows
 */
int* readSet(char *filename) {
  FILE* fp = fopen(filename, "r");
  
  // Reads the number of rows in the file
  int* numRowsPtr = malloc(sizeof(int));
  fscanf(fp, "%d", numRowsPtr);
  
  // Stores the value read in as a global int
  numRows = *numRowsPtr;
  free(numRowsPtr);

  // Creates the I[] of values in our set
  int* I = (int*) malloc(sizeof(int) * numRows);
  for (int i = 0; i < numRows; i++) {
    fscanf(fp, "%d", (I+i));
  }

  return I;
}

/**
 * Reads the list of target values used to see if a subset of I sums
 * to the target values.
 * There is Always 30 target values in each file
 */
int* readTargets(char *filename) {
  FILE* fp = fopen(filename, "r");

  int* targets = (int*) malloc(sizeof(int) * NUM_TARGETS);

  for (int i = 0; i < NUM_TARGETS; i++) {
    fscanf(fp, "%d", (targets+i));
  }

  return targets;
}
