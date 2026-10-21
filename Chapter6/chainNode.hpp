#ifndef chainNode_
#define chainNode_

//1. struct chainNode 
template <class T>
struct chainNode {
    T element;
    chainNode<T> *next;

    // method
    chainNode() {}
    chainNode(const T& element) { this->element = element; }
    chainNode(const T& element, chainNode<T>* next) {
        this->element = element;
        this->next = next;
    }
};

#endif
