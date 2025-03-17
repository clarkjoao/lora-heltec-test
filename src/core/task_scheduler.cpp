#include "core/task_scheduler.h"

void TaskScheduler::registerTask(std::function<void()> callback, unsigned long interval) {
    tasks.push_back(Task(callback, interval));
}

void TaskScheduler::runTasks() {
    unsigned long now = millis();
    for (auto& task : tasks) {
        if ((now - task.lastRun) >= task.interval) {
            task.callback();
            task.lastRun = now;
        }
    }
}
