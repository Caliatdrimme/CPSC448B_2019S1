#include <mpi.h>
#include <stdio.h>

int rank, size, data;
int prime = 2;
int fst = 0;

//every process when receives 0 sends 0 along and returns
//every process when receives 1 sends along and continues

void generator(){

//sends number each time starting with 2 ++ to node 1
//receives from node size-1
//if what it receives is 0, sends 0 to node 1 and quits

	while(1){
	        MPI_Send(&prime, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
		//printf("rank %d sent %d\n", rank, prime);
        	MPI_Recv(&data, 1, MPI_INT, size-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		//printf("rank %d received %d\n", rank, data);
		if (data == 0){
			MPI_Send(&data, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
			//printf("rank %d sent %d\n", rank, data);
			return;}
		prime++;
	}
}

void multiple(){
	while(1){

// receive a number to check
//if flag to stop - send the 0 along to rank+1 and return
//if flag to empty continue, send the 1 along to rank+1
// if first one store it in prime
//if not first one check against the prime
//if not multiple send to next one
//else go onto next loop

		MPI_Recv(&data, 1, MPI_INT, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		//printf("rank %d received %d\n", rank, data);

		if (data == 0) {
			MPI_Send(&data, 1, MPI_INT, rank+1, 0, MPI_COMM_WORLD);
			//printf("rank %d sent %d\n", rank, data);
			return;}

		if (data == 1){
			MPI_Send(&data, 1, MPI_INT, rank+1, 0, MPI_COMM_WORLD);
			//printf("rank %d sent %d\n", rank, data);
			continue;}

		if ((data%prime == 0)&&(rank != 1)){
			//not a prime so we send empty 1 forward
			int mult =1;
			MPI_Send(&mult, 1, MPI_INT, rank+1, 0, MPI_COMM_WORLD);
			//printf("rank %d sent %d\n", rank, mult);

		} else if (fst == 0){
			/* this is our prime */
			prime = data;
			printf("PRIME FOUND %d\n\n", prime);
			/* flag so we don't overwite our prime */
			fst = 1;
			MPI_Send(&fst, 1, MPI_INT, rank+1, 0, MPI_COMM_WORLD);
			//printf("rank %d sent %d\n", rank, fst);
		} else { 
			MPI_Send(&data, 1, MPI_INT, rank+1, 0, MPI_COMM_WORLD);	
			//printf("rank %d sent %d\n", rank, data);	
		} //else 
	} // while true
} //void multiple

void last_multiple(){
	while(1){

//if receives 1 send 1 to generator 0 to continue
//else sends 0 to generator 0 to stop, prints the last prime and quits

		MPI_Recv(&data, 1, MPI_INT, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		//printf("rank %d received %d\n", rank, data);

		if (data == 1){
			MPI_Send(&data, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
			//printf("rank %d sent %d\n", rank, data);
			continue;
		} else {
			MPI_Send(&fst, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
			printf("last prime %d\n", data);
			return;}

	}//while true

}//void last



int main(int argc, char ** argv)
{
	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	//printf("I am %d of %d\n", rank, size);

	if (rank == 0){generator();}
	else if (rank == size-1){last_multiple();}
	else {multiple();}
	//printf("rank %d done\n", rank);
	MPI_Finalize();
	return 0;
}

