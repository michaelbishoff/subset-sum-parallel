#include <stdio.h>
#include <omp.h>
#include <time.h>

/* Assume we are reading in file with a single integer on each line and the
   integers are in sorted order
*/

int offset;
int numCols;

int getNumColumns();
void clearQMatrix(int Q[][numCols], int numRows);
void printQ(int Q[][numCols], int numRows);
void printPretty(int I[], int Q[][numCols], int numRows);
int findSolution(int I[], int Q[][numCols], int numRows, int target);
void reconstructSolution(int I[], int Q[][numCols], int numRows, int target);

int main() {
  int I[] = {-5, 0, 1, 4, 7};
  int numRows = 5;
  // Determines the number of columns needed in the Q table
  numCols = getNumColumns(I, numRows);

  int Q[numRows][numCols];
  clearQMatrix(Q, numRows);

  // The target value
  int target = 0;

  // double startTime = omp_get_wtime();
  clock_t startTime = clock();
  int solutionExists = findSolution(I, Q, numRows, target);
  // double endTime = omp_get_wtime();
  clock_t endTime = clock();

  // If there's a solution, print it
  if (solutionExists) {
    printPretty(I, Q, numRows);
    reconstructSolution(I, Q, numRows, target);

  } else {
    printf("NO SOUTION EXISTS!\n");
  }

  printf("Time to run: %f\n", (endTime - startTime));
  
  return 0;
}


int getNumColumns(int I[], int size) {
  // Finds the highest and lowest possible values that can be reached
  int maxSum = 0, minSum = 0;
  // Used to check if the number zero is in the array
  int hasZero = 0;

  for (int i = 0; i < size; i++) {
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


void clearQMatrix(int Q[][numCols], int numRows) {

  for (int row = 0; row < numRows; row++) {
    for (int col = 0; col < numCols; col++) {
      Q[row][col] = 0;
    }
  }
}

void printQ(int Q[][numCols], int numRows) {
  for (int row = 0; row < numRows; row++) {
    for (int col = 0; col < numCols; col++) {
      printf("Q[%d][%d] = %d\n", row, col, Q[row][col]);
    }
  }
}

void printPretty(int I[], int Q[][numCols], int numRows) {
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

int findSolution(int I[], int Q[][numCols], int numRows, int target) {

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
    #pragma omp parallel for
    for (int col = 0; col < numCols; col++) {

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
          // return 1;
        }
      }
    }

    if (targetFound) {
      return 1;
    }

    // Set's this I[i]'s value as valid in the Q table
    Q[row][I[row] - offset] = 1;
  }

  return 0;
}

/**
 * Precondition: There is guarenteed solution and a constructed Q table.
 */
void reconstructSolution(int I[], int Q[][numCols], int numRows, int target) {
  int targetCol = target - offset;

  // Skip the rows that weren't used in the solution
  // numRows is out of bounds, so decrement
  // Each iteration also needs to decrement, so we're good! :)
  while (Q[--numRows][targetCol] == 0) { }

  // if the target was in I[]
  if (target == I[numRows]) {
    printf("%d ", I[numRows]);

    // the target was constructed from I[]
  } else {
    // while we're not out of bounds and the row has a 1 in it
    while (numRows >= 0 && targetCol >= 0 && Q[numRows][targetCol]) {
      printf("%d ", I[numRows]);

      // Moves the target column to be a part of the sum that made the target
      targetCol -= I[numRows];
      numRows--;
    }
  }
  printf("\n");
}






