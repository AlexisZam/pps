#include "mpi.h"
#include "utils.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#if defined(JACOBI)
void Jacobi(double **u_previous, double **u_current, int X_min, int X_max, int Y_min, int Y_max) {
    int i, j;
    for (i = X_min; i < X_max; i++)
        for (j = Y_min; j < Y_max; j++)
            u_current[i][j] = (u_previous[i - 1][j] + u_previous[i + 1][j] + u_previous[i][j - 1] + u_previous[i][j + 1]) / 4.0;
}
#elif defined(GAUSS_SEIDEL_SOR)
void GaussSeidel(double **u_previous, double **u_current, int X_min, int X_max, int Y_min, int Y_max, double omega) {
    int i, j;
    for (i = X_min; i < X_max; i++)
        for (j = Y_min; j < Y_max; j++)
            u_current[i][j] = u_previous[i][j] + (u_current[i - 1][j] + u_previous[i + 1][j] + u_current[i][j - 1] + u_previous[i][j + 1] - 4 * u_previous[i][j]) * omega / 4.0;
}
#elif defined(RED_BLACK_SOR)
void RedSOR(double **u_previous, double **u_current, int X_min, int X_max, int Y_min, int Y_max, double omega, int correction) {
    int i, j;
    for (i = X_min; i < X_max; i++)
        for (j = Y_min; j < Y_max; j++)
            if ((i + j + correction) % 2 == 0)
                u_current[i][j] = u_previous[i][j] + (omega / 4.0) * (u_previous[i - 1][j] + u_previous[i + 1][j] + u_previous[i][j - 1] + u_previous[i][j + 1] - 4 * u_previous[i][j]);
}

void BlackSOR(double **u_previous, double **u_current, int X_min, int X_max, int Y_min, int Y_max, double omega, int correction) {
    int i, j;
    for (i = X_min; i < X_max; i++)
        for (j = Y_min; j < Y_max; j++)
            if ((i + j + correction) % 2 == 1)
                u_current[i][j] = u_previous[i][j] + (omega / 4.0) * (u_current[i - 1][j] + u_current[i + 1][j] + u_current[i][j - 1] + u_current[i][j + 1] - 4 * u_previous[i][j]);
}
#endif

