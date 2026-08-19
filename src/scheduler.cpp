//
// Created by os on 6/18/26.
//

#include "../inc/scheduler.hpp"
#include "../inc/_thread.hpp"

List<_thread> Scheduler::readyThreadQueue;

_thread* Scheduler::get() {
    return readyThreadQueue.removeFirst();
}

void Scheduler::put(_thread* t) {
    readyThreadQueue.addLast(t);
}
