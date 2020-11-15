#ifndef MPI_COMMUNICATION_MANAGER_HPP
#define MPI_COMMUNICATION_MANAGER_HPP

#include <mpi.h>
#include <vector>
#include <cstdint>
#include <mutex>
#include <shared_mutex>

#include "TimeWarpCommunicationManager.hpp"
#include "TimeWarpKernelMessage.hpp"

namespace warped {

struct MessageQueue;
struct MPISendQueue;
struct MPIRecvQueue;

#define MPI_DATA_TAG        729

class TimeWarpMPICommunicationManager : public TimeWarpCommunicationManager {
public:
    TimeWarpMPICommunicationManager(unsigned int max_buffer_size, unsigned max_aggregate) :
        max_buffer_size_(max_buffer_size*max_aggregate), max_aggregate_(max_aggregate) {}

    virtual ~TimeWarpMPICommunicationManager() = default;
    unsigned int initialize();
    void finalize();
    unsigned int getNumProcesses();
    unsigned int getID();
    int waitForAllProcesses();

    void insertMessage(std::unique_ptr<TimeWarpKernelMessage> msg);
    void handleMessages();
    void flushMessages();

    int sumReduceUint64(uint64_t* send_local, uint64_t* recv_global);
    int gatherUint64(uint64_t* send_local, uint64_t* recv_root);
    int sumAllReduceInt64(int64_t* send_local, int64_t* recv_global);
    int minAllReduceUint(unsigned int* send_local, unsigned int* recv_global);

    bool barrierHoldStatus();
    void barrierPause();
    void barrierResume();

    bool getTokenSendConfirmation();
    void setTokenSendConfirmation(bool input);

protected:
    void packAndSend(unsigned int receiver_id);

    unsigned int startSendRequests();
    unsigned int startReceiveRequests();
    unsigned int testSendRequests();
    unsigned int testReceiveRequests();

    bool isInitiatingThread();

private:
    unsigned int max_buffer_size_;
    unsigned int max_aggregate_;

    int num_processes_;
    int my_rank_;

    std::unique_ptr<std::list<std::unique_ptr<TimeWarpKernelMessage>>[]> aggregate_messages_;
    std::unique_ptr<unsigned int[]> aggregate_message_count_by_receiver_ = 0;

    std::shared_ptr<MessageQueue> send_queue_;
    std::shared_ptr<MessageQueue> recv_queue_;

    bool barrier_hold_;
    std::shared_mutex barrier_hold_lock_;

    std::mutex gvt_token_send_confirmation_lock_;
    bool gvt_token_send_confirmation_;
};

struct PendingRequest {
    PendingRequest(std::unique_ptr<uint8_t[]> buffer, unsigned int count) :
        buffer_(std::move(buffer)), count_(count) {}

    std::unique_ptr<uint8_t[]> buffer_;
    MPI_Request request_;
    int flag_;
    MPI_Status status_;
    int count_;
};

struct MessageQueue {
    MessageQueue(unsigned int max_buffer_size) :
        max_buffer_size_(max_buffer_size) {}

    unsigned int max_buffer_size_;

    std::deque<std::unique_ptr<TimeWarpKernelMessage>>  msg_list_;
    std::mutex msg_list_lock_;

    std::vector<std::unique_ptr<PendingRequest>> pending_request_list_;
};

} // namespace warped

#endif

