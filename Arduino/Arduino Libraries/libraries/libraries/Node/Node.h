#ifndef NODE_H
#define NODE_H

#include "Arduino.h"

class Node
{
public:
    Node();
    Node(byte element);
    
    byte data;
    Node * next;
};

#endif