#pragma once

#include <iostream>
#include <list>
#include <map>

template <typename T>
class Topology_t 
{
public:
    using list_type = std::list<std::list<T>>;
    using iterator = typename std::list<T>::iterator;
    using list_iterator = typename list_type::iterator;
    
    Topology_t() : _container(), _size(0) {};
    ~Topology_t() = default;

    void insert(const T & elem)
    {
        std::list<T> newList;
        newList.emplace_back(elem);
        ++_size;
        _container.emplace_back(newList);
    }

    bool insert(const T & parent, const T & elem) 
    {
        for (list_iterator externalIt = _container.begin(); externalIt != _container.end(); ++externalIt) {
            for (iterator internalIt = externalIt->begin(); internalIt != externalIt->end(); ++internalIt) {
                if (*internalIt == parent) {
                    externalIt->insert(++internalIt, elem);
                    ++_size;
                    return true;
                }
            }
	}
	    return false;
    }

    bool erase(const T & elem)
    {
        for (list_iterator externalIt = _container.begin(); externalIt != _container.end(); ++externalIt) {
            for (iterator internalIt = externalIt->begin(); internalIt != externalIt->end(); ++internalIt) {
                if (*internalIt == elem) {
                    if (externalIt->size() > 1) {
                        externalIt->erase(internalIt);
                    } else {
                        _container.erase(externalIt);
                    }
                    --_size;
                    return true;
                }
            }
	}
	    return false;
    }

    size_t size() 
    {
        return _size;
    }

    int find(const T & elem)
    {
        int ind = 0;
        for (auto & external : _container) {
            for (auto & internal : external) {
                if (internal == elem) {
                    return ind;
                }
            }
            ++ind;
        }

        return -1;
    }

    template <typename S>
    friend std::ostream & operator<<(std::ostream & os, const Topology_t<S> & topology)
    {
        for (auto & external : topology._container) {
            os << "{";
            for (auto & internal : external) {
                os << internal << " ";
            }
            os << "}" << std::endl;
        }

        return os;
    }

// fields struct
    list_type _container;
    size_t _size;
};