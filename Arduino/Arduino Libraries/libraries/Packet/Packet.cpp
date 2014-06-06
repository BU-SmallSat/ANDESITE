#include "Arduino.h"
#include "Packet.h"

Packet::Packet()
{
    size = 0;
    head->next = NULL;
    tail->next = NULL;
}

// override = operator to deallocate memory
Packet & Packet::operator=(Packet & newPacket)
{
    Serial.println("in = operator overriding function");
	clear();
    for(int x = 0; x < newPacket.length(); x++)
    {
        add(newPacket.get(x));
    }
}

Packet::~Packet()
{
    //Serial.println("Packet destructor called");
}

void Packet::add(byte element)
{
    Node * n = new Node(element);
    if(head == NULL)
    {
        head = n;
        tail = head;
    }
    else
    {
        tail->next = n;
        tail = tail->next;
    }
    
    size++;
}

byte Packet::get(int index)
{    
    int counter = 0;
    Node * n = head;
    while(n != NULL)
    {
        if(counter == index)
        {
            return n->data;
            
        }
        n = n->next;
        counter++;
    }
    return -1;
}

void Packet::set(int index, byte element)
{
    int counter = 0;
    Node * n = head;
    while(n != NULL)
    {
        if(counter == index)
        {
            n->data = element;
            break;
        }
        n = n->next;
        counter++;
    }
}

void Packet::clear()
{
    if(size != 0)
    {
        Node * n = head;
        Node * newNext;
        while(n != NULL)
        {
            newNext = n->next;
            delete n;
            n = newNext;
        }
        size = 0;
        head = NULL;
        tail = NULL;
    }
}

int Packet::length()
{
    return size;
}