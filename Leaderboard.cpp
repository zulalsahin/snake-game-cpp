#include "Leaderboard.h"
#include <iostream>

Leaderboard::~Leaderboard() { clear(); }

void Leaderboard::insertSorted(Snake *s)
{

    if(s == nullptr) {
        return; // Do not insert null snakes
    }
    RankNode *newNode = new RankNode(s);

    if(head == nullptr) {
        head = newNode;
        size++;
        return;
    }

    if(s->getScore() > head->snake->getScore() || 
       (s->getScore() == head->snake->getScore() && s->getId() < head->snake->getId())) {
        newNode->next = head;
        head = newNode;
        size++;
        return;
    }

    RankNode *current = head;
    while(current->next != nullptr &&
          (current->next->snake->getScore() > s->getScore() ||
          (current->next->snake->getScore() == s->getScore() && current->next->snake->getId() < s->getId()))) {
        current = current->next;
    }
    newNode->next = current->next;
    current->next = newNode;
    size++;
}

bool Leaderboard::remove(Snake *s)
{
    // TODO: Find the node whose snake == s, unlink and delete the NODE.
    // Do NOT delete the snake itself; the Arena owns the snakes.
    // Return false if not found.

    if(s == nullptr || head == nullptr) {
        return false; // Nothing to remove
    }

    if(head->snake == s) {
        RankNode *temp = head;
        head = head->next;
        delete temp;
        size--;
        return true;
    }

    RankNode *current = head;
    while(current->next != nullptr) {
        if(current->next->snake == s) {
            RankNode *temp = current->next;
            current->next = current->next->next;
            delete temp;
            size--;
            return true;
        }
    current = current->next;
    }
    return false;
}

void Leaderboard::reposition(Snake *s)
{
    if(s == nullptr) {
        return; // Nothing to reposition
    }
    if(remove(s)) {
        insertSorted(s);
    }
}

void Leaderboard::clear()
{

    RankNode *current = head;
    while(current != nullptr) {
        RankNode *temp = current;
        current = current->next;
        delete temp;
    }
    head = nullptr;
    size = 0;
}

// Given ready
void Leaderboard::print() const
{
    std::cout << "--- Leaderboard ---" << std::endl;
    if (!head)
    {
        std::cout << "No snakes in the arena." << std::endl;
        return;
    }
    int rank = 1;
    for (RankNode *cur = head; cur; cur = cur->next, ++rank)
    {
        std::cout << rank << ". " << cur->snake->getId()
                  << " score=" << cur->snake->getScore()
                  << " length=" << cur->snake->getLength() << std::endl;
    }
}
