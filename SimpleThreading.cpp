#include <iostream>
#include <thread>
#include <vector>

using namespace std;

constexpr size_t VecSize = 8192;
const static size_t NumOfThreads = thread::hardware_concurrency();
vector<vector<int>> doubleVec(VecSize, vector<int>(VecSize));

template <typename F>
chrono::duration<float> MeasureRuntime(F&& inFunc)
{
    chrono::time_point<chrono::steady_clock> threadStart = chrono::steady_clock::now();
    inFunc();
    chrono::time_point<chrono::steady_clock> threadEnd = chrono::steady_clock::now();
    chrono::duration<float> threadTime = threadEnd - threadStart;
    return threadTime;
}

void Count(size_t startRow, size_t endRow, size_t& outCounter)
{
    outCounter = 0;
    for (size_t i = startRow; i < endRow; i++)
    {
        for (size_t j = 0; j < VecSize; j++)
        {
            outCounter = (doubleVec[i][j] < 64) ? outCounter + 1 : outCounter;
        }
    }
}
int main(int argc, char* argv[])
{
    srand(static_cast<unsigned int>(time(0)));
    
    for (size_t i = 0; i < VecSize; i++)
    {
        for (size_t j = 0; j < VecSize; j++)
        {
            doubleVec[i][j] = rand() % 256;
        }
    }
    
    vector<thread> vecThreads;
    vector<size_t> counters(NumOfThreads, 0);
     
    for (size_t i = 0; i < NumOfThreads; i++)
    {
        size_t startRow = (VecSize / NumOfThreads) * i;
        size_t endRow = (VecSize / NumOfThreads) * (i + 1);
        vecThreads.emplace_back(thread(Count, startRow, endRow, ref(counters[i])));
    }
    
    size_t totalCount1 = 0;
    auto executeAllThreads = [&vecThreads, &totalCount1, &counters]() -> void
    {
        for (size_t i = 0; i < vecThreads.size(); i++)
        {
            // block main thread until currentThread is done executing
            vecThreads[i].join();
            totalCount1 += counters[i];
        }
    };

    size_t totalCount2 = 0;
    auto executeWithoutThreads = [&counters, &totalCount2]() -> void
    {
        for (size_t i = 0; i < counters.size(); i++)
        {
            Count(0, VecSize, counters[i]);
            totalCount2 += counters[i];
        }
    };
    
    chrono::duration<float> threadTime = MeasureRuntime(executeAllThreads);
    cout << "with threads: " << threadTime.count() << "\n";
    cout << "with threads :: cells lesser than 64: " << totalCount1 << "\n";
    
    chrono::duration<float> NoThreadTime = MeasureRuntime(executeWithoutThreads);
    cout << "no threads: " << NoThreadTime.count() << "\n";
    cout << "no threads :: cells lesser than 64: " << totalCount2 << "\n";

    return 0;
}