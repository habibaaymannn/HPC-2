#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <mpi.h>


void PrintMatrix(char arr[], int rows, int cols){
	int cnt = 0;
	for (int i = 0; i < rows; i++) {
        	for (int j = 0; j < cols; j++) {
            		printf("%d ", arr[cnt]);
			cnt++;
        	}
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
	
	const int N=3, M=3;
	const int length = N*M;
	int chunk = length / (size);
	const int reminder = length % (size);
	if(reminder){chunk++;}

	char StringA[N*M],StringB[N*M], StringC[N*M];

    char local_A[chunk], local_B[chunk], local_C[chunk];

    if(rank == 0) {
		char MatrixA[N][M],MatrixB[N][M];
		int cnt = 0;
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < M; j++) {
					MatrixA[i][j] = cnt;
					MatrixB[i][j] = cnt;
			cnt++;
			}
		}

		cnt = 0;
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < M; j++) {
				StringA[cnt] = MatrixA[i][j];
				StringB[cnt] = MatrixB[i][j];
				cnt++;
			}
		}
		printf("Matrix A:\n");
        PrintMatrix(StringA, N, M);
        printf("\nMatrix B:\n");
        PrintMatrix(StringB, N, M);
    }

	MPI_Scatter(StringA, chunk, MPI_CHAR, local_A, chunk, MPI_CHAR, 0, MPI_COMM_WORLD);
	MPI_Scatter(StringB, chunk, MPI_CHAR, local_B, chunk, MPI_CHAR, 0, MPI_COMM_WORLD);

	for (int i = 0; i < chunk; i++) {
			local_C[i] = local_A[i] + local_B[i];
	}
	
	printf("Processor %d has data: ", rank);
	for (int i = 0; i < chunk; i++) {
		printf("%d ", local_C[i]);
	}
	printf("\n");

	MPI_Gather(local_C, chunk, MPI_CHAR, StringC, chunk, MPI_CHAR, 0, MPI_COMM_WORLD);
		
	if(rank==0){
		printf("\nResult Matrix C (A + B):\n");
        PrintMatrix(StringC, N, M);
	}
	
    MPI_Finalize();
}