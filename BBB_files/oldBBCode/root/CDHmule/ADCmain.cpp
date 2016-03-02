#include <stdio.h>
#include <unistd.h>

using namespace std;
/*
int ejectNodes(){
    //return based on nodeEject variable - set high if pins connected to each node go low
    //return if timeout elapses
    printf("Ejecting Nodes");
    int nodeOne, nodeTw0 = 0;
    while(!(nodeOne && nodeTwo)){
        //update time based on clock() and check if timeout occured
        nodeOne = 1;  //change these to based on pins connected to nodes
        nodeTwo = 1;
    }
    timeout = 0; //change to timeout based on clock cycles
    if(timeout){
        return 0;
    }
    return 1;
}
*/
int main(){
    printf("ADC process: Hello World!\n");
    printf("ADC process: Finished with this process. Closing now \n");
/*
    //while loop until all nodes are ejected and time hasnt elapsed
      //how long timeout on ejection process?
      //should it be based on number of attempts?
    while(ejectedNodes < 8 && !timeout){
        //ADC variable read
          //while loop of read memory - waiting f or true
          //if exceeds timer break loop and read global star for deploy command
        while(!MemoryRead()){  //way to do passive waiting and not
        //ask ADC process for data?
        }
        //eject nodes
        if(NodeEject()){
            ejectedNodes += 2;
        }
    }
*/
return 0;
}
