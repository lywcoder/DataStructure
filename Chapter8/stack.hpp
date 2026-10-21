template<class T>
class stack {
public:
    virtual ~stack() {}

    virtual bool empty() const = 0;

    virtual int size() const = 0;

    virtual T& top() = 0;  // 返回栈顶元素的引用

    virtual void pop() = 0;

    virtual void push(const T& theElement) = 0;
};
