#ifndef CHAPTER9_QUEUE_EMPTY_HPP
#define CHAPTER9_QUEUE_EMPTY_HPP

#include <stdexcept>

class queueEmpty : public std::out_of_range {
public:
    queueEmpty() : std::out_of_range("queue is empty") {}
};

#endif
