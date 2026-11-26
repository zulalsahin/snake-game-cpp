#include "SnakeBody.h"

SnakeBody::~SnakeBody() { clear(); }

SnakeBody::SnakeBody(SnakeBody &&other) noexcept
    : head(nullptr), tail(nullptr), length(0){

        head = other.head;
        tail = other.tail;
        length = other.length;

        other.head = nullptr;
        other.tail = nullptr;
        other.length = 0;
        
    }


SnakeBody &SnakeBody::operator=(SnakeBody &&other) noexcept
{
    if(this != &other)
    {
        clear(); // Free existing segments
        head = other.head;
        tail = other.tail;
        length = other.length;

        other.head = nullptr;
        other.tail = nullptr;
        other.length = 0;
    }
    
    return *this;
}

bool SnakeBody::isEmpty() const
{

    if(length == 0)
    {
        return true;
    }
    return false;
}

void SnakeBody::pushFront(int row, int col)
{

    Segment *newNode = new Segment(row, col);
    newNode->setNext(head);
    if (head != nullptr){
        head->setPrev(newNode);
    } else {
        tail = newNode; // If the list was empty, tail also points to the new node
    }
    head = newNode;
    length++;
}

void SnakeBody::pushBack(int row, int col)
{
    Segment *newNode = new Segment(row, col);
    newNode -> setPrev(tail);
    if (tail != nullptr){
        tail->setNext(newNode);
    } else {
        head = newNode; // If the list was empty, head also points to the new node
    }
    tail = newNode;
    length++;
}

bool SnakeBody::popBack(int &row, int &col)
{

    if(isEmpty())
    {
        return false;
    }

    row = tail->getRow();
    col = tail->getCol();

Segment *oldTail = tail;
    tail = tail->getPrev();
    if (tail != nullptr) {
        tail->setNext(nullptr);
    } else {
        head = nullptr; // If the list is now empty, head should also be nullptr
    }
    delete oldTail;
    length--;
    return true;

}

void SnakeBody::reverse()
{

    Segment *current = head;

    while (current != nullptr){
        Segment *temp = current->getNext();
        current->setNext(current->getPrev());
        current->setPrev(temp);
        current = temp;

    }

    Segment *temphead = head;
    head = tail;
    tail = temphead;

}

Segment *SnakeBody::findAt(int row, int col) const
{

    Segment *current = head;
    while (current != nullptr) {
        if (current->getRow() == row && current->getCol() == col) {
            return current;
        }
        current = current->getNext();
    }
    return nullptr;
}

SnakeBody SnakeBody::splitAt(int row, int col)
{
    SnakeBody cut; // returned by value -> moved, not copied

    Segment *splitNode = findAt(row, col);
    if (splitNode == nullptr) {
        return cut; // No segment at (row, col), return empty body
    }
    Segment *previousNode = splitNode->getPrev();
    if (previousNode != nullptr) {
        previousNode->setNext(nullptr);
    } else {
        head = nullptr; // If splitNode was the head, the original body is now empty
    }
    splitNode->setPrev(nullptr);
    cut.head = splitNode;
    cut.tail = tail;
    tail = previousNode;
    cut.length = 0;
    Segment *current = cut.head;
    while (current != nullptr) {
        cut.length++;
        current = current->getNext();
    }
    length = length - cut.length; // Update the length of the original body

    return cut;
}

void SnakeBody::clear()
{

    Segment *current = head;
    while (current != nullptr) {
        Segment *nextNode = current->getNext();
        delete current;
        current = nextNode;
    }
    head = nullptr;
    tail = nullptr;
    length = 0;
}

// Given ready
std::ostream &operator<<(std::ostream &os, const SnakeBody &b)
{
    for (Segment *cur = b.head; cur; cur = cur->getNext())
    {
        os << *cur;
        if (cur->getNext())
            os << " ";
    }
    return os;
}
