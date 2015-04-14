#ifndef CONSTRUCTORLIST_H
#define CONSTRUCTORLIST_H

namespace Util
{
    template <typename T>
    class ConstructorList
    {
    public:
        ConstructorList()
        {
            next_ = Head();
            Head() = static_cast<const T *>(this);
        }

        const T* Next() const
        {
            return next_;
        }

        static const T*& Head()
        {
            static const T* head = 0;
            return head;
        }

    private:
        const T* next_;
    };
}
#endif