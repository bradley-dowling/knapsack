#include <stdio.h>
#include "structures.h"

struct maxValueReturn maxValue(int, struct item *, struct maxValueReturn *);

// *********************************
// ** Bradley Dowling - CSE215 - PA5
// ** 3/3/2021
// **
// ** This is a program that ingests an item list (containing weights and
// ** values for different items)  from a database (argv[2]), takes a user 
// ** specified bag capacity (maximum weight), and prints out the maximum
// ** value that can be attained from a combination of items in the item list
// ** where the total weight of the items is less than the user specified capacity.
// ** This program is a solution to "The Knapsack Problem".
// ** 

int main(int argc, char **argv)
{
  // check if user entered correct number of args:
  if (argc != 3) 
  {
    printf("ERROR: Expected 2 arguments, got %d\n", argc - 1);
    return 1; // exit, user didn't enter correct number of args
  }  

  int capacity;
  
  // check if argv[1] is an int
  if (1 != sscanf(argv[1], "%d", &capacity))
  {
    printf("ERROR: First argument needs to be a valid integer less than or equal to 1024\n");
    return 1;
  }
  
  // check if argv[1] is less than or equal to 1024
  if (capacity > 1024)
  {
    printf("ERROR: First argument needs to be a valid integer less than or equal to 1024\n");
    return 1;
  }

  // Reading the database file
  char buffer[120]; // temp storage for file reading
  FILE *fp;
  fp = fopen(argv[2], "r");
  if (fp == NULL)
  {
    printf("ERROR: unable to to open file <%s>\n", argv[2]);
    return 1;
  }

  struct item itemList[128]; // list of item structures
  int itemsIndex = 0; // index

  // copy database values into items array
  while (NULL != fgets(buffer, 120, fp))
  {
    if (itemsIndex > 128) break; // maximum item count reached
    sscanf(buffer, "%s %d %d", itemList[itemsIndex].name, &itemList[itemsIndex].value, &itemList[itemsIndex].weight);
    itemsIndex++;
  }

  // table header:
  printf("Weight |Value   |Item            \n");
  printf("---------------------------------\n");
  
  int i; // iterator
  for (i = 0; i < itemsIndex; i++)
  {
    printf("%-7d %-8d %s\n", itemList[i].weight, itemList[i].value, itemList[i].name);
  }

  printf("---------------------------------\n");

  // Get max value: 
  struct maxValueReturn maxValueReturns[1025]; // list of max value return structures (data chache)
  struct maxValueReturn finalInventory = maxValue(capacity, itemList, maxValueReturns); // maxValueReturn structure
                                                                                        // for users inputted capacity
  
  // Start outputing max value information and the respective inventory that gives that max value
  printf("Bag's capacity = %d\n", capacity);
  printf("Highest possible value = %d\n", finalInventory.value);
  
  // loop through the item list to print out the item inventory
  for (i = 0; i < 128; i++)
  {
    if (itemList[i].weight == 0) break; // stop because we've reached the end of the item list
    if (finalInventory.inventory[i] == 0) continue; // don't bother printing this item out because it isn't in final inventory
    printf("Item %d (%s): %d\n", i, itemList[i].name, finalInventory.inventory[i]);
  }

  // end!
  return 0;
}

// Function name: maxValue
//
// Purpose: to calculate the maximum value and inventory necessary to acheive the max value
//          with a given knapsack capacity. Function is used recursively on occasions where maxValue
//          hasn't been called before with the given capacity
//
// Inputs:
//   int capacity: maximum weight of knapsack
//   struct item *itemList: the list of all items (comprised of struct items)
//   struct maxValueReturn *maxValueReturns: a list of all maxValueReturn structures representing all 
//                                           possible knapsack capacities
// 
// Return:
//   struct maxValueReturn: the maxValueReturn structure containing the maximum value and inventory 
//                          needed to attain that value based on capacity given as input
//
// Side Effects: maxValue sets the maxValueReturn structure in the maxValueReturns list initialized in main.
//               This is done in order to avoid having to continually call maxValue multiple times to
//               to calculate each maximum value based on given capacity.
//
struct maxValueReturn maxValue(int capacity, struct item *itemList, struct maxValueReturn *maxValueReturns)
{
  // First, check to see if capacity is 0
  if (capacity == 0) {
    return maxValueReturns[0];
  }

  int maximumValue = 0; // this will ultimately be the maximum value possible for the specifed capacity
  int maxValueItemIndex = -1; // index of the item added to attain this maximum value
                              // set to a non-indexable value to start with in case a maximumValue isn't found
                              // for this iteration of maxVal
  int possibleMax = 0; // a possible max value that we'll compare to the actual max value during our loop
  int subCapacity; // the capacity that results from the current capacity minus the item we are looking
                   // to add to the final inventory
  int i; // iterator variable for for loop below

  // Now loop through each item to see what item added will give us the max value
  for (i = 0; i < 128; i++)
  {
    subCapacity = capacity - itemList[i].weight;
    if (itemList[i].weight == 0) break; // if we encounter an item of weight 0, we've reached the end
                                        // of the itemList so break out of the loop
    if (subCapacity < 0) continue; // do not run maxVal with a negative number, move to next item in item list
    if (maxValueReturns[subCapacity].value == 0)
    {
      // maxValue for this new capacity hasn't been calculated yet
      // set the return value for the subCapcity to be another maxValue call so we 
      // we don't need to calculate this in the future
      maxValueReturns[subCapacity] = maxValue(subCapacity, itemList, maxValueReturns);
    }

    // set possibleMax
    possibleMax = itemList[i].value + maxValueReturns[subCapacity].value;

    if (possibleMax > maximumValue)
    {
      maximumValue = possibleMax; // we've found a max value
      maxValueReturns[capacity] = maxValueReturns[subCapacity]; // preliminary step before adding new item
      maxValueItemIndex = i; // store the item (as an index in itemList) that gave us this new max value
    }
  }

  maxValueReturns[capacity].value = maximumValue; // set the value for this capacity in the cache
                                                  // so we don't have to keep calling maxValue(capacity,...)
  if (maxValueItemIndex != -1) maxValueReturns[capacity].inventory[maxValueItemIndex] += 1; // increase the item amount of the item
                                                                                            // that gave us the max value in this 
                                                                                            // returns' inventory
  
  return maxValueReturns[capacity]; // return the max capacity
}
