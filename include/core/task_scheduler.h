#ifndef TASK_SCHEDULER_H
#define TASK_SCHEDULER_H

#include <vector>
#include <functional>
#include <Arduino.h>

class TaskScheduler {
private:
    struct Task {
        std::function<void()> callback;
        unsigned long interval;
        unsigned long lastRun;

        Task(std::function<void()> cb, unsigned long intv)
            : callback(cb), interval(intv), lastRun(0) {}
    };

    std::vector<Task> tasks;

public:
    void registerTask(std::function<void()> callback, unsigned long interval);
    void runTasks();
};

#endif