int main(int argc, char **argv) {
    int rank, size;
    int global[2], local[2]; //global matrix dimensions and local matrix dimensions (2D-domain, 2D-subdomain)
    int global_padded[2];    //padded global matrix dimensions (if padding is not needed, global_padded=global)
    int grid[2];             //processor grid dimensions
    int i, j, t;
#ifdef TEST_CONV
    int global_converged = 0, converged = 0; //flags for convergence, global and per process
#endif
    MPI_Datatype dummy; //dummy datatype used to align user-defined datatypes in memory
#if defined(GAUSS_SEIDEL_SOR) || defined(RED_BLACK_SOR)
    double omega; //relaxation factor - useless for Jacobi
#endif

    struct timeval tts, ttf, tcs, tcf; //Timers: total-> tts,ttf, computation -> tcs,tcf, convergence -> tcvs, tcvf
#ifdef TEST_CONV
    struct timeval tcvs, tcvf;
#endif
    double ttotal = 0, tcomp = 0, total_time, comp_time;
#ifdef TEST_CONV
    double tconv = 0, conv_time;
#endif

    double **U = NULL, **u_current, **u_previous, **swap; //Global matrix, local current and previous matrices, pointer to swap between current and previous

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    //----Read 2D-domain dimensions and process grid dimensions from stdin----//

    if (argc != 5) {
        fprintf(stderr, "Usage: mpirun .... ./exec X Y Px Py");
        exit(-1);
    } else {
        global[0] = atoi(argv[1]);
        global[1] = atoi(argv[2]);
        grid[0] = atoi(argv[3]);
        grid[1] = atoi(argv[4]);
    }

    //----Create 2D-cartesian communicator----//
    //----Usage of the cartesian communicator is optional----//

    MPI_Comm CART_COMM;      //CART_COMM: the new 2D-cartesian communicator
    int periods[2] = {0, 0}; //periods={0,0}: the 2D-grid is non-periodic
    int rank_grid[2];        //rank_grid: the position of each process on the new communicator

    MPI_Cart_create(MPI_COMM_WORLD, 2, grid, periods, 0, &CART_COMM); //communicator creation
    MPI_Cart_coords(CART_COMM, rank, 2, rank_grid);                   //rank mapping on the new communicator

    //----Compute local 2D-subdomain dimensions----//
    //----Test if the 2D-domain can be equally distributed to all processes----//
    //----If not, pad 2D-domain----//

    for (i = 0; i < 2; i++) {
        if (global[i] % grid[i] == 0) {
            local[i] = global[i] / grid[i];
            global_padded[i] = global[i];
        } else {
            local[i] = (global[i] / grid[i]) + 1;
            global_padded[i] = local[i] * grid[i];
        }
    }

#if defined(GAUSS_SEIDEL_SOR) || defined(RED_BLACK_SOR)
    //Initialization of omega
    omega = 2.0 / (1 + sin(3.14 / global[0]));
#endif

    //----Allocate global 2D-domain and initialize boundary values----//
    //----Rank 0 holds the global 2D-domain----//
    if (rank == 0) {
        U = allocate2d(global_padded[0], global_padded[1]);
        init2d(U, global[0], global[1]);
    }

    //----Allocate local 2D-subdomains u_current, u_previous----//
    //----Add a row/column on each size for ghost cells----//

    u_previous = allocate2d(local[0] + 2, local[1] + 2);
    u_current = allocate2d(local[0] + 2, local[1] + 2);

    //----Distribute global 2D-domain from rank 0 to all processes----//

    //----Appropriate datatypes are defined here----//

    //----Datatype definition for the 2D-subdomain on the global matrix----//

    MPI_Datatype global_block;
    MPI_Type_vector(local[0], local[1], global_padded[1], MPI_DOUBLE, &dummy);
    MPI_Type_create_resized(dummy, 0, sizeof(double), &global_block);
    MPI_Type_commit(&global_block);

    //----Datatype definition for the 2D-subdomain on the local matrix----//

    MPI_Datatype local_block;
    MPI_Type_vector(local[0], local[1], local[1] + 2, MPI_DOUBLE, &dummy);
    MPI_Type_create_resized(dummy, 0, sizeof(double), &local_block);
    MPI_Type_commit(&local_block);

    //----Rank 0 defines positions and counts of local blocks (2D-subdomains) on global matrix----//
    int *scatteroffset = NULL, *scattercounts = NULL;
    if (rank == 0) {
        scatteroffset = (int *)malloc(size * sizeof(int));
        scattercounts = (int *)malloc(size * sizeof(int));
        for (i = 0; i < grid[0]; i++)
            for (j = 0; j < grid[1]; j++) {
                scattercounts[i * grid[1] + j] = 1;
                scatteroffset[i * grid[1] + j] = (local[0] * local[1] * grid[1] * i + local[1] * j);
            }
    }

    //----Rank 0 scatters the global matrix----//

    MPI_Scatterv(rank == 0 ? U[0] : NULL, scattercounts, scatteroffset, global_block, &u_current[1][1], 1, local_block, 0, CART_COMM);
    memcpy(u_previous[0], u_current[0], (local[0] + 2) * (local[1] + 2) * sizeof(double));

    //************************************//

    if (rank == 0)
        free2d(U);

    //----Define datatypes or allocate buffers for message passing----//

    MPI_Datatype local_row;
    MPI_Type_contiguous(local[1], MPI_DOUBLE, &dummy);
    MPI_Type_create_resized(dummy, 0, sizeof(double), &local_row);
    MPI_Type_commit(&local_row);

    MPI_Datatype local_col;
    MPI_Type_vector(local[0], 1, local[1] + 2, MPI_DOUBLE, &dummy);
    MPI_Type_create_resized(dummy, 0, sizeof(double), &local_col);
    MPI_Type_commit(&local_col);

    //************************************//

    //----Find the 4 neighbors with which a process exchanges messages----//

    int north, south, east, west;

    MPI_Cart_shift(CART_COMM, 0, 1, &north, &south);
    MPI_Cart_shift(CART_COMM, 1, 1, &west, &east);

    //************************************//

    //---Define the iteration ranges per process-----//

    int min[2], max[2];

    for (i = 0; i < 2; i++) {
        min[i] = rank_grid[i] == 0 ? 2 : 1;
        max[i] = local[i] + 1;
        if (grid[i] - rank_grid[i] < (global_padded[i] - global[i]) / local[i] + 1)
            max[i] = 1;
        else if (grid[i] - rank_grid[i] == (global_padded[i] - global[i]) / local[i] + 1)
            max[i] = local[i] - (global_padded[i] - global[i]) % local[i];
    }

    //************************************//

    //----Computational core----//
    gettimeofday(&tts, NULL);
#ifdef TEST_CONV
    for (t = 0; t < T && !global_converged; t++) {
#else
#undef T
#define T 256
    for (t = 0; t < T; t++) {
#endif

#if defined(JACOBI)
        swap = u_previous;
        u_previous = u_current;
        u_current = swap;

        gettimeofday(&tcs, NULL);

        Jacobi(u_previous, u_current, min[0], max[0], min[1], max[1]);

        gettimeofday(&tcf, NULL);
        tcomp += (tcf.tv_sec - tcs.tv_sec) + (tcf.tv_usec - tcs.tv_usec) * 0.000001;

        MPI_Request array_of_requests[8];
        i = 0;
        if (north != MPI_PROC_NULL) {
#define DUMMY_TAG 0
            MPI_Isend(u_current[1] + 1, 1, local_row, north, DUMMY_TAG, CART_COMM, &array_of_requests[i++]);
            MPI_Irecv(u_current[0] + 1, 1, local_row, north, MPI_ANY_TAG, CART_COMM, &array_of_requests[i++]);
        }
        if (south != MPI_PROC_NULL) {
            MPI_Isend(u_current[local[0]] + 1, 1, local_row, south, DUMMY_TAG, CART_COMM, &array_of_requests[i++]);
            MPI_Irecv(u_current[local[0] + 1] + 1, 1, local_row, south, MPI_ANY_TAG, CART_COMM, &array_of_requests[i++]);
        }
        if (east != MPI_PROC_NULL) {
            MPI_Isend(&u_current[1][local[1]], 1, local_col, east, DUMMY_TAG, CART_COMM, &array_of_requests[i++]);
            MPI_Irecv(&u_current[1][local[1] + 1], 1, local_col, east, MPI_ANY_TAG, CART_COMM, &array_of_requests[i++]);
        }
        if (west != MPI_PROC_NULL) {
            MPI_Isend(&u_current[1][1], 1, local_col, west, DUMMY_TAG, CART_COMM, &array_of_requests[i++]);
            MPI_Irecv(u_current[1], 1, local_col, west, MPI_ANY_TAG, CART_COMM, &array_of_requests[i++]);
        }
        MPI_Waitall(i, array_of_requests, MPI_STATUSES_IGNORE);
#elif defined(GAUSS_SEIDEL_SOR)
        swap = u_previous;
        u_previous = u_current;
        u_current = swap;

        MPI_Request array_of_requests[6];
        i = 0;
        if (north != MPI_PROC_NULL)
            MPI_Irecv(u_current[0] + 1, 1, local_row, north, MPI_ANY_TAG, CART_COMM, &array_of_requests[i++]);
        if (west != MPI_PROC_NULL)
            MPI_Irecv(u_current[1], 1, local_col, west, MPI_ANY_TAG, CART_COMM, &array_of_requests[i++]);
        MPI_Waitall(i, array_of_requests, MPI_STATUSES_IGNORE);

        gettimeofday(&tcs, NULL);

        GaussSeidel(u_previous, u_current, min[0], max[0], min[1], max[1], omega);

        gettimeofday(&tcf, NULL);
        tcomp += (tcf.tv_sec - tcs.tv_sec) + (tcf.tv_usec - tcs.tv_usec) * 0.000001;

        i = 0;
        if (north != MPI_PROC_NULL)
#define DUMMY_TAG 0
            MPI_Isend(u_current[1] + 1, 1, local_row, north, DUMMY_TAG, CART_COMM, &array_of_requests[i++]);
        if (south != MPI_PROC_NULL) {
            MPI_Isend(u_current[local[0]] + 1, 1, local_row, south, DUMMY_TAG, CART_COMM, &array_of_requests[i++]);
            MPI_Irecv(u_current[local[0] + 1] + 1, 1, local_row, south, MPI_ANY_TAG, CART_COMM, &array_of_requests[i++]);
        }
        if (east != MPI_PROC_NULL) {
            MPI_Isend(&u_current[1][local[1]], 1, local_col, east, DUMMY_TAG, CART_COMM, &array_of_requests[i++]);
            MPI_Irecv(&u_current[1][local[1] + 1], 1, local_col, east, MPI_ANY_TAG, CART_COMM, &array_of_requests[i++]);
        }
        if (west != MPI_PROC_NULL)
            MPI_Isend(&u_current[1][1], 1, local_col, west, DUMMY_TAG, CART_COMM, &array_of_requests[i++]);
        MPI_Waitall(i, array_of_requests, MPI_STATUSES_IGNORE);
#elif defined(RED_BLACK_SOR)
    swap = u_previous;
    u_previous = u_current;
    u_current = swap;

    for (j = 0; j < 2; j++) {
        gettimeofday(&tcs, NULL);

        int correction = ((local[0] & rank_grid[0]) ^ (local[1] & rank_grid[1])) & 1;
        if (j == 0)
            RedSOR(u_previous, u_current, min[0], max[0], min[1], max[1], omega, correction);
        else
            BlackSOR(u_previous, u_current, min[0], max[0], min[1], max[1], omega, correction);

        gettimeofday(&tcf, NULL);
        tcomp += (tcf.tv_sec - tcs.tv_sec) + (tcf.tv_usec - tcs.tv_usec) * 0.000001;

        MPI_Request array_of_requests[8];
        i = 0;
        if (north != MPI_PROC_NULL) {
#define DUMMY_TAG 0
            MPI_Isend(u_current[1] + 1, 1, local_row, north, DUMMY_TAG, CART_COMM, &array_of_requests[i++]);
            MPI_Irecv(u_current[0] + 1, 1, local_row, north, MPI_ANY_TAG, CART_COMM, &array_of_requests[i++]);
        }
        if (south != MPI_PROC_NULL) {
            MPI_Isend(u_current[local[0]] + 1, 1, local_row, south, DUMMY_TAG, CART_COMM, &array_of_requests[i++]);
            MPI_Irecv(u_current[local[0] + 1] + 1, 1, local_row, south, MPI_ANY_TAG, CART_COMM, &array_of_requests[i++]);
        }
        if (east != MPI_PROC_NULL) {
            MPI_Isend(&u_current[1][local[1]], 1, local_col, east, DUMMY_TAG, CART_COMM, &array_of_requests[i++]);
            MPI_Irecv(&u_current[1][local[1] + 1], 1, local_col, east, MPI_ANY_TAG, CART_COMM, &array_of_requests[i++]);
        }
        if (west != MPI_PROC_NULL) {
            MPI_Isend(&u_current[1][1], 1, local_col, west, DUMMY_TAG, CART_COMM, &array_of_requests[i++]);
            MPI_Irecv(u_current[1], 1, local_col, west, MPI_ANY_TAG, CART_COMM, &array_of_requests[i++]);
        }
        MPI_Waitall(i, array_of_requests, MPI_STATUSES_IGNORE);
    }
#endif

#ifdef TEST_CONV
        if (t % C == 0) {
            gettimeofday(&tcvs, NULL);
            converged = converge(u_previous, u_current, local[0] + 2, local[1] + 2);
            gettimeofday(&tcvf, NULL);
            MPI_Allreduce(&converged, &global_converged, 1, MPI_INT, MPI_LAND, MPI_COMM_WORLD);
            tconv += (tcvf.tv_sec - tcvs.tv_sec) + (tcvf.tv_usec - tcvs.tv_usec) * 0.000001;
        }
#endif

        //************************************//
    }
    gettimeofday(&ttf, NULL);

    ttotal = (ttf.tv_sec - tts.tv_sec) + (ttf.tv_usec - tts.tv_usec) * 0.000001;

    MPI_Reduce(&ttotal, &total_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&tcomp, &comp_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
#ifdef TEST_CONV
    MPI_Reduce(&tconv, &conv_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
#endif

    //----Rank 0 gathers local matrices back to the global matrix----//

    if (rank == 0) {
        U = allocate2d(global_padded[0], global_padded[1]);
    }

    MPI_Gatherv(&u_current[1][1], 1, local_block, rank == 0 ? U[0] : NULL, scattercounts, scatteroffset, global_block, 0, CART_COMM);

    //************************************//

    //----Printing results----//

    if (rank == 0) {
#if defined(JACOBI)
        char exe[] = "Jacobi";
#elif defined(GAUSS_SEIDEL_SOR)
        char exe[] = "GaussSeidelSOR";
#elif defined(RED_BLACK_SOR)
    char exe[] = "RedBlackSOR";
#endif
        printf("%s X %d Y %d Px %d Py %d Iter %d ComputationTime %lf TotalTime %lf ", exe, global[0], global[1], grid[0], grid[1], t, comp_time, total_time);
#ifdef TEST_CONV
        printf("ConvergenceTime %lf ", conv_time);
#endif
        printf("midpoint %lf\n", U[global[0] / 2][global[1] / 2]);

#ifdef PRINT_RESULTS
        char *s = malloc(50 * sizeof(char));
        sprintf(s, "res%sMPI_%dx%d_%dx%d", exe, global[0], global[1], grid[0], grid[1]);
        fprint2d(s, U, global[0], global[1]);
        free(s);
#endif
    }
    MPI_Finalize();
    return 0;
}
