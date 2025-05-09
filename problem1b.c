#include <mpi.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

int is_prime(int n) {
    if (n <= 1) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    for (int i = 3; i <= sqrt(n); i += 2) {
        if (n % i == 0) return 0;
    }
    return 1;
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Status status;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    double startTime = MPI_Wtime();

    int x, y;
    if (rank == 0) {
        printf("enter x: \n");
        scanf("%d", &x);
        printf("enter y: \n");
        scanf("%d", &y);
        int total = y - x+1;
        int subrange = total / (size - 1);
        //int remainder = total % (size - 1);

        for (int i = 1; i < size; i++) {
            MPI_Send(&x, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&subrange, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&y, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    }

    if (rank != 0) {
        int start, subrange, end;
        MPI_Recv(&start, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&subrange, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&end, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

        int total = end - start+1;
        int remainder = total % (size - 1);
        int id = rank - 1;
        
        int a = start + id * subrange + (id < remainder ? id : remainder);
        int b = a + subrange - 1;

        if (id < remainder) {
            b += 1;
        }

        size_t range = (size_t)(b - a + 1);
        int* subArray = (int*)malloc(range * sizeof(int));


        int subCount = 0;
        for (int i = a; i <= b; i++) {
            if (is_prime(i)) {
                subArray[subCount++] = i;
            }
        }



        printf("P%d: calculate partial count of prime numbers from %d to %d     Count = %d (", rank, a, b, subCount);

        //printf("Primes from process %d: ", rank);
        for (int i = 0; i < subCount; i++) {
            printf("%d ", subArray[i]);
        }
        printf(") \n");


        MPI_Send(&subCount, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
        MPI_Send(subArray, subCount, MPI_INT, 0, 2, MPI_COMM_WORLD);

        free(subArray);
    }

    if (rank == 0) {
        int total = y - x;

        int* allPrimes = (int*)malloc(total * sizeof(int));

        int pos = 0;
        int total_count = 0;

        for (int i = 1; i < size; i++) {
            int subcount = 0;

            MPI_Recv(&subcount, 1, MPI_INT, i, 1, MPI_COMM_WORLD, &status);

            int* subarray = (int*)malloc(subcount * sizeof(int));

            MPI_Recv(subarray, subcount, MPI_INT, i, 2, MPI_COMM_WORLD, &status);


            for (int j = 0; j < subcount; j++) {
                allPrimes[pos++] = subarray[j];
            }

            total_count += subcount;
            free(subarray);
        }


        printf("After reduction, P0 will have Count = %d (", total_count);

        for (int i = 0; i < total_count; i++) {
            printf("%d ", allPrimes[i]);
        }
        printf(")\n");

        free(allPrimes);
    double endTime = MPI_Wtime();
    printf("Execution time: %f sec \n", endTime - startTime);

    }

    MPI_Finalize();
   

    return 0;
}