#include "Ref.h"


template<typename T>
Ref<T>::Ref(T* ptr) : _ptr(ptr), _counter(new unsigned int(1)) {}

template<typename T>
Ref<T>::Ref(const Ref& ref) : _ptr(ref._ptr), _counter(ref._counter) {
    (*_counter)++;
}

template<typename T>
Ref<T>& Ref<T>::operator=(const Ref& ref)
{
    if (this != &ref)
    {
        (*_counter)--;
        if (*_counter == 0)
        {
            delete _ptr;
            delete _counter;
        }
        _ptr = ref._ptr;
        _counter = ref._counter;
        (*_counter)++;
    }
    return *this;
}

template<typename T>
T* Ref<T>::get() const
{
    return _ptr;
}

template<typename T>
unsigned int Ref<T>::use_count() const
{
    return *_counter;
}

template<typename T>
T& Ref<T>::operator*() const
{
    return *_ptr;
}

template<typename T>
T* Ref<T>::operator->() const
{
    return _ptr;
}

template<typename T>
Ref<T>::~Ref()
{
    (*_counter)--;
    if (*_counter == 0)
    {
        delete _ptr;
        delete _counter;
    }
}