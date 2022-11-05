//#include <string>
//#include <mutex>
//#include <queue>
//#include <pthread.h>
//
//
//
//
//class check_class
//{
//    std::queue<int> messageQueue;
//public:
//    check_class()
//    {
//
//    }
//
//    check_class(const check_class& th)
//    {
//
//    }
//
//    check_class(check_class&& th) noexcept
//    {
//    }
//
//    ~check_class()
//    {
//
//    }
//
//    void check(size_t child_keys_num);
//};
//
//
//static std::mutex mtx;
//void check_class::check(size_t child_keys_num)
//{
//    while (1) {
//
//        // Getting the lock on queue using mutex
//        pthread_mutex_lock(&mtx);
//
//        // Pop only when queue has at least 1 element
//        if (messageQueue.size() > 0) {
//            // Get the data from the front of queue
//            int data = Q.front();
//
//            cout << "B thread consumed: " << data << endl;
//
//            // Add the data to the integer variable
//            // associated with thread B
//            sum_B += data;
//
//            // Pop the consumed data from queue
//            Q.pop();
//
//            consumerCount1++;
//
//            pthread_cond_signal(&dataNotConsumed);
//        }
//
//
//        // Get the mutex unlocked
//        pthread_mutex_unlock(&mutex);
//    }
//    mtx.lock();
//    //output_buffer.insert(private_version_buffer.begin(), private_version_buffer.end());
//    mtx.unlock();
//}
//
//
//
//
//
//
//
//
//
//
