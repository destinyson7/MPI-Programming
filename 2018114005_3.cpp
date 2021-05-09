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
#define sz(a) (ll)(a.size())
#define endl "\n"

template <class Ch, class Tr, class Container>
basic_ostream <Ch, Tr> & operator << (basic_ostream <Ch, Tr> & os, Container const& x)
{
    os << "{ ";
    for(auto& y : x)
    {
        os << y << " ";
    }
    return os << "}";
}

template <class X, class Y>
ostream & operator << (ostream & os, pair <X, Y> const& p)
{
    return os << "[" << p.ff << ", " << p.ss << "]";
}

const int L = 5e2 + 5;
int adj[L][L];
int value[L];
int color[L];

// const int M = 1e2 + 5;
// int adj_edge[M][M];

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

    int n = 0, m = 0;
    if(rank == root_rank)
    {
        fstream fin;
        fin.open(argv[1], ios::in);

        int original_n, original_m;
        fin >> original_n >> original_m;

        vector <pair <int, int>> edge;
        edge.pb(mp(-1, -1));

        for(int i = 0; i < original_m; i++)
        {
            int u, v;
            fin >> u >> v;

            // adj_edge[u][v] = 1;
            // adj_edge[v][u] = 1;

            edge.pb(mp(u, v));
        }

        n = original_m;
        m = 0;

        for(int i = 1; i <= original_m; i++)
        {
            for(int j = i + 1; j <= original_m; j++)
            {
                if(edge[i].ff == edge[j].ff || edge[i].ff == edge[j].ss || edge[i].ss == edge[j].ff || edge[i].ss == edge[j].ss)
                {
                    m++;
                    adj[i][j]++;
                    adj[j][i]++;
                }
            }
        }

        for(int i = 1; i <= n; i++)
        {
            value[i] = i;
        }
        random_shuffle(value + 1, value + n + 1);

        // for(int i = 1; i <= n; i++)
        // {
        //     cout << value[i] << endl;
        // }
    }

    MPI_Bcast(&n, 1, MPI_INT, root_rank, MPI_COMM_WORLD);
    MPI_Bcast(adj, L * L, MPI_INT, root_rank, MPI_COMM_WORLD);
    MPI_Bcast(value, n + 1, MPI_INT, root_rank, MPI_COMM_WORLD);

    // cout << "***" << endl;

    int each = n / numprocs;
    int rem = n % numprocs;

    int start = 1 + rank * each + min(rem, rank);
    int end = start + each + (rank < rem ? 1 : 0) - 1;

    // cout << "each, rem: " << each << " " << rem << endl;
    // cout << "rank: " << rank << " " << start << " " << end << endl;

    int iter_cnt = 0;

    while(true)
    {
        iter_cnt++;

        vector <pair <int, int>> ind_set;
        int this_iter = 0;

        for(int i = start; i <= end; i++)
        {
            if(!color[i])
            {
                bool greatest = true;
                // set <int> s;

                for(int j = 1; j <= n; j++)
                {
                    // s.insert(j);

                    // if(iter_cnt == 1)
                    // {
                    //     cout << rank << " " << i << " " << j << " " << value[i] << " " << value[j] << " " << color[i] << " " << color[j] << " " << adj[i][j] << endl;
                    // }

                    if(!color[j] && adj[i][j] && value[i] < value[j])
                    {
                        // if(iter_cnt == 1)
                        // {
                        //     cout << "break condition " << rank << " " << i << endl;
                        // }

                        greatest = false;
                        break;
                    }
                }

                if(greatest)
                {
                    vector <bool> taken(n + 1, false);
                    // cout << "rank: " << rank << " " << i << " at iteration: " << iter_cnt << endl;

                    for(int j = 1; j <= n; j++)
                    {
                        if(color[j] && adj[i][j])
                        {
                            // s.erase(color[j]);
                            taken[color[j]] = true;
                        }
                    }

                    int cur_color = 1;

                    for(int j = 1; j <= n; j++)
                    {
                        if(!taken[j])
                        {
                            cur_color = j;
                            break;
                        }
                    }

                    ind_set.pb(mp(i, cur_color));
                    color[i] = cur_color;
                    this_iter++;
                }

                // s.clear();
            }
        }

        int toColor[this_iter][2];
        // int toColor_copy[L][2];

        for(int i = 0; i < this_iter; i++)
        {
            toColor[i][0] = ind_set[i].ff;
            toColor[i][1] = ind_set[i].ss;

            // toColor_copy[i][0] = toColor[i][0];
            // toColor_copy[i][1] = toColor[i][1];
        }

        // if(iter_cnt <= 2)
        // {
        //    cout << ind_set << endl;
        // }

        ind_set.clear();

        int this_iter_copy = this_iter;

        for(int i = 0; i < numprocs; i++)
        {
            MPI_Bcast(&this_iter_copy, 1, MPI_INT, i, MPI_COMM_WORLD);

            if(i == rank)
            {
                MPI_Bcast(toColor, this_iter_copy * 2, MPI_INT, i, MPI_COMM_WORLD);
            }

            else
            {
                int toColor_copy[this_iter_copy][2];
                MPI_Bcast(toColor_copy, this_iter_copy * 2, MPI_INT, i, MPI_COMM_WORLD);

                for(int j = 0; j < this_iter_copy; j++)
                {
                    color[toColor_copy[j][0]] = toColor_copy[j][1];
                }
            }

            this_iter_copy = this_iter;

            // for(int i = 0; i < L; i++)
            // {
            //     toColor_copy[i][0] = toColor[i][0];
            //     toColor_copy[i][1] = toColor[i][1];
            // }
        }

        bool finish = false;

        int already_colored = 0;

        for(int i = 1; i <= n; i++)
        {
            if(color[i])
            {
                already_colored++;
            }

            else
            {
                break;
            }
        }

        // cout << "already colored: " << already_colored << endl;

        if(already_colored == n)
        {
            finish = true;
        }

        if(finish)
        {
            break;
        }
    }

    if(rank == root_rank)
    {
        int n_colors = 0;

        for(int i = 1; i <= n; i++)
        {
            n_colors = max(n_colors, color[i]);
        }

        fstream fout;
        fout.open(argv[2], ios::out);

        fout << n_colors << endl;
        for(int i = 1; i <= n; i++)
        {
            fout << color[i] << " ";
        }
        fout << endl;
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
