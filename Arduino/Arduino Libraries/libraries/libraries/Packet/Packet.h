#ifndef PACKET_H
#define PACKET_H

#include "Arduino.h"
#include "Node.h"

// IMPORTANT
// You must call clear() on a packet when you no longer need it to deallocate the memory.

class Packet
{
public:
    // override default constructor
    Packet();
    
    // override = operator to deallocate memory
    Packet & operator=(Packet & packet);
    
    ~Packet();
    
    // add a new element to the packet list
    void add(byte element);
    
    // get an element from the packet list
    byte get(int index);
    
    // set an element in the packet list
    void set(int index, byte element);
    
    // removes all nodes from the packet list
    void clear();
    
    // return the size of the list
    int length();
    
private:
    int size;
    Node *head;
    Node *tail;
    
};

#endif