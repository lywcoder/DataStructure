#include <cstddef>
#include <iterator>
template<class iterator>
void copy(iterator start, iterator end, iterator to) {
    while (start != end) {
        *to = *start;
        start++;
        to++;
    }
}

template<class T>
class iterator {
    public:
        typedef std::bidirectional_iterator_tag iterator_category;
        typedef T value_type;
        typedef ptrdiff_t difference_type;
        typedef T* pointer;
        typedef T& reference;
        
        iterator(T* thePosition = 0) {
            position = thePosition;
        }

        T& operator*() const { return *position; }
        T* operator->() const { return &*position; }

        iterator& operator++() {
            ++position;
            return *this;
        }

        iterator operator++(int) {
            iterator old = *this;
            ++position;
            return old;
        }

        iterator& operator--() {
            --position;
            return *this;
        }

        iterator operator--(int) {
            iterator old = *this;
            --position;
            return old;
        }

        bool operator!=(const iterator right) const {
            return position != right.position;
        }

        bool operator==(const iterator right) const {
            return position == right.position;
        }
        
    protected:
        T* position;
};