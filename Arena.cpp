#include "Arena.h"
#include <iostream>
#include <sstream>
#include <cctype>

Arena::Arena() : firstSnake(nullptr), lastSnake(nullptr), tickCount(0) {}

Arena::~Arena()
{

    clearSnakes();
}

// ---------------------------------------------------------------- helpers (given)

// A valid snake id is a single uppercase letter A-Z.
static bool isSnakeId(const std::string &s)
{
    return s.size() == 1 && s[0] >= 'A' && s[0] <= 'Z';
}

// ---------------------------------------------------------------- player list

Snake *Arena::findSnake(char id) const
{
    Snake *cur = firstSnake;
    while (cur != nullptr)
    {
        if (cur->getId() == id && cur->isAlive())
        {
            return cur;
        }
        cur = cur->getNext();
    }
    return nullptr;
}

void Arena::addSnake(Snake *s)
{

    if(s == nullptr) {
        return; // Do not add null snakes
    }
    s->setNext(nullptr); // Ensure the new snake's next is null
    if(firstSnake == nullptr) {
        firstSnake = s;
        lastSnake = s;
    } else {
        lastSnake->setNext(s);
        lastSnake = s;
    }
}

void Arena::clearSnakes()
{

    Snake *current = firstSnake;
    while(current != nullptr) {
        Snake *temp = current;
        current = current->getNext();
        delete temp;
    }
    firstSnake = nullptr;
    lastSnake = nullptr;

    leaderboard.clear();
}

void Arena::removeDeadSnakes()
{

    Snake *current = firstSnake;
    Snake *prev = nullptr;

    while(current != nullptr){
        if(!current -> isAlive()){
            Snake *temp = current;
            if(prev == nullptr){
                firstSnake = current -> getNext();
            } else {
                prev -> setNext(current -> getNext());
            }
            current = current -> getNext();

            if(temp == lastSnake){
                lastSnake = prev;
            }
            delete temp;    
        } else {
            prev = current;
            current = current -> getNext();
        }
    }
}

void Arena::killSnake(Snake *s)
{

    if(s == nullptr || !s->isAlive())
    {
        return; // Snake is already dead or null
    }

    std::cout << "Snake " << s->getId() << " died with length " << s->getLength()
              << " and score " << s->getScore() << "." << std::endl;

    Segment *currentSegment = s->getBody().getHead();
    while(currentSegment != nullptr)
    {
        board.set(currentSegment->getRow(), currentSegment->getCol(), FOOD_CELL);
        currentSegment = currentSegment->getNext(); 
    }
    s->setAlive(false);
    leaderboard.remove(s);          
}

// ---------------------------------------------------------------- movement

void Arena::moveSnake(Snake *s)
{

    if(s == nullptr || !s->isAlive())
    {
        return; // Snake is dead or null, do nothing
    }
    int nextRow, nextCol;
    s->getNextHeadPosition(nextRow, nextCol);

    if(!board.inBounds(nextRow, nextCol) || board.get(nextRow, nextCol) == WALL_CELL)
    {
        std::cout << "Snake " << s->getId() << " hit a wall at (" << nextRow << "," << nextCol << ")." << std::endl;
        killSnake(s);
        return;
    }

    char cellContent = board.get(nextRow, nextCol);

    if(cellContent>= 'A' && cellContent <= 'Z') // Head of another snake
    {
        Snake *otherSnake = findSnake(cellContent);
        if(otherSnake != nullptr && otherSnake != s)
        {
            std::cout << "Snake " << s->getId() << " and Snake " << otherSnake->getId()
                      << " collided head-on at (" << nextRow << "," << nextCol << ")." << std::endl;
            
            if(s->getLength() < otherSnake->getLength()){
                killSnake(s);
                return;

            }
            if(s->getLength() == otherSnake->getLength()){
                killSnake(s);
                killSnake(otherSnake);
                return;
            }

            killSnake(otherSnake);
            cellContent = FOOD_CELL; // Treat as food for the surviving snake
        }
    }
    if(cellContent == s->getId() + 'a' - 'A') // Body of itself
    {
        Segment *tail = s->getBody().getTail();
        if(tail->getRow() != nextRow || tail->getCol() != nextCol) // Not the tail
        {
            std::cout << "Snake " << s->getId() << " bit itself at (" << nextRow << "," << nextCol << ")." << std::endl;
            killSnake(s);
            return;
        }

    }
    if(cellContent >= 'a' && cellContent <= 'z'){
        char otherId = cellContent - 'a' + 'A';
        Snake *otherSnake = findSnake(otherId);
        if(otherSnake != nullptr && otherSnake != s)
        {
           SnakeBody removedBody = otherSnake->getBody().splitAt(nextRow, nextCol);
           int m = removedBody.getLength();

           Segment *curr = removedBody.getHead();
           while(curr != nullptr){
                board.set(curr->getRow(), curr->getCol(), FOOD_CELL);
                curr = curr->getNext();
           }
           std::cout << "Snake " << s->getId() << " bit Snake " << otherSnake->getId()
                     << " at (" << nextRow << "," << nextCol << "). Snake " << otherSnake->getId()
                     << " lost " << m << " segment(s)." << std::endl;
            cellContent = FOOD_CELL; // Treat as food for the biting snake

        }
    }

    if(cellContent == FOOD_CELL)
    {
        Segment *oldHead = s->getBody().getHead();

        board.set(oldHead->getRow(), oldHead->getCol(), s->getId() + 'a' - 'A'); // Lowercase
        s->getBody().pushFront(nextRow, nextCol);
        board.set(nextRow, nextCol, s->getId()); // Uppercase
        s->addScore(1); 
        leaderboard.reposition(s); // Update leaderboard position
        std::cout << "Snake " << s->getId() << " ate food at (" << nextRow << "," << nextCol << ")." << std::endl;
        return;
    }

    int tailRow, tailCol;
    s->getBody().popBack(tailRow, tailCol);
    board.set(tailRow, tailCol, EMPTY_CELL);
    Segment *oldHead = s->getBody().getHead();
    if(oldHead != nullptr)
    {
        board.set(oldHead->getRow(), oldHead->getCol(), s->getId() + 'a' - 'A'); // Lowercase
    }
    s->getBody().pushFront(nextRow, nextCol);
    board.set(nextRow, nextCol, s->getId()); // Uppercase

}

