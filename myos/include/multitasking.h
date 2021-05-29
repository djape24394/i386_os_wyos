#ifndef MYOS_MULTITASKING
#define MYOS_MULTITASKING

#include <common/types.h>
#include <GlobalDescriptorTable.h>

namespace myos
{
    struct CPUstate
    {
        // PUSHED BY O
        common::uint32_t eax; // accumulaor
        common::uint32_t ebx; // base register 
        common::uint32_t ecx; // counting register 
        common::uint32_t edx; // data register
        
        common::uint32_t esi; // stack index
        common::uint32_t edi; // data index
        common::uint32_t ebp; // stack base pointer

        // common::uint32_t gs;
        // common::uint32_t fs; 
        // common::uint32_t es; 
        // common::uint32_t ds; 
 
        common::uint32_t error; // error code

        // PUSHED BY CPU
        common::uint32_t eip;    // instruction pointer
        common::uint32_t cs;     // code segment
        common::uint32_t eflags; // instruction pointer
        common::uint32_t esp;    // stack pointer
        common::uint32_t ss;     // stack segment
    } __attribute__((packed));

    class Task
    {
        friend class TaskManager;
    private:
        common::uint8_t stack[4096]; //KB
        CPUstate* cpu_state;
    public:
        Task(GlobalDescriptorTable *gdt, void entry_point_function());
        ~Task();
    };

    class TaskManager
    {
    private:
        static constexpr int max_nof_tasks = 256;
        Task *tasks[max_nof_tasks];
        int nof_tasks;
        int current_task_index;
    public:
        TaskManager();
        ~TaskManager();
        bool addTask(Task* task);
        CPUstate *schedule(CPUstate *cpu_state);
    };
} // namespace myos

#endif // MYOS_MULTITASKING
