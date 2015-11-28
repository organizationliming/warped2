#ifndef SYNCHRONOUS_GVT_MANAGER_HPP
#define SYNCHRONOUS_GVT_MANAGER_HPP

#include <memory> // for shared_ptr
#include <atomic>

#include <pthread.h>

#include "TimeWarpEventDispatcher.hpp"
#include "TimeWarpGVTManager.hpp"

namespace warped {

class TimeWarpSynchronousGVTManager : public TimeWarpGVTManager {
public:
    TimeWarpSynchronousGVTManager(std::shared_ptr<TimeWarpCommunicationManager> comm_manager,
        unsigned int period, unsigned int num_worker_threads)
        : TimeWarpGVTManager(comm_manager, period, num_worker_threads) {}

    void initialize() override;

    bool readyToStart();

    void progressGVT();

    void receiveEventUpdate(std::shared_ptr<Event>& event, Color color);

    Color sendEventUpdate(std::shared_ptr<Event>& event);

    bool gvtUpdated();

    void reportThreadMin(unsigned int timestamp, unsigned int thread_id,
                                 unsigned int local_gvt_flag);

    void reportThreadSendMin(unsigned int timestamp, unsigned int thread_id);

    unsigned int getLocalGVTFlag();

protected:
    bool gvt_updated_ = false;

    std::atomic<int64_t> msg_count_ = ATOMIC_VAR_INIT(0);

    std::atomic<unsigned int> local_gvt_flag_ = ATOMIC_VAR_INIT(0);

    std::unique_ptr<unsigned int []> local_min_;

    std::unique_ptr<unsigned int []> send_min_;

    pthread_barrier_t min_report_barrier_;

};

} // warped namespace

#endif