void Arena::tick()
{

    tickCount++;
    std::cout << "--- Tick " << tickCount << " ---" << std::endl;
    Snake *current = firstSnake;
    while(current != nullptr)
    {
        if(current->isAlive())
        {
            moveSnake(current);
        }
        current = current->getNext();
    }
    removeDeadSnakes();
}

// ---------------------------------------------------------------- commands

void Arena::cmdInitBoard(const std::string &args)
{
    
    std::istringstream ss(args);
    int r, c;
    ss >> r >> c;
    clearSnakes();

    tickCount = 0;
    board = Board(r, c);
    std::cout << "Board initialized with " << r << " rows and " << c << " columns." << std::endl;
}

// Given ready, as an example for parsing.
// (Checking the format is not required: the input is always well-formed.)
void Arena::cmdWall(const std::string &args)
{
    std::istringstream ss(args);
    int r, c;
    ss >> r >> c;
    if (!board.inBounds(r, c) || board.get(r, c) != EMPTY_CELL)
    {
        std::cout << "Error: Cell (" << r << "," << c << ") is not available." << std::endl;
        return;
    }
    board.set(r, c, WALL_CELL);
    std::cout << "Wall placed at (" << r << "," << c << ")." << std::endl;
}

void Arena::cmdFood(const std::string &args)
{

    std::istringstream ss(args);
    int r, c;
    ss >> r >> c;
    if (!board.inBounds(r, c) || board.get(r, c) != EMPTY_CELL)
    {
        std::cout << "Error: Cell (" << r << "," << c << ") is not available." << std::endl;
        return;
    }
    board.set(r, c, FOOD_CELL);
    std::cout << "Food placed at (" << r << "," << c << ")." << std::endl;
}

void Arena::cmdSpawn(const std::string &args)
{

    std::istringstream ss(args);
    char id;
    int r, c, len;
    std::string dirStr;
    ss >> id >> r >> c >> dirStr >> len;

    if(findSnake(id) != nullptr)
    {
        std::cout << "Error: Snake " << id << " already exists." << std::endl;
        return;
    }
    Direction dir = Direction::UP; // Default
    if(dirStr == "UP") dir = Direction::UP;
    else if(dirStr == "DOWN") dir = Direction::DOWN;
    else if(dirStr == "LEFT") dir = Direction::LEFT;
    else if(dirStr == "RIGHT") dir = Direction::RIGHT;

    int dr = 0, dc = 0;
    if(dir == Direction::UP) dr = 1;
    else if(dir == Direction::DOWN) dr = -1;
    else if(dir == Direction::LEFT) dc = 1;
    else if(dir == Direction::RIGHT) dc = -1;

    for(int i = 0; i < len; ++i)
    {
        int checkR = r + i * dr;
        int checkC = c + i * dc;
        if(!board.inBounds(checkR, checkC) || board.get(checkR, checkC) != EMPTY_CELL)
        {
            std::cout << "Error: Cannot spawn Snake " << id << " at (" << r << "," << c << ")." << std::endl;
            return;
        }
    }
    Snake *newSnake = new Snake(id, dir);
    for(int i = 0; i < len; ++i)
    { int nr = r + i * dr;
      int nc = c + i * dc;
      newSnake->getBody().pushBack(nr, nc);
      if(i == 0)
          board.set(nr, nc, id); // Head
      else
          board.set(nr, nc, id + 'a' - 'A'); // Body
    }
    addSnake(newSnake);
    leaderboard.insertSorted(newSnake);
    std::cout << "Snake " << id << " spawned at (" << r << "," << c << ") facing " << dirStr << " with length " << len << "." << std::endl;



}

