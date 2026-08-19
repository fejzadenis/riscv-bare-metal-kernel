#ifndef PROJECT_BASE_V1_1_1_LIST_HPP
#define PROJECT_BASE_V1_1_1_LIST_HPP
#include "MemoryAllocator.hpp"

template<typename T>
class List {
private:
    struct Elem {
        T*    data;
        Elem* next;
        Elem(T* data, Elem* next) : data(data), next(next) {}

        // Interna alokacija — direktno kroz MemoryAllocator, BEZ ecall-a.
        // List<T> se koristi samo unutar jezgra (Scheduler, _sem), nikad iz
        // korisnickog koda, pa ovo ne krsi nikakav API ugovor.
        void* operator new(size_t size) {
            return MemoryAllocator::memoryAllocate(size);
        }
        void operator delete(void* ptr) {
            MemoryAllocator::memoryDeallocate(ptr);
        }
    };
    Elem* head;
    Elem* tail;
public:
    List() : head(nullptr), tail(nullptr) {}
    List(const List<T>&)            = delete;
    List<T>& operator=(const List<T>&) = delete;

    void addFirst(T* data) {
        Elem* elem = new Elem(data, head);
        head = elem;
        if (!tail) tail = head;
    }
    void addLast(T* data) {
        Elem* elem = new Elem(data, nullptr);
        if (tail) {
            tail->next = elem;
            tail = elem;
        } else {
            head = tail = elem;
        }
    }
    T* removeFirst() {
        if (!head) return nullptr;
        Elem* elem = head;
        head = head->next;
        if (!head) tail = nullptr;
        T* ret = elem->data;
        delete elem;
        return ret;
    }
    T* peekFirst() {
        if (!head) return nullptr;
        return head->data;
    }
    bool isEmpty() const {
        return head == nullptr;
    }
};
#endif //PROJECT_BASE_V1_1_1_LIST_HPP