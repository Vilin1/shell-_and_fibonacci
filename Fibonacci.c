#include <stdio.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/stat.h>
#define MAX_SEQUENCE 10

typedef struct {
	int fib_sequence[MAX_SEQUENCE];
	int sequence_size;
} shared_data;

int main(int argc, char *argv[]) {
	//vilid argc 
	if(argc != 2) {
		printf("You have to run in (a.out + size) way.\n");
		return -1;
	}

	//get size
	int sequence_size = atoi(argv[1]);

	if(sequence_size > MAX_SEQUENCE) {
		printf("The quence you want to create out of range!\n");
		return -1;
	}

	//allocate a shared memory segment
	int segment_id = shmget(IPC_PRIVATE, sizeof(shared_data), S_IRUSR|S_IWUSR);
	if(segment_id == -1) {
		printf("Allocate a shared memory segment fail!");
		return -1;
	}	

	//attach
	shared_data* shared_memory = (shared_data*)shmat(segment_id, NULL, 0);

	if(shared_memory == (shared_data*) - 1) {
		printf("Attach error!\n");
		return -1;
	}
	shared_memory->sequence_size = sequence_size;
	//----------------------------------------------------------------------
	//write data from pid
	pid_t pid = fork();
	if(pid == 0) {
		printf("Write data to shared_memory in pid\n");
		if(sequence_size == 1) {
			shared_memory->fib_sequence[0] = 0;
		} else if(sequence_size == 2) {
			shared_memory->fib_sequence[0] = 0;
			shared_memory->fib_sequence[1] = 1;
		} else {
			shared_memory->fib_sequence[0] = 0;
			shared_memory->fib_sequence[1] = 1;
			for(int i = 2; i < sequence_size; i++) {
				shared_memory->fib_sequence[i] = shared_memory->fib_sequence[i-1] + shared_memory->fib_sequence[i-2];
			}
		}
		//detach the shared memory
		int detach = shmdt(shared_memory);
		if(detach == -1) {
			printf("Detach fail!\n");
			return -1;
		}
	} else {
		//wait until 
		wait(NULL);
		for(int i = 0; i < sequence_size; i++) {
			printf("%d\n", shared_memory->fib_sequence[i]);
		}
		//detach the shared memory
		int detach = shmdt(shared_memory);
		if(detach == -1) {
			printf("Detach fail!\n");
			return -1;
		}
		shmctl(segment_id, IPC_RMID, NULL);
	}

	return 0;
}