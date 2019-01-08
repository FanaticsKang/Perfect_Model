/**
 * @file time_sync_pair.h
 * @brief Header file for the ThreadsafeQueue class.
 * @author Yifei Kang
 */

#ifndef TIME_SYNC_PAIR_H
#define TIME_SYNC_PAIR_H
#include <utility>
#include <vector>

template <class Type> class TimeSyncPair {
public:
  TimeSyncPair(const int buffer_size, const double in_time_tol = 0.02)
      : timestamp_(-1), time_tol_(in_time_tol) {
    data_with_timestamp_.resize(buffer_size);
  };
  ~TimeSyncPair(){};

  /**
  * @brief add Data in Pair
  *
  * @param in_timestamp  timestamp
  * @param in_data inputdata
  * @param data_index data_index, related to camera_index, start at 0.
  * @return true if success.
  */
  bool AddNewFrame(double in_timestamp, Type in_data, const int data_index) {
    if (data_index >= data_with_timestamp_.size()) {
      std::cerr << "Data index oversteped." << std::endl;
      return false;
    }
    // camera is 25Hz, interval is 0.04, default time_tol_ = 0.02
    if (timestamp_ > 0 && fabs(timestamp_ - in_timestamp) > time_tol_) {
      std::cerr << "Time span is too long." << std::to_string(in_timestamp)
                << "(" << data_index << ") : " << std::to_string(timestamp_)
                << std::endl;
      return false;
    }
    if (timestamp_ <= 0) {
      timestamp_ = in_timestamp;
    }
    data_with_timestamp_[data_index].first = in_timestamp;
    data_with_timestamp_[data_index].second = &in_data;
    return true;
  };

  /**
  * @brief All data has element, then return true.
  */
  bool IsPaired() {
    for (auto iter = data_with_timestamp_.begin(); iter != data_with_timestamp_.end(); iter++) {
      if (iter->second == nullptr) {
        return false;
      }
    }
    return true;
  }

  double timestamp() { return timestamp_; }

  // test
  void ShowAllTime() {
    int i = 0;
    std::cerr << "Pair Data: ";
    for (auto &one_data:data_with_timestamp_) {
      std::cerr << "[" << i  << "]: " <<
                  std::to_string(one_data.first) << "; ";
      ++i;
    }
    std::cerr << std::endl << std::endl;
  }

private:
  std::vector<std::pair<double, Type*>> data_with_timestamp_; //need to sync
  double timestamp_; // public timestamp
  const double time_tol_; //Tolerable time intervals
};

#endif // TIME_SYNC_PAIR_H
