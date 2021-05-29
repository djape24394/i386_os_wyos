#include <multitasking.h>

using namespace myos;
using namespace myos::common;

Task::Task(GlobalDescriptorTable *gdt, void entry_point_function())
{
    cpu_state = (CPUstate*)(stack + sizeof(stack) - sizeof(CPUstate));
    
    cpu_state->eax = 0U; // accumulaor
    cpu_state->ebx = 0U; // base register 
    cpu_state->ecx = 0U; // counting register 
    cpu_state->edx = 0U; // data register
        
    cpu_state->esi = 0U; // stack index
    cpu_state->edi = 0U; // data index
    cpu_state->ebp = 0U; // stack base pointer

    // cpu_state->esp = ; // only for user spaces, not dealing with different security levels now
    cpu_state->eip = (uint32_t) entry_point_function;
    cpu_state->cs = gdt->getCodeSegmentAddressOffset(); 
    // cpu_state->ss = 0U; // not dealing with different security levels now
    cpu_state->eflags = 0x202U;
}

Task::~Task()
{
}

TaskManager::TaskManager(): nof_tasks{0}, current_task_index{-1}
{
}

TaskManager::~TaskManager()
{
}

bool TaskManager::addTask(Task *task)
{
    if(nof_tasks < max_nof_tasks)
    {
        tasks[nof_tasks++] = task;
        return true;
    }
    return false;
}

CPUstate *TaskManager::schedule(CPUstate *cpu_state)
{
    if(nof_tasks <= 0) return cpu_state;
    if(current_task_index >= 0)
    {
        tasks[current_task_index]->cpu_state = cpu_state;
    }
    current_task_index = (current_task_index + 1) % nof_tasks;
    return tasks[current_task_index]->cpu_state;
}
