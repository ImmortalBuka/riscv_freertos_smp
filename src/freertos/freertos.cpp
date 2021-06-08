#define portBYTE_ALIGNMENT_MASK			3
#define portBYTE_ALIGNMENT				4
#define configTOTAL_HEAP_SIZE			0x1000
#define configMAX_TASK_NAME_LEN			16
//
enum class status
{
	errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY = -1,
	errQUEUE_BLOCKED = -4,
	errQUEUE_YIELD = -5,
};
typedef status	BaseType_t;
typedef void (* TaskFunction_t)( void * );
typedef uint32_t UBaseType_t;
typedef uint32_t StackType_t;
typedef struct
{
	volatile StackType_t* pxTopOfStack;
	ListItem_t xStateListItem;
	ListItem_t xEventListItem;
	UBaseType_t uxPriority;
	StackType_t * pxStack;
	char pcTaskName[configMAX_TASK_NAME_LEN];
#if ( ( portSTACK_GROWTH > 0 ) || ( configRECORD_STACK_HIGH_ADDRESS == 1 ) )
	StackType_t * pxEndOfStack; /*< Points to the highest valid address for the stack. */
#endif
#if ( portCRITICAL_NESTING_IN_TCB == 1 )
	UBaseType_t uxCriticalNesting; /*< Holds the critical section nesting depth for ports that do not maintain their own count in the port layer. */
#endif
#if ( configUSE_TRACE_FACILITY == 1 )
	UBaseType_t uxTCBNumber;  /*< Stores a number that increments each time a TCB is created.  It allows debuggers to determine when a task has been deleted and then recreated. */
	UBaseType_t uxTaskNumber; /*< Stores a number specifically for use by third party trace code. */
#endif
#if ( configUSE_MUTEXES == 1 )
	UBaseType_t uxBasePriority; /*< The priority last assigned to the task - used by the priority inheritance mechanism. */
	UBaseType_t uxMutexesHeld;
#endif
#if ( configUSE_APPLICATION_TASK_TAG == 1 )
	TaskHookFunction_t pxTaskTag;
#endif
#if ( configNUM_THREAD_LOCAL_STORAGE_POINTERS > 0 )
	void * pvThreadLocalStoragePointers[ configNUM_THREAD_LOCAL_STORAGE_POINTERS ];
#endif
#if ( configGENERATE_RUN_TIME_STATS == 1 )
	uint32_t ulRunTimeCounter; /*< Stores the amount of time the task has spent in the Running state. */
#endif
#if ( configUSE_TASK_NOTIFICATIONS == 1 )
	volatile uint32_t ulNotifiedValue[ configTASK_NOTIFICATION_ARRAY_ENTRIES ];
	volatile uint8_t ucNotifyState[ configTASK_NOTIFICATION_ARRAY_ENTRIES ];
#endif
/* See the comments in FreeRTOS.h with the definition of tskSTATIC_AND_DYNAMIC_ALLOCATION_POSSIBLE. */
#if ( tskSTATIC_AND_DYNAMIC_ALLOCATION_POSSIBLE != 0 ) /*lint !e731 !e9029 Macro has been consolidated for readability reasons. */
	uint8_t ucStaticallyAllocated;                     /*< Set to pdTRUE if the task is a statically allocated to ensure no attempt is made to free the memory. */
#endif
#if ( INCLUDE_xTaskAbortDelay == 1 )
	uint8_t ucDelayAborted;
#endif
#if ( configUSE_POSIX_ERRNO == 1 )
	int iTaskErrno;
#endif
} TCB_t;
//
BaseType_t xTaskCreate(TaskFunction_t pxTaskCode, const char* const pcName, const size_t usStackDepth, void * const pvParameters, UBaseType_t uxPriority, TCB_t* const pxCreatedTask);
void* pvPortMalloc(size_t xWantedSize);
void vPortFree(void* pv);
//
uint8_t ucHeap[configTOTAL_HEAP_SIZE] __attribute((aligned(4)));
static size_t xNextFreeByte = 0;
//
BaseType_t xTaskCreate(TaskFunction_t pxTaskCode, const char* const pcName,
	const size_t usStackDepth, void * const pvParameters, UBaseType_t uxPriority,
	TCB_t* const pxCreatedTask)
{
	TCB_t* pxNewTCB;
	BaseType_t xReturn;
	StackType_t* pxStack;
	pxStack = pvPortMalloc(usStackDepth*sizeof(StackType_t));
	if(pxStack != NULL)
	{
		pxNewTCB = (TCB_t*)pvPortMalloc(sizeof(TCB_t));
		if(pxNewTCB != NULL) pxNewTCB->pxStack = pxStack;
		else vPortFree(pxStack);
	}
	else pxNewTCB = NULL;
	if(pxNewTCB != NULL)
	{
#if ( tskSTATIC_AND_DYNAMIC_ALLOCATION_POSSIBLE != 0 ) /*lint !e9029 !e731 Macro has been consolidated for readability reasons. */
		pxNewTCB->ucStaticallyAllocated = tskDYNAMICALLY_ALLOCATED_STACK_AND_TCB;
#endif
		prvInitialiseNewTask(pxTaskCode, pcName, (uint32_t) usStackDepth, pvParameters, uxPriority,
			pxCreatedTask, pxNewTCB, NULL);
		prvAddNewTaskToReadyList( pxNewTCB );
		xReturn = pdPASS;
	}
	else xReturn = errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY;
	return xReturn;
}
void* pvPortMalloc(size_t xWantedSize)
{
	void* pvReturn = NULL;
	static uint8_t* pucAlignedHeap = ucHeap;
	if(xWantedSize & portBYTE_ALIGNMENT_MASK)
	{
		if((xWantedSize + (portBYTE_ALIGNMENT - (xWantedSize & portBYTE_ALIGNMENT_MASK))) > xWantedSize)
			xWantedSize += ( portBYTE_ALIGNMENT - ( xWantedSize & portBYTE_ALIGNMENT_MASK ) );
		else xWantedSize = 0;
	}
	vTaskSuspendAll();
	if((xWantedSize > 0) &&
		((xNextFreeByte + xWantedSize ) < configTOTAL_HEAP_SIZE) &&
		((xNextFreeByte + xWantedSize ) > xNextFreeByte))/* Check for overflow. */
	{
		pvReturn = pucAlignedHeap + xNextFreeByte;
		xNextFreeByte += xWantedSize;
	}
	traceMALLOC(pvReturn, xWantedSize);
	(void)xTaskResumeAll();
#if ( configUSE_MALLOC_FAILED_HOOK == 1 )
	if( pvReturn == NULL )
	{
		extern void vApplicationMallocFailedHook( void );
		vApplicationMallocFailedHook();
	}
#endif
	return pvReturn;
}
void vPortFree(void* pv)
{
	extern void print_port_free(void*);
	(void)pv;
	print_port_free(pv);
}
