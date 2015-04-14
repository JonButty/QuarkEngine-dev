#ifndef CYCLICNUMBERGENERATOR_H
#define CYCLICNUMBERGENERATOR_H

#include <stack>

namespace Math
{
    template <typename T>
    class CyclicNumGen
    {
    public:
        CyclicNumGen(T start,
                     T size)
        {
            for(T i = start; i < size; ++i)
                pool_.push(i);
        }

        T Generate()
        {
            T ret(pool_.top());
            pool_.pop();
            pool_.push(ret);
            return ret;
        }
    private:
        std::stack<T> pool_;
    };
}

#endif