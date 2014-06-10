#include "Arduino.h"
#include "Node.h"

Node::Node()
{
    next = NULL;
}

Node::Node(byte element)
{
    next = NULL;
    data = element;
}