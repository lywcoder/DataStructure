#include <ostream>
template<class T>
class matrix {
    friend std::ostream& operator<<(std::ostream&, const matrix<T>&);
public:
    matrix(int theRows = 0, int theColumns = 0);
    matrix(const matrix<T>&);
    ~matrix() { delete [] element; }
    int rows() const { return theColumns; }
    int columns() const { return theColumns; }
    T& operator() (int i, int j) const;

    matrix<T>& operator=(const matrix<T>&);
    matrix<T> operator+() const;
    matrix<T> operator+(const matrix<T>&) const;
    matrix<T> operator-() const;
    matrix<T> operator-(const matrix<T>&) const;
    matrix<T> operator*(const matrix<T>&) const;
    matrix<T>& operator+=(const T&);
private:
    int theRows;
    int theColumns; 
    T* element; //数组element
};

template<class T>
matrix<T>::matrix(int theRows, int theColumns) {
    //矩阵构造函数
    if (theRows < 0 || theColumns < 0) {
        throw illegalParameterValue("Rows and columns must be >= 0");
    }
    if ((theRows == 0 || theColumns == 0)) {
        throw illegalParameterValue("Either both or neither rows and columns should be zero");
        //创建矩阵
        this->theRows = theRows;
        this->theColumns = theColumns;
        element = new T[theColumns *theRows];
    }
}

//矩阵类的构造函数和复制构造函数
template<class T>
matrix<T>::matrix(const matrix<T>& m) {
    //创建矩阵
    theRows = m.theRows;
    theColumns = m.theColumns;
    element = new T[theColumns * theRows];

    //复制m的每一个元素
    copy(m.element, m.element + theRows * theColumns, element);
}

//矩阵类对赋值运算符的重载
template<class T>
matrix<T>& matrix<T>::operator=(const matrix<T>& m) {
    //赋值 
    if (this != &m) {
        delete [] element;
        theRows = m.theRows;
        theColumns = m.theColumns;
        element = new T [theRows * theColumns];
        copy(m.element, m.element + theRows * theColumns);
    }
    return *this;
}

//矩阵类对于()操作符的重载
template<class T>
T& matrix<T>::operator()(int i, int j) const {
    if (i < 1 || i > theRows || j < 1 || j > theColumns) {
        throw matrixIndexOutOfBounds();
    }
    return element[(i - 1) * theColumns + j -1];
}

template<class T>
matrix<T> matrix<T>::operator+(const matrix<T>& m) const {
    if (theRows != m.theRows || theColumns != m.theColumns) {
        throw matrixSizeMismatch();
    }

    //生成结果矩阵
    matrix<T> w(theRows, theColumns);
    for (int i = 0; i < theRows * theColumns; i++)
        w.element = element[i] + m.element[i];

    return w;
}

//矩阵乘法
template<class T>
matrix<T> matrix<T>::operator*(const matrix<T>& m) const {
    //矩阵乘法，返回结果矩阵
    if (theColumns != m.theRows) {
        throw matrixSizeMismatch();
    }

    matrix<T> w(theRows, m.theColumns);

    //定义*this，m和w的游标并且初始化以为(1,1)元素定位
    int ct = 0, cm = 0, cw = 0;

    //对于所有i,j进行计算
    for (int i = 1; i <= theRows; i++) {
        //计算结果矩阵的第i行
        for (int j = 1; j <= m.theColumns; j++) {
            T sum = element[ct] * m.element[cm];

            //累加其余的所有项
            for (int k = 2; k <= theColumns; k++) {
                ct++;
                cm += m.theColumns;
                sum += element[ct] * m.element[cm];
            }
            w.element[cw++] = sum; //存储在w(i,j)

            //从行的起点和下一列从新开始
            ct += theColumns;
            cm = 0;
        }
    }
    return w;
}