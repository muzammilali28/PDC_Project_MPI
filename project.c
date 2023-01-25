// Include the libraries required
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

// Initialzie the Program Start main()
int main(int argc, char **argv)
{
    int rank, size;           // Variables to Store the rank and size

    MPI_Init(&argc, &argv);                       // Initialize the MPI instance 
    MPI_Comm_size(MPI_COMM_WORLD, &size);         // Store the Total Processes Size in "size" variable
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);         // Store the Current Process in "rank" variable

    int arrSize = 0;
    int portion = 0;

    if (size % 4 != 0 )
    {
        MPI_Finalize();
        return 0;
    }
    else
    {
        arrSize = size * 3;             // Initialzing the Size of Array acoording to number of total process, to save memory
        portion = arrSize / size;       // calculating the portion of the array to send to next process
    }

    int *temp = malloc(portion * sizeof(int));

    int perProcMaxVal = 0;

    printf("____Hello Process --> %d <--____\n", rank);

    if (rank == 0)
    {
        int *array = malloc(arrSize * sizeof(int));

        for (int i = 0; i < arrSize; i++)
        {
            array[i] = rand() % 10;
        }

        for (int i = 1; i < size; i++)
        {
            MPI_Send(array + ((i - 1) * portion), portion, MPI_INT, i, i, MPI_COMM_WORLD);
            printf("Done Sending Array Portion to ----> Process (%d)\n", i);
        }
    }
    else
    {
        printf("Fetching Data ..... from Process 0\n");

        MPI_Recv(temp, portion, MPI_INT, 0, rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        perProcMaxVal = temp[0];

        for (int i = 0; i < portion; i++)
        {
            printf("Process %d -> Values : %d\n", rank, temp[i]);
            // local_sum += temp[i];
            if (temp[i] > perProcMaxVal)
            {
                perProcMaxVal = temp[i];
            }
        }
        printf("Process (%d) has Max Value = %d\n", rank, perProcMaxVal);
    }

    if (rank == 0)
    {
        int tempValue = 0;
        int MaxValue = -99999;

        for (int i = 1; i < size; i++)
        {
            MPI_Recv(&tempValue, 1, MPI_INT, i, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            if (tempValue > MaxValue)
            {
                MaxValue = tempValue;
            }
        }
        printf("The (Max Value of the Array) ===== %d\n\n",MaxValue);
    }
    else
    {
        MPI_Send(&perProcMaxVal, 1, MPI_INT, 0, rank, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    
    return 0;
}