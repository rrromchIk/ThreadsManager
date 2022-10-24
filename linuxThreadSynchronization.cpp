
//-------------------------- libraries --------------------------------

#include <pthread.h>
#include <iostream>
#include <vector>
#include <string>
#include <math.h>
#include <unistd.h>
#include <sys/wait.h>
#include <csignal>
#include <sys/resource.h>
#include <sys/types.h>
#include <chrono>
#include <sys/syscall.h>
#include <time.h>

using namespace std;

//--------------------------- data -------------------------------------

pthread_mutex_t LOCK = PTHREAD_MUTEX_INITIALIZER;
pthread_spinlock_t SPIN_LOCK;

vector<double> array;                     //global array
double sumOfElements = 0;                 //global sum
std::chrono::_V2::system_clock::time_point begin_t;



typedef struct {                   //parameters to thread function
    int begin;
    int end;
} params;

params* parameters;
pthread_t* threads;
pthread_attr_t* attributes;

//------------------------- functions --------------------------------

void initialiseArray(int arraySize);
void printArray();
void calculateSumInCertainInterval(int begin, int end);
void creatingThreads(int numOfThreads);
void joiningThreads(int numOfThreads);
void* threadTask(void* args);
void* provideFunctionality(void* args);

//--------------------------- main ----------------------------------

int main() {

//--------------------------- input data ---------------------------

    int arraySize;
    int numOfThreads;

    cout << "Enter size of array: ";
    cin >> arraySize;
    cout << "Enter amount of threads: ";
    cin >> numOfThreads;
    
//-------------------------- initialise array ---------------------

    initialiseArray(arraySize);
    //printArray();
  
//------------------ structs for creating threads -----------------
    
    parameters = new params[numOfThreads];
    threads = new pthread_t[numOfThreads];
    attributes = new pthread_attr_t[numOfThreads];
    
//--------------------- creating threads --------------------------

    creatingThreads(numOfThreads);
    pthread_t* threadsControl = new pthread_t;
    pthread_create(threadsControl, NULL, provideFunctionality, &numOfThreads);
    joiningThreads(numOfThreads);
    pthread_join(*threadsControl, NULL);
   
    
//--------------------- printing SUM ------------------------------

    cout << "Sum: " << sumOfElements << endl;
    auto time = chrono::high_resolution_clock::now() - begin_t;
    cout << "Duration: " << chrono::duration<double, milli>(time).count() << "ms\n";
    
    delete[] threads;
    delete[] parameters;
    delete[] attributes;
    delete threadsControl;
   
//---------------------------- END --------------------------------

    getchar();
    return 0;
}


//------------------------ functions realisation ----------------

void creatingThreads(int numOfThreads) {
    int gap;
    pthread_spin_init(&SPIN_LOCK, 0);
    begin_t = chrono::high_resolution_clock::now();
    for(int i = 0; i < numOfThreads; ++i) {
    	
    	//------------ calculate intervals -------------
    	gap = array.size() / numOfThreads;
    	parameters[i].begin = gap * i;
    	if(i == numOfThreads - 1) {
    	    gap = array.size() - gap * (numOfThreads - 1);
    	}
    	parameters[i].end = parameters[i].begin + gap;
    	//-----------------------------------------------
    	
    	pthread_attr_init(&attributes[i]);
    	pthread_create(&threads[i], &attributes[i], threadTask, &parameters[i]);
    }
    for(int i = 0; i < numOfThreads; ++i) {
    	cout << i << " - thread" << endl;
    }
    cout << "PID: " << getpid() << endl;
}

void* threadTask(void* args) {
    params* parameter = (params*)args;
    //cout << "thread is working" << endl;
    calculateSumInCertainInterval(parameter->begin, parameter->end);
    return 0;
}

void calculateSumInCertainInterval(int begin, int end) {
    for(int i = begin; i < end; ++i) {
    
        //----------------- mutex ------------------
        //pthread_mutex_lock(&LOCK);
        //sumOfElements += array[i];
        //pthread_mutex_unlock(&LOCK);
        
        //-------------- spin lock ----------------
        
        pthread_spin_lock(&SPIN_LOCK);
        sumOfElements += array[i];
        pthread_spin_unlock(&SPIN_LOCK);
        
        //sleep(0.1);
        // TO DO
    }
    sleep(10);
}

void* provideFunctionality(void* args) {
    int numOfThreads = *((int*)args);
    while(true) {
        int choice;
        cout << "\nChoose option: \nDetach thread - 1\nSet thread affinity - 2\nCancel thread - 3\nChange priority - 4\nExit - 5\n";
        cout << "\nEnter choice: ";
        int ID;
        cin >> choice;
        switch(choice) {
            case 1: {
                cout << "Enter num of thread to detach: ";
            	std::cin >> ID;
           	int detached = pthread_detach(threads[ID]);
            	if(detached == 0)
            	    cout << "Thread with " << ID << " index was detached" << endl;
            	else
            	    cout << "Error in detached thread with " << ID << " index, code:" << detached << endl;
            	break;
            }
            case 2: {
            	int cpuIndex;
            	cout << "Enter num of thread: ";
 		cin >> ID;
     	    	cout << "Enter count CPU do you want to use for set affinity: ";
     	    	cin >> cpuIndex;
     	    	cpu_set_t cpuSet;
     	    	CPU_ZERO(&cpuSet);
     	    	CPU_SET(cpuIndex, &cpuSet);
     	    	pthread_setaffinity_np(threads[ID], sizeof(cpuSet), &cpuSet);
     	    	break;
            }
            case 3: {
		cout << "Enter num of thread to cancel: ";
            	cin >> ID;
            	pthread_cancel(threads[ID]);
                break;
            }
            case 4: {
            	cout << "Enter num of thread: ";
            	cin >> ID;
            	int priority;
            	cout << "Enter priority: ";
            	cin >> priority;
            	
            	struct sched_param prioParams;
            	int pol = 0;
            	pthread_getschedparam(threads[ID], &pol, &prioParams);
            	
            	cout << "Old priority is: " << prioParams.sched_priority << endl;
            	
            	//pthread_setschedprio(threads[ID], priority);
            	prioParams.sched_priority=priority;
            	int policy = 2;
            	pthread_setschedparam(threads[ID], policy, &prioParams);
            	
            	pthread_getschedparam(threads[ID], &pol, &prioParams);
            	cout << "New priority is: " << prioParams.sched_priority << endl;
                break;
            }
            case 5: {
            	return 0;
            }
        }
    }
}

void printArray() {
    cout << endl << "Array: ";
    for(auto i : array) {
        cout << i << " ";
    }
    cout << endl;
}

void initialiseArray(int arraySize) {
    //array.push_back(2);
    for(int i = 0; i < arraySize; ++i) {
    	//array.push_back(array[i - 1] * i + exp(i));
    	array.push_back(2);
    }
}

void joiningThreads(int numOfThreads) {
    for(int i = 0; i < numOfThreads; ++i) {
        pthread_join(threads[i], NULL);
    }
}

























