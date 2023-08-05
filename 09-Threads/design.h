
/* user task stack in memory */

/* This is how we simulate stack handling that pendsv_handler
* does. Thread_create sets 17 entries in stack, and the 9
* entries we pop here will be pushed back in pendsv_handler
* in the same order.
*
*
*                      pop {r4-r11, lr}
*                      ldr r0, [sp]
*          stack
*  offset -------
*        |   16  | <- Reset value of PSR
*         -------
*        |   15  | <- Task entry
*         -------
*        |   14  | <- LR for task
*         -------
*        |  ...  |                             register
*         -------                              -------
*        |   9   | <- Task argument ---->     |   r0  |
* psp after pop--<                             -------
*        |   8   | <- EXC_RETURN    ---->     |   lr  |
*         -------                              -------
*        |   7   |                            |  r11  |
*         -------                              -------
*        |  ...  |                            |  ...  |
*         -------                              -------
*        |   0   |                            |   r4  |
* psp ->  -------                              -------
*
* Instead of "pop {r0}", use "ldr r0, [sp]" to ensure consistent
* with the way how PendSV saves _old_ context[1].
*/