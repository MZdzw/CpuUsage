#include "unity.h"
#include "queue.h"

void setUp (void) {} /* Is run before every test, put unit init calls here. */
void tearDown (void) {} /* Is run after every test, put unit clean-up calls here. */

extern struct Queue cpuStatsQueue;

void test_Queue(void)
{
    //test if it's possible to dequeue empty queue
    CpuUsageStats* testCpuUsageStats = dequeue();
    TEST_ASSERT(testCpuUsageStats == NULL); 

    //test different operations on queue
    CpuUsageStats testCpuUsageStatsOp;
    int queueOverflow;
    for(int i = 0; i < MAX_CAPACITY; i++)
    {
        queueOverflow = push(testCpuUsageStatsOp);
    }
    TEST_ASSERT(queueOverflow == OK);
    queueOverflow = push(testCpuUsageStatsOp);
    TEST_ASSERT(queueOverflow == FULL_ERROR);
    TEST_ASSERT(cpuStatsQueue.size == MAX_CAPACITY);
    //dequeue all elements
    for(int i = 0; i < MAX_CAPACITY - 1; i++)
    {
        testCpuUsageStats = dequeue();
        TEST_ASSERT(testCpuUsageStats != NULL);
    }
    TEST_ASSERT(cpuStatsQueue.front == MAX_CAPACITY - 1);   //front should be at last index
    TEST_ASSERT(cpuStatsQueue.rear == 0);    //rear should be at first index
    queueOverflow = push(testCpuUsageStatsOp);
    TEST_ASSERT(queueOverflow == OK);
    TEST_ASSERT(cpuStatsQueue.rear == 1);
    TEST_ASSERT(cpuStatsQueue.size == 2);
    testCpuUsageStats = dequeue();
    testCpuUsageStats = dequeue();
    TEST_ASSERT(testCpuUsageStats != NULL);
    testCpuUsageStats = dequeue();
    TEST_ASSERT(testCpuUsageStats == NULL);
    TEST_ASSERT(cpuStatsQueue.front == 1);
    TEST_ASSERT(cpuStatsQueue.rear == 1);
    for(int i = 0; i < MAX_CAPACITY + 1; i++)
    {
        queueOverflow = push(testCpuUsageStatsOp);
    }
    TEST_ASSERT(queueOverflow == FULL_ERROR);
}


int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_Queue);
    return UNITY_END();
}