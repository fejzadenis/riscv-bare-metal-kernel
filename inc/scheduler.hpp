//
// Created by os on 6/18/26.
//

#ifndef PROJECT_BASE_V1_1_1_SCHEDULER_HPP
#define PROJECT_BASE_V1_1_1_SCHEDULER_HPP

#include "list.hpp"

class _thread;

class Scheduler {
public:
    static _thread* get();
    static void put(_thread* t);

private:
    static List<_thread> readyThreadQueue;
};


#endif //PROJECT_BASE_V1_1_1_SCHEDULER_HPP
