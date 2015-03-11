#ifndef SEARCH_H
#define SEARCH_H

#include <vector>

namespace Util
{
    template <typename T>
    static bool BinarySearch(std::vector<T>& container,
                            T& element,
                            typename std::vector<T>::iterator result)
    {
        unsigned int left = 0;
        unsigned int right = container.size();
        unsigned int mid;

        while(right >= 1)
        {
            mid = (left + right)/2;

            if(container[mid] == element)
            {
                result = container.begin() + mid;
                return true;
            }

            Predicate comp;

            // Left
            if(comp(container[mid],element))
                right = mid - 1;
            else
                left = mid + 1;
        }
        return false;
    }
}

#endif