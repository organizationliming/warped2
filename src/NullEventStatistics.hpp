#ifndef NULL_EVENT_STATISTICS_HPP
#define NULL_EVENT_STATISTICS_HPP

#include <iosfwd>
#include <string>

#include "Event.hpp"
#include "EventStatistics.hpp"
#include "utility/warnings.hpp"

namespace warped {

// An EventStatistics class that performs no actions
class NullEventStatistics : public EventStatistics {
public:
    NullEventStatistics() : EventStatistics("") {}

    void record(const std::string& source, unsigned int send_time, Event* event) {
        unused(source, send_time, event);
    }
    void record(const std::string& source, unsigned int send_time,
                             const std::vector<std::shared_ptr<Event>>& events) {
    for (const auto& e : events) {
        record(source, send_time, e.get());
    }
}


    void writeToFile() {}
private:
    std::ostream& printStats(std::ostream& stream) const { return stream; };
};

} // namespace warped

#endif
