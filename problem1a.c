#include <mpi.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define MASTER 0

int isPrime(int n) {
    if (n <= 1) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    for (int i = 3; i <= sqrt(n); i += 2) {
        if (n % i == 0) return 0;
    }
    return 1;
}

int main(int argc , char** argv){

    MPI_Init(&argc , &argv);
    int rank , size;
    int x , y , r;
    int cnt = 0,Pcnt = 0;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double startTime = MPI_Wtime();

    if(rank == MASTER){
        printf("enter x: \n");
        scanf("%d",&x);
        printf("enter y: \n");
        scanf("%d",&y);

        r = (y - x) / ( size - 1);

    }
    MPI_Bcast(&x, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&y, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&r, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int start,end;
    char buffer[100]="";

    if(rank != MASTER){
        start = x + ( rank - 1) *r;
        end = start + r - 1;

        //remaining numbers
        if(rank==size - 1){
            end = y;
        }
    }

    for (int i = start; i <= end; i++) {
        if(isPrime(i)) {
            Pcnt++;
            char prime[10];
            sprintf(prime, "%d, ", i);
            strcat(buffer, prime);
        }

    }
    if(rank != MASTER)
        printf("p%d: calculate partial count of prime numbers from %d to %d Count = %d (%s)\n", rank, start, end, Pcnt, buffer);

    MPI_Reduce(&Pcnt, &cnt, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    double endTime = MPI_Wtime();

    if (rank == MASTER) {
        char primes[10000] = "";
        char prime[10];
        for (int i = x; i <= y; i++) {
            if (isPrime(i)) {
                sprintf(prime, "%d", i);
                if (strlen(primes) > 0) strcat(primes, ", ");
                strcat(primes, prime);
            }
        }
        printf("After reduction, P0 will have Count =%d (%s)\n", cnt,primes);
        printf("Execution time: %f sec \n",endTime - startTime);
    }

    MPI_Finalize();
    return 0;
}