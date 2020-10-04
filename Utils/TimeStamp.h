//
// Created by jarvist on 3/31/20.
//

#ifndef WEBSERVER_TIMESTAMP_H
#define WEBSERVER_TIMESTAMP_H


#include "../Base/Copyable.h"
#include <string>

class TimeStamp : Copyable{
private:
    int64_t microSecondsSinceEpoch_;

public:
    TimeStamp(): microSecondsSinceEpoch_(0){};
    explicit TimeStamp(int64_t microSecondsSinceEpochArg): microSecondsSinceEpoch_(microSecondsSinceEpochArg){};
    static TimeStamp now();
    static const int kMicroSecondsPerSecond = 1000 * 1000;
    int64_t microSecondsSinceEpoch() const { return microSecondsSinceEpoch_; }
    static TimeStamp invalid()
    {
        return {};
    }

    std::string toString() const;
    std::string toFormattedString(bool showMicroseconds) const;
    bool valid()const {return microSecondsSinceEpoch_ > 0;};
    void swap(TimeStamp &that)
    {
        std::swap(microSecondsSinceEpoch_, that.microSecondsSinceEpoch_);
    }


};

inline bool operator<(TimeStamp lhs, TimeStamp rhs)
{
    return lhs.microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch();
}

inline bool operator==(TimeStamp lhs, TimeStamp rhs)
{
    return lhs.microSecondsSinceEpoch() == rhs.microSecondsSinceEpoch();
}

inline double timeDifference(TimeStamp high, TimeStamp low)
{
    int64_t diff = high.microSecondsSinceEpoch() - low.microSecondsSinceEpoch();
    return static_cast<double>(diff) / TimeStamp::kMicroSecondsPerSecond;
};

inline TimeStamp addTime(TimeStamp timestamp, double seconds)
{
    int64_t delta = static_cast<int64_t>(seconds * TimeStamp::kMicroSecondsPerSecond);
    return TimeStamp(timestamp.microSecondsSinceEpoch() + delta);
}


#endif //WEBSERVER_TIMESTAMP_H
