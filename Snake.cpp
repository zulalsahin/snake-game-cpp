#include "Snake.h"

Snake::Snake(char _id, Direction _direction)
    : id(_id), direction(_direction), score(0), alive(true), next(nullptr) {}

bool Snake::turn(Direction newDir)
{
    if(body.isEmpty())
    {
        return false; // Cannot turn an empty snake
    }

    if(body.getLength() == 1)
    {
        direction = newDir;
        return true;
    }

    Segment *headSegment = body.getHead();
    Segment *neckSegment = headSegment->getNext();

    int newRow = headSegment->getRow();
    int newCol = headSegment->getCol();

    if(newDir == Direction::UP) newRow--;
    else if(newDir == Direction::DOWN) newRow++;
    else if(newDir == Direction::LEFT) newCol--;
    else if(newDir == Direction::RIGHT) newCol++;

    if(newRow == neckSegment->getRow() && newCol == neckSegment->getCol())
    {
        return false; // Invalid turn, would collide with neck
    }

    direction = newDir;

    return true;
}

void Snake::reverse()
{
    // TODO: Reverse the body. New direction: from the new second segment towards the new head.
    //   e.g. head (2,1), second (2,2)  -> LEFT
    // If the snake has length 1, the direction just becomes the opposite.

    body.reverse();

    if(body.isEmpty()) {
        return; // No segments to determine direction
    }
    if (body.getLength() == 1) {
        direction = oppositeDirection(direction);
    } else {
        Segment *newHead = body.getHead();
        Segment *newSecond = newHead->getNext();
        if (newSecond) {
            int dRow = newHead->getRow() - newSecond->getRow();
            int dCol = newHead->getCol() - newSecond->getCol();
            if (dRow == -1 && dCol == 0) direction = Direction::UP;
            else if (dRow == 1 && dCol == 0) direction = Direction::DOWN;
            else if (dRow == 0 && dCol == -1) direction = Direction::LEFT;
            else if (dRow == 0 && dCol == 1) direction = Direction::RIGHT;
        }
    }

}

void Snake::getNextHeadPosition(int &row, int &col) const
{

    Segment *headSegment = body.getHead();
    if(!headSegment) {
        row = 0; // Invalid position if there's no head
        col = 0;
        return;
    }
    row = headSegment->getRow();
    col = headSegment->getCol();

    if(direction == Direction::UP) row--;
    else if(direction == Direction::DOWN) row++;
    else if(direction == Direction::LEFT) col--;
    else if(direction == Direction::RIGHT) col++;
}
