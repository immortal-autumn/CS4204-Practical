# Examination

## Question 1

#### Sub-question a

Amdahl's law evaluates how the speed of multi-core CPU is improved theoretically in the parallel computation. The Amdahl's law can be described as follows:
$$
S = \frac{1}{1-p+\frac{p}{n}}
$$
 The derivation of the axiom is shown as follows:

Speed up rate can be calculated as the original total execution time divides by current total execution time, where can be represents as $S=\frac{T}{T'}$

Suppose the parallelisable fraction can be represents as p, hence the non-parallelisable fraction can be represents as $(1-p)$. In this case, parallel time of execution completion on 𝑛 units costs:
$$
(1 - p) + \frac{p}{n}
$$
Therefore, the speed up rate is:
$$
S = \frac{1}{1-p+\frac{p}{n}}
$$
And the Amdahl's law is now derived.

#### Sub-question b

Since p is the speed of code execution, which is not changed whatever processor cores we would use. As a result, p is being calculated as follows:
$$
S_2 = \frac{1}{(1 - p) + \frac{p}{2}} = \frac{1}{1 - 0.5\times p}\\
S_2 \times (1 - 0.5p) = 1\\
S2 - 0.5p \times S2 = 1\\
0.5p \times S_2 = S_2 - 1\\
p = 2\times\frac{S_2 - 1}{S_2}
$$

$$
S_4 = \frac{1}{1-(2\times\frac{S_2 - 1}{S_2})+\frac{2\times\frac{S_2 - 1}{S_2}}{4}}\\
S_4 = \frac{1}{1-\frac{2S_2 - 2}{S_2} + \frac{S_2-1}{2S_2}}\\
S_4 = \frac{1}{\frac{2S_2 - 4S_2 + 4 + S_2 - 1}{2S_2}}\\
S_4 = \frac{2S_2}{3 - S_2}
$$

#### Sub-question c

###### Sequential Consistency Model

Observation: r = 1

Justify: 

1. Thread 0 executed x = 1 and write back x = 1 to the shared memory. (No matter which instruction executes first, thread 1 and thread 2 will keep looping until thread 0 is executed)
2. Thread 1 break the loop and thread 2 continue looping.
3. Thread 1 executes y = 1, and thread 2 continue looping
4. Thread 2 break the loop with y = 1.
5. Thread 2 executes r = x, where x is 1, so r = 1.

###### X86-TSO Model

Observation: r = 0 

Justify:

1. Thread 0 executes x=1, but found that x is missing in Thread 0 buffer, so it read value from shared memory and put it in its thread cache and transmit a "read invalid message".
2. Thread 1 executes while(x != 1) and found that x is missing in Thread 1 buffer, so it read value of x(which is 0 currently, as thread 0 is not write back the buffer to the shared memory) to thread 1 cache. since x = 0 != 1, so the thread 1 continue looping, and transmit a "Invalid read" message.
3. Thread 2 executes while(y != 1) and found that y is missing in Thread 2 buffer, so it read value of y from shared memory, which is 0. The assertion (y != 1) failed so thread 2 continue looping, and transmit a "Invalid read" message.
4. Thread 0 read the message and transmit the cheche line to thread 1 and remove the cache line in the buffer store. Thread 1 received cache line from thread 0, and pass the assertion. But the thread 2 is still looping. 
5. Thread 1 executes y = 1, and the thread 2 is still lopping and transmitting "Invalid read"
6. Thread 1 read message from thread 1 and transmit the cache line of y=1 to thread 2, thread 2 then read y=1 to the buffer store, and pass the assertion
7. Thread 2 executes r = x, and found that x is missing. (Considering the time cost on execution of program, r may read 1 from the memory as memory may flushed, however we assume that the memory is not flushed) r will read 0 from memory. Thread 2 will then transmit a “Invalid read" message.
8. If the value is not being flushed, thread 1 will read the "Invalid read" message and start to transmit the cache line x=1 to thread 2, and remove that cache line from its own cache.
9. However, 0 is already write to r, the cache line x=1 is comes too late and r is already being observed to be r=0.

#### Sub-question d

Observing: r=1

Translation of the ARM memory model can be represents as follows:

```
Thread 0:
dmb;
st x 1;

Thread 1:
loop: ld x rtmp
	cmp rtmp 1;
	beq loop;
dmb;
st y 1

Thread 2:
loop: ld y rtmp
	cmp temp 1;
	beq loop;
isb;
ld r x
```

Since instructions running before step 7 in X86-TSO Model are the same, the ARM memory model just add a instruction: $isb$ before setting x to r. With isb, read reordering are prevented, and the cache line x=1 successfully comes to thread 2 as the cached x=0 in memory is unable to set to r before cached line comes. As a result, r is set to 1.

#### Sub-question e

In sequential consistency model, values are being observed directly from shared memory, any modification is directly observed by observers. However, the condition is different in ARM architecture.

1. In the shared memory, x=0, with a inconsistency with thread 1 where x is set to 1. Observation on x is inconsistent. Thread 1 transmitting invalid read message and retrieve cached line from thread 0, without retrieving information from thread 0, which is inconsistency as well.
2. In the shared memory, y=0, with a inconsistency with thread 2 where y is set to 1 after the while loop. Thread 2 transmitting invalid read message and retrieve cached line from thread 0, without retrieving information from thread 1. Observation on y is forbidden as well.
3. The last instruction load the value of x to r, as the value of x in shared memory is 0, but the x in thread 2 is the passed cached line from thread 1. Which means that x here is unobserved by observer, which is forbidden. 

#### Sub-question f

```c
typedef struct Account {
	int value;
	pthread_mutex_t plock;
} Account;

Account *init_account() {
    Account *account = (Account*)malloc(sizeof(Account));
    account -> value = 0;
    // Lock initialize failed
    if (pthread_mutex_init(&(account->plock), NULL) != 0) {
        free(account);
    }
    return account;
}

int getvalue(Account a) {
    int value = 0;
    pthread_mutex_lock(&a.plock);
    value = a.value;
    pthread_mutex_unlock(&a.plock);
    return value;
}

int deposit(Account a, int d) {
    pthread_mutex_lock(&a.plock);
    a.value += d;
    pthread_mutex_unlock(&a.plock);
    return 0;
}

int withdraw(Account a, int w) {
    pthread_mutex_lock(&a.plock);
    if (a.value < w) {
        pthread_mutex_unlock(&a.plock);
        return -1;
    }
    a.value -= d;
    pthread_mutex_unlock(&a.plock);
    return 0;
}
```



#### Sub-question g

Linearisable shows the linearisability of the program, which aims at showing that methods are calling take effect instantaneously at some time between invocation and response. The implementation of methods ensures that the mutex locks the minor code block and will not cost extra time on unrelated code block.

getvalue() method locks only when value is being get from the data structure. The lock is instantaneously released when the value is assigned locally, which ensures the linearisability of the program.

deposit() method locks only when the value is being added to the data structure, and instantaneously released when the computation is finished, which ensures the linearisability of the program.

withdraw() method locks when value of the account is visited, if the account balance does not support the withdraw function, lock will instantaneously released; otherwise, decrease the value in the account data structure will be executed and the lock will be released. The minor lock ensures the linearisability of the program. 

#### Sub-question h

```c
typedef struct Account {
	int value;
} Account;

Account *init_account() {
    Account *account = (Account*)malloc(sizeof(Account));
    account -> value = 0;
    return account;
}

int getvalue(Account a) {
    return a.value;
}

int deposit(Account a, int d) {
    return compare_and_swap(&a.value, a.value, a.value + d);
}

int withdraw(Account a, int w) {
    int value = a.value;
    if (value >= w) {
        // Ensure that value is not incremented or decremented after the condition
        return compare_and_swap(&a.value, value, value-w);
    }
    return -1;
}
```

The implementation uses CAS technology, using atomic to ensure that no lock are to be added to the programme. Without data race, value can be read directly from the data structure and no write operation will be done in atomic code block. The only thing that may cause failure is the withdraw method, as the value may change after the value comparison. As a result, we make sure that the value is not changed with setting the predicted value as the value that compared with the withdraw value, only when the value is not modified, can the value being swapped.

## Question 2

#### Sub-question a

1. The pattern helps with program design
2. The pattern helps with guiding implementations.
3. The pattern helps with building wheels: programmers do not need to work directly with the parallel runtime system.

#### Sub-question b

###### OpenMP

Advantages:

1. Treats the parallel programming as a very high level process
2. Using relaxed consistency shared memory model.
3. Supports heterogeneous systems and using fork-join execution model.
4. User makes strategic decisions
5. User does not need to specify any parallelism details: The compiler tackle with parallelism and the result is automatically in a $pthread()$ execution.

4. Compilers transform the pragma into a parallel implementation of the execution.

Disadvantages:

1. Need a designed compiler.
2. Compilers supports OpenMp with the specific flag $-fopenmp$.
3. Specialised compilers and architectures with struggle.
4. Less generality: only C, C++ and Fortran supports OpenMp.
5. Similar models do already exists.

###### TBB

Advantages: 

1. Designed for developers with limited knowledge of multicore and parallelism
2. Third party libraries and syntaxes are not required.
3. Additional compilers are not required: relies on the feature of C++
4. Uses the built-in scheduler that use work stealing technique that allows for load balancing. 

Disadvantage: Limited in control and parallelism

#### Sub-question c

- The reduce pattern is a variant of a farm sums up items into a collection. Each independent input $x_i$, can be $x_1, x_2, ..., x_n$ is being  load into multiple shuffling patterns concat all item that the same together $(p :: a \rightarrow [b])$ that can work upon the parallel. (e.g. <bear, 1, 1>, <house, 1>, <bear, 1> $\to$ <<bear, 1, 1>, <bear, 1>>, <<house, 1>>)

- A worker function (c::[b] $\rightarrow$ [c])  concat the count of the items shows above together and get the desired output of the item (Operations: $<name, <1, 1, 1, 1, 1, 1, ....> \to <name, n>$)

- Finally, the result is combined $f:[c]\rightarrow d$ into a single result of all input. (Operations: $<<name, num>, <name, num> ...>$)

#### Sub-question d

Original cost:
$$
(50 + 5) * 100 =  5500ms
$$


Costing Pipelines:

```pseudocode
func(Ims) {
	result =  pipeline(geoRef, filter, Images);
	return result;
}
```

$$
T = (Max{(50, 5)} + 2) \times 100 = 5200ms\\
SpeedUpRate = 5500 / 5200 = 1.05769231
$$



Costing Maps:

```
func(Ims) {
	result = pipeline(geoRef, parMap(filter', partition, combine), Images);
	return result;
}
```

$$
T = (Max(5, 50 / 8) + 2) * 100 = 8.25 * 100 = 825 ms\\
SpeedUpRate = 5500 / 825 = 6.66666666\\
SpeedUpRateCmpNaive = 5200 / 825 = 6.303030\\
$$



Costing Farms:

```
func(Ims) {
	result = pipeline(farm(geoRef, 8), parMap(filter', partition, combine), Images);
	return result;
}
```

$$
T = (Max(5 / 8, 50 / 8) + 2) * 100 = 8.25 * 100 = 825 ms\\
SpeedUpRate = 5500 / 825 = 6.66666666
SpeedUpRateCmpMaps = 1
$$



As a result, costing map gives the best speed ups.

#### Sub-question e

```
threadIDx.x: Since for each pixel in the grid of blocks, a thread is assigned to the pixel. The threadIDx is the index of the thread, as the threadIDx.x is the index of the thread in the x axis.

blockIDx.x: The blockIDx represents the index of the block. BlockIDx.x represents as the ID of the block in the x axis.

blockDim.x: The blockDim represents the size of the block. BlockDim.x represents as the size of the block in the x axis.
```

#### Sub-question f

```
void histogram(float* A, int N, float* H, int B) {
	float bin_begin;
	float x, y;
	#pragma omp parallel num_threads(4) shared(A, H)
	#pragma omp for firstprivate(N,B) private(x, y, bin_begin) schedual(dynamic, 50)
    for (int i = 0; i < N; i++) {	// N is first initialized from the main thread
        for (int j = 0; j < B; j++) {	// B is first initialized from the main thread
			#pragma omp task firstprivate(B) 	// Make B be private and independent prevents B changes
			bin_begin = float(j) / B;	// bin_begin is set private
			x = A[i];	// Avoid possiable read or write on another program
			if (x <= bin_begin) { 
				y = H[j];
				H[j] = x + y;
			}
        }
    }
}
```



#### Sub-question g

```
int N, i, x, res, z;
int* b;
int* c;
int *a;

#pragma omp parallel num_threads(4) shared(a)
#pragma omp for private(x, res, z) firstprivate(N) schedual(static, 25)
for (i = 0; i < N - 1; i++) {
	#pragma omp task shared(b, c)
	x = b[i] + c[i];
	z = a[i+1];	
	res = x + z;
	a[i] = res;
}
```

#### Sub-question h

```
int dataSize, numIter;
int data[100];

void main() {
	dataSize = 100;
	numIter = 4;
	InitializeData();
	run_pipe();
}

void run_pipe() {
	parallel_execution_tbb par{};
	
	int i = 0;
	
	//data[i] = data[i] + data[i] + 1 + myid; id = [0, 1, 2, 3]
	pipeline(par,
        [&i, numberIter]() -> {
        	i++;
            if (i < numberIter) return i-1;	// Current index of i
            else return {};
		},	
		[&j, dataSize](int i) -> {
            int j = 0;
            iny mystate = 0;	// Inner declared: not gloablly shared
            pipeline(	// 4 stages worker function
                [&j, dataSize]() -> {
                    j++;
                    if (j < dataSize) return j-1;
                    else return {};
                },
                [mystate](int x) {
                	return mystate + data[x] + 1;
                },
                [&mystate](int x) {
                    mystat = x;
                    return data[x] + mystat;
                },
                [i](int x) {
                	data[i] = x;
                }
            );
		},
	);
}

void InitializeData() {
    int i;
    for (i = 0; i < dataSize; i++) {
    	data[i] = 0;
    }
}

```



*Note that the worker function is a 4-stages pipeline pattern. For a better efficiency, I make the worker iteratiation function into parallel, which adds a pipeline to the program. Replacing the higher-level pipeline into the for loop does makes the program to be a 4-stages pipeline







