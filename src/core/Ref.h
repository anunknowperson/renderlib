#pragma once
#include <memory>

template<typename T>
class Ref {

private:
    T* _ptr;
    unsigned int* _counter;

public:
    Ref(T* ptr = nullptr);

    Ref(const Ref& ref);

    Ref& operator=(const Ref& ref);

    T* get() const;
    unsigned int use_count() const;

    ~Ref();

    T& operator*() const;
    T* operator->() const;
};
