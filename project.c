// Include the libraries required
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

/**
 * First Command ---> mpicc project.c -o project.exe
 * 
 * Second Command ---> mpirun -np 4 ./project.exe
 *      
 *                      where "-np" flag is for number of processes to use.
 *                      you can remove it and run it as well.
*/

// Initialize the Program Start main()
int main(int argc, char **argv)
{
    int rank, size;           // Variables to Store the rank and size

    MPI_Init(&argc, &argv);                       // Initialize the MPI instance 
    MPI_Comm_size(MPI_COMM_WORLD, &size);         // Store the Total Processes Size in "size" variable
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);         // Store the Current Process in "rank" variable

    int arrSize = 0;
    int portion = 0;

    if (size < 2)
    {
        printf("Number of Processes should be > = 2\n");         // Show this Prompt to the User
        
        MPI_Finalize();           // Terminate the MPI enviorment
        
        return 0;
    }
    else
    {
        arrSize = (size-1) * 3;             // Initialzing the Size of Array acoording to number of total process, to save memory
        portion = arrSize / (size-1);       // calculating the portion of the array to send to next process
    }

    int *temp = malloc(portion * sizeof(int));    // Dynamically allocating the "temp" array

    int perProcMaxVal = 0;             // Variable to store Max-Value of each processes

    printf("____Hello Process --> %d <--____\n", rank);       // Prompt to see which process is active currently

    // "Master Process" sends the portions
    if (rank == 0)
    {
        int *array = malloc(arrSize * sizeof(int));     // Dynamically allocating the array

        for (int i = 0; i < arrSize; i++)
        {
            array[i] = rand() % 10;              // Populating the Array with Random Values
        }

        for (int i = 1; i < size; i++)
        {
            MPI_Send(array + ((i - 1) * portion), portion, MPI_INT, i, i, MPI_COMM_WORLD);  // Sending Portion to other Processes
            printf("Done Sending Array Portion to ----> Process (%d)\n", i); // Prompt that portion is send to the dest process.
        }
    }
    else
    {
        printf("Fetching Data ..... from Process 0\n");         // Prompt that Child Process is activ
        
        MPI_Recv(temp, portion, MPI_INT, 0, rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);  // Receive Array via Address, Blocking Call
        
        perProcMaxVal = temp[0];   // Initializing the per Process Max-Value variable with arrays[0]
        
        for (int i = 0; i < portion; i++)
        {
            printf("Process %d -> Values : %d\n", rank, temp[i]);     // Printing the Data in Array received
            if (temp[i] > perProcMaxVal)
            {
                perProcMaxVal = temp[i];           // Updating the Max-Value variable
            }
        }
        printf("Process (%d) has Max Value = %d\n", rank, perProcMaxVal);   // Print the Max-Value of the Portion
    }
    // "Master Process" wait for Max-Value of each portion
    if (rank == 0)
    {
        int tempValue = 0;
        int MaxValue = -99999;

        for (int i = 1; i < size; i++)
        {
            // Receive the Max-Value from each process
            MPI_Recv(&tempValue, 1, MPI_INT, i, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            if (tempValue > MaxValue)
            {
                MaxValue = tempValue;         // Update the Maximum value as soon as the value is received from other process
            }
        }
        printf("The (Max Value of the Array) ===== %d\n\n",MaxValue);   // Finally print the Largest of all Value
    }
    else
    {
        // If Child Process then send its perProcMaxValue to the Master Process
        MPI_Send(&perProcMaxVal, 1, MPI_INT, 0, rank, MPI_COMM_WORLD);
    }

    MPI_Finalize();           // Terminate the MPI envionment
    
    return 0;
}