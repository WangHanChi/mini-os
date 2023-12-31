#include <stdint.h>
#include "thread.h"
#include "os.h"
#include "malloc.h"
#include "reg.h"

#define THREAD_PSP	0xFFFFFFFD

/* Thread Control Block */
typedef struct {
	void *stack;
	void *orig_stack;
	uint8_t in_use;
} tcb_t;

static tcb_t tasks[MAX_TASKS];
static int lastTask;

/* Caution: Without naked attribute, GCC will normally push r7 which is used
 * for stack pointer. If so, after restoring the tasks' context, we will get
 * wrong stack pointer. PendSV handles calling convention itself.
 */
void __attribute__((naked)) pendsv_handler()
{
	/* Save the old task's context[1]*/
	asm volatile("mrs   r0, psp\n"
	             "stmdb r0!, {r4-r11, lr}\n");
	/* To get the task pointer address from result r0 */
	asm volatile("mov   %0, r0\n" : "=r" (tasks[lastTask].stack));

	/* Find a new task to run */
	while (1) {
		lastTask++;
		if (lastTask == MAX_TASKS)
			lastTask = 0;
		if (tasks[lastTask].in_use) {
			/* Move the task's stack pointer address into r0 */
			asm volatile("mov r0, %0\n" : : "r" (tasks[lastTask].stack));
			/* Restore the new task's context and jump to the task */
			asm volatile("ldmia r0!, {r4-r11, lr}\n"
			             "msr psp, r0\n"
			             "bx lr\n");
		}
	}
}

void systick_handler()
{
	*SCB_ICSR |= SCB_ICSR_PENDSVSET;
}

/* As PendSV does, this function also switches context and
 * maintain assembly on its own. This avoids the compiler
 * modifying register at prologue/epilogue sequences and
 * corrupts inline-assembly usage.
 */
void __attribute__((naked)) thread_start()
{
	lastTask = 0;

	/* Reset APSR before context switch.
	 * Make sure we have a _clean_ PSR for the task.
	 */
	asm volatile("mov r0, #0\n"
	             "msr APSR_nzcvq, r0\n");
	/* To bridge the variable in C and the register in ASM,
	 * move the task's stack pointer address into r0.
	 * http://www.ethernut.de/en/documents/arm-inline-asm.html
	 */
	asm volatile("mov r0, %0\n" : : "r" (tasks[lastTask].stack));
	asm volatile("msr psp, r0\n"
	             "mov r0, #3\n"
	             "msr control, r0\n"
	             "isb\n");

	asm volatile("pop {r4-r11, lr}\n"
	             "ldr r0, [sp]\n");
	/* Okay, we are ready to run first task, get address from
	 * stack[15]. We just pop 9 register so #24 comes from
	 * (15 - 9) * sizeof(entry of sp) = 6 * 4.
	 */
	asm volatile("ldr pc, [sp, #24]\n");

	print_str("Oh, no, reach thread start end!\n\r");
	/* Never reach here */
	while(1);
}

int thread_create(void (*run)(void *), void *userdata)
{
	/* Find a free thing */
	int threadId = 0;
	uint32_t *stack;

	for (threadId = 0; threadId < MAX_TASKS; threadId++) {
		if (tasks[threadId].in_use == 0)
			break;
	}

	if (threadId == MAX_TASKS)
		return -1;

	/* Create the stack */
	stack = (uint32_t *) malloc(STACK_SIZE * sizeof(uint32_t));
	tasks[threadId].orig_stack = stack;
	if (stack == 0)
		return -1;

	stack += STACK_SIZE - 17; /* End of stack, minus what we are about to push */
	stack[8] = (unsigned int) THREAD_PSP;
	stack[9] = (unsigned int) userdata;
	stack[14] = (unsigned) &thread_self_terminal;
	stack[15] = (unsigned int) run;
	stack[16] = (unsigned int) 0x01000000; /* PSR Thumb bit */

	/* Construct the control block */
	tasks[threadId].stack = stack;
	tasks[threadId].in_use = 1;

	return threadId;
}

void thread_kill(int thread_id)
{
	tasks[thread_id].in_use = 0;

	/* Free the stack */
	free(tasks[thread_id].orig_stack);
}

void thread_self_terminal()
{
	/* This will kill the stack.
	 * For now, disable context switches to save ourselves.
	 */
	asm volatile("cpsid i\n");
	thread_kill(lastTask);
	asm volatile("cpsie i\n");

	/* And now wait for death to kick in */
	while (1);
}

/* [1] Exception entry only saves R0-R4, R12, LR, PC and xPSR,
 *     which means PendSV has to save other context (R4-R11) itself to
 *     complete context switch.
 *     Also, pushing LR (=EXC_RETURN) into thread stack before switching
 *     task is needed in order to perform exception return next time the task
 *     is selected to run.
 *     Reference:
 *         Cortex-M4 Devices: Generic User Guide (ARM DUI 0553A):
 *         2.3.7 Exception entry and return
 */