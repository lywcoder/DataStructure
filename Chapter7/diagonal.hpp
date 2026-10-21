template<class T>
class diagonalMatrix {
public:
    diagonalMatrix(int theN = 10);
    ~diagonalMatrix() { delete[] element; }
    T get(int, int) const;
    void set(int, int, const T&);

protected: 
    int n;
    T* element;
};

template<class T>
diagonalMatrix<T>::diagonalMatrix(int theN) {
    if (theN < 1) {
        throw illegalParameterValue("Matrix size must be > 0");
    }

    n = theN;
    element = new T[n];
}

//get的方法
template<class T>
T diagonalMatrix<T>::get(int i, int j) const {
    //返回对应位置上面的元素
    if (i < 1 || j < 1 || i > n || j > n) {
        throw matrixIndexOutOfBounds();
    }

    if (i == j)
        return element[i - 1];
    else 
        return 0;
}

//set的方法
template<class T>
void diagonalMatrix<T>::set(int i, int j, const T& newValue) {
    if (i < 1 || j < 1 || i > n || j > n) {
        throw matrixIndexOutOfBounds();
    }

    if (i == j)
        element[i - 1] = newValue;
    else 
        if (newValue != 0) {
            throw illegalParameterValue("nondiagonal elements must be zero");
        }
}