/* MPI Program Template */

#include <stdio.h>
#include <string.h>
#include "mpi.h"
#include <fstream>
#include <bits/stdc++.h>
using namespace std;
typedef long long int ll;

#define pb push_back
#define mp make_pair
#define ff first
#define ss second
#define all(a) a.begin(), a.end()
#define sz(a) (int)(a.size())
#define endl "\n"

const int L = 1e6 + 5;
int a[L];

int partition(int* a, int start, int end)
{
    int ran = rand();
    ran = start + ran % (end - start + 1);
    swap(a[ran], a[end]);

    int pivot = a[end];
    int i = start - 1;

    for(int j = start; j < end; j++)
    {
        if(a[j] < pivot)
        {
            i++;
            swap(a[i], a[j]);
        }
    }
    swap(a[i+1], a[end]);

    return i + 1;
}

void quicksort(int* a, int start, int end)
{
    if(start < end)
    {
        int p_index = partition(a, start, end);
        quicksort(a, start, p_index - 1);
        quicksort(a, p_index + 1, end);
    }
}

int main( int argc, char **argv ) {

    // ifstream in(argv[1]);
    // streambuf *cinbuf = cin.rdbuf();
    // cin.rdbuf(in.rdbuf());

    // ofstream out(argv[2]);
    // streambuf *coutbuf = cout.rdbuf();
    // cout.rdbuf(out.rdbuf());

    int rank, numprocs;
    int root_rank = 0;

    /* start up MPI */
    MPI_Init( &argc, &argv );

    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &numprocs );

    /*synchronize all processes*/
    MPI_Barrier( MPI_COMM_WORLD );
    double tbeg = MPI_Wtime();

    /* write your code here */

    int n;

    int cur_size = 0;
    int* cur_seg;

    if(rank == root_rank)
    {
        fstream fin;
        fin.open(argv[1], ios::in);

        fin >> n;
        // scanf("%d", &n);

        for(int i = 0; i < n; i++)
        {
            fin >> a[i];
            // scanf("%d", &a[i]);
        }
    }

    MPI_Bcast(&n, 1, MPI_INT, root_rank, MPI_COMM_WORLD);

    if(rank == root_rank)
    {
        int each = n / numprocs;
        int rem = n % numprocs;

        int done = each + (rem > 0 ? 1 : 0);
        int first = done;

        for(int i = 1; i < numprocs; i++)
        {
            int cur = each + (i < rem ? 1 : 0);
            MPI_Send(a + done, cur, MPI_INT, i, 0, MPI_COMM_WORLD);
            done += cur;
        }

        assert(done == n);

        quicksort(a, 0, first - 1);

        vector <queue <pair <int, int>>> v(numprocs);
        priority_queue <pair <int, int>, vector <pair <int, int>>, greater <pair <int, int>>> pq;

        for(int i = 0; i < first; i++)
        {
            if(!i)
            {
                pq.push(mp(a[i], 0));
            }

            else
            {
                v[0].push(mp(a[i], 0));
            }

            // cout << a[i] << " ";
        }
        // cout << endl;

        cur_seg = (int*) malloc((each + 1) * sizeof(int));

        for(int i = 1; i < numprocs; i++)
        {
            cur_size = each + (i < rem ? 1 : 0);
            MPI_Recv(cur_seg, cur_size, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            for(int j = 0; j < cur_size; j++)
            {
                if(!j)
                {
                    pq.push(mp(cur_seg[j], i));
                }

                else
                {
                    v[i].push(mp(cur_seg[j], i));
                }

                // cout << cur_seg[j] << " ";
            }
            // cout << endl;
        }

        vector <int> ans;

        while(!pq.empty())
        {
            pair <int, int> p = pq.top();
            pq.pop();

            ans.pb(p.ff);
            // cout << p.ff << " ";

            if(!v[p.ss].empty())
            {
                pq.push(v[p.ss].front());
                v[p.ss].pop();
            }
        }
        // cout << endl;

        // assert(sz(ans) == n);

        // vector <int> sorted_ans = ans;
        // sort(all(sorted_ans));

        // assert(sorted_ans == ans);

        fstream fout;
        fout.open(argv[2], ios::out);

        for(int i = 0; i < n; i++)
        {
            fout << ans[i] << " ";
        }
        fout << endl;
    }

    else
    {
        cur_size = n / numprocs + (rank < (n % numprocs) ? 1 : 0);
        cur_seg = (int*) malloc(cur_size * sizeof(int));
        MPI_Recv(cur_seg, cur_size, MPI_INT, root_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        quicksort(cur_seg, 0, cur_size - 1);
        MPI_Send(cur_seg, cur_size, MPI_INT, root_rank, 0, MPI_COMM_WORLD);
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