void Arena::cmdTurn(const std::string &args)
{

    std::istringstream ss(args);
    char id;
    std::string dirStr;
    ss >> id >> dirStr;

    Snake *s = findSnake(id);
    if(s == nullptr){
        std::cout << "Error: Snake " << id << " not found." << std::endl;
        return;
    }

    Direction dir = Direction::UP; // Default
    if(dirStr == "UP") dir = Direction::UP;
    else if(dirStr == "DOWN") dir = Direction::DOWN;
    else if(dirStr == "LEFT") dir = Direction::LEFT;
    else if(dirStr == "RIGHT") dir = Direction::RIGHT;

    if(!s->turn(dir)){
        std::cout << "Error: Snake " << id << " cannot turn back." << std::endl;
        return;
    }
    std::cout << "Snake " << id << " is now facing " << dirStr << "." << std::endl;
}

void Arena::cmdReverse(const std::string &args)
{
    std::istringstream ss(args);
    char id;
    ss >> id;
    Snake *s = findSnake(id);
    if(s == nullptr){
        std::cout << "Error: Snake " << id << " not found." << std::endl;
        return;
    }
    Segment *oldHead = s->getBody().getHead();
    Segment *oldTail = s->getBody().getTail();

    board.set(oldHead->getRow(), oldHead->getCol(), id + 'a' - 'A'); // Lowercase
    s->reverse();
    board.set(oldTail->getRow(), oldTail->getCol(), id); // Uppercase
    std::cout << "Snake " << id << " reversed and is now facing " << directionToString(s->getDirection()) << "." << std::endl;
}

void Arena::cmdTick(const std::string &args)
{
    std::istringstream ss(args);
    int n = 1; // Default
    ss >> n;
    for(int i = 0; i < n; ++i)
    {
        tick();
    }
}

// Given ready
void Arena::printSnakes() const
{
    std::cout << "--- Snakes ---" << std::endl;
    if (!firstSnake)
    {
        std::cout << "No snakes in the arena." << std::endl;
        return;
    }
    for (Snake *cur = firstSnake; cur; cur = cur->getNext())
    {
        std::cout << cur->getId() << " " << directionToString(cur->getDirection())
                  << " length=" << cur->getLength() << " score=" << cur->getScore()
                  << ": " << cur->getBody() << std::endl;
    }
}

// Given ready
void Arena::processCommand(const std::string &line)
{
    std::istringstream ss(line);
    std::string cmd;
    if (!(ss >> cmd))
        return;
    std::string args;
    std::getline(ss, args);

    if (cmd == "INIT_BOARD")
    {
        cmdInitBoard(args);
        return;
    }

    bool knownBoardCmd = cmd == "WALL" || cmd == "FOOD" || cmd == "SPAWN" || cmd == "TURN" ||
                         cmd == "REVERSE" || cmd == "TICK" || cmd == "PRINT_BOARD" ||
                         cmd == "PRINT_SNAKES" || cmd == "PRINT_LEADERBOARD";
    if (!knownBoardCmd)
    {
        std::cout << "Error: Unknown command '" << cmd << "'." << std::endl;
        return;
    }
    if (!board.isInitialized())
    {
        std::cout << "Error: Board not initialized." << std::endl;
        return;
    }

    if (cmd == "WALL")
        cmdWall(args);
    else if (cmd == "FOOD")
        cmdFood(args);
    else if (cmd == "SPAWN")
        cmdSpawn(args);
    else if (cmd == "TURN")
        cmdTurn(args);
    else if (cmd == "REVERSE")
        cmdReverse(args);
    else if (cmd == "TICK")
        cmdTick(args);
    else if (cmd == "PRINT_BOARD")
        board.print();
    else if (cmd == "PRINT_SNAKES")
        printSnakes();
    else if (cmd == "PRINT_LEADERBOARD")
        leaderboard.print();
}
