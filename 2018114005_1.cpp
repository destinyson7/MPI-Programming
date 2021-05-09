/* MPI Program Template */

#include <stdio.h>
#include <string.h>
#include "mpi.h"
#include <fstream>
#include <bits/stdc++.h>
using namespace std;
typedef long long int ll;

int main( int argc, char **argv ) {

    // freopen(argv[1], "r", stdin);
    // freopen(argv[2], "w", stdout);

    int rank, numprocs;
    int root_rank = 0;
    double estimated_value = 0;

    /* start up MPI */
    MPI_Init( &argc, &argv );

    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &numprocs );

    /*synchronize all processes*/
    MPI_Barrier( MPI_COMM_WORLD );
    double tbeg = MPI_Wtime();

    int n;
    if(rank == root_rank)
    {
        fstream fin;
        fin.open(argv[1], ios::in);

        fin >> n;
    }

    MPI_Bcast(&n, 1, MPI_INT, root_rank, MPI_COMM_WORLD);

    int each = (n + numprocs - 1)/numprocs;
    double answer = 0;

    for(int i = rank * each + 1; i <= min(n, (rank + 1) * each); i++)
    {
        answer += (double)1.0/(double)(i * i);
    }

    // printf("rank: %d, answer: %lf\n", rank, answer);

    MPI_Reduce(&answer, &estimated_value, 1, MPI_DOUBLE, MPI_SUM, root_rank, MPI_COMM_WORLD);

    if(rank == root_rank)
    {
        fstream fout;
        fout.open(argv[2], ios::out);

        fout << fixed << setprecision(6) << estimated_value << endl;
    }

    MPI_Barrier( MPI_COMM_WORLD );
    double elapsedTime = MPI_Wtime() - tbeg;
    double maxTime;
    MPI_Reduce( &elapsedTime, &maxTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD );
    if ( rank == 0 ) {
        printf( "Total time (s): %f\n", maxTime );
    }

    /* shut down MPI */
    MPI_Finalize();
    return 0;
}
