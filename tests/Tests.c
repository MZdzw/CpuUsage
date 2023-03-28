#include "unity.h"
#include "queue.h"
#include "readerThr.h"

void setUp (void) {} /* Is run before every test, put unit init calls here. */
void tearDown (void) {} /* Is run after every test, put unit clean-up calls here. */

extern struct QueueCpuStats cpuStatsQueue;

extern unsigned int threadsNum;

pthread_mutex_t queueCpuStatsMutex;
pthread_mutex_t queueCpuStatsPrinterMutex;

pthread_cond_t condCpuStatsQueue;
pthread_cond_t condCpuStatsPrinterQueue;

pthread_barrier_t barrier;

void test_Queue(void)
{
    //test if it's possible to dequeue empty queue
    CpuUsageStats* testCpuUsageStats = dequeue_CpuStats();
    TEST_ASSERT(testCpuUsageStats == NULL); 

    //test different operations on queue
    CpuUsageStats testCpuUsageStatsOp;
    int queueOverflow;
    for(int i = 0; i < MAX_CAPACITY; i++)
    {
        queueOverflow = push_CpuStats(testCpuUsageStatsOp);
    }
    TEST_ASSERT(queueOverflow == OK);
    queueOverflow = push_CpuStats(testCpuUsageStatsOp);
    TEST_ASSERT(queueOverflow == FULL_ERROR);
    TEST_ASSERT(cpuStatsQueue.size == MAX_CAPACITY);
    //dequeue all elements
    for(int i = 0; i < MAX_CAPACITY - 1; i++)
    {
        testCpuUsageStats = dequeue_CpuStats();
        TEST_ASSERT(testCpuUsageStats != NULL);
    }
    TEST_ASSERT(cpuStatsQueue.front == MAX_CAPACITY - 1);   //front should be at last index
    TEST_ASSERT(cpuStatsQueue.rear == 0);    //rear should be at first index
    queueOverflow = push_CpuStats(testCpuUsageStatsOp);
    TEST_ASSERT(queueOverflow == OK);
    TEST_ASSERT(cpuStatsQueue.rear == 1);
    TEST_ASSERT(cpuStatsQueue.size == 2);
    testCpuUsageStats = dequeue_CpuStats();
    testCpuUsageStats = dequeue_CpuStats();
    TEST_ASSERT(testCpuUsageStats != NULL);
    testCpuUsageStats = dequeue_CpuStats();
    TEST_ASSERT(testCpuUsageStats == NULL);
    TEST_ASSERT(cpuStatsQueue.front == 1);
    TEST_ASSERT(cpuStatsQueue.rear == 1);
    for(int i = 0; i < MAX_CAPACITY + 1; i++)
    {
        queueOverflow = push_CpuStats(testCpuUsageStatsOp);
    }
    TEST_ASSERT(queueOverflow == FULL_ERROR);
}

void test_Num_Threads(void)
{
    getCpuThreadsNum("./tests/testFiles/testFileCorrect.txt");
    TEST_ASSERT(threadsNum == 12);
    
    TEST_ASSERT(getCpuThreadsNum("./tests/testFiles/testFileCpuError.txt") == READ_NO_CPU_THR);

    TEST_ASSERT(getCpuThreadsNum("./tests/testFiles/noExistingFile.txt") == FILE_OPEN_ERROR);

    TEST_ASSERT(getCpuStatsFromFile("./tests/testFiles/testFileCpuTooMuchInfos.txt") == INCORRECT_NUM_CPU_USAGE);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_Queue);
    RUN_TEST(test_Num_Threads);
    return UNITY_END();
}