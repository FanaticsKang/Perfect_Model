/**
 * @file frame_synchronizer.h
 * @brief Header file for the Time synchronization, it depends on timestamp and
 * contain lots of Type
 * @author Yifei Kang
 */

#ifndef TIME_IMAGE_PAIR_H
#define TIME_IMAGE_PAIR_H
#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>

/**
 * @brief Use to Sync serveral Type. The size depends on buffer_size.
 *
 */
template <class Type>
class TimeSyncPair {
 public:
  explicit TimeSyncPair(const int buffer_size, const double in_time_tol = 0.02)
      : timestamp_(-1), time_tol_(in_time_tol) {
    data_with_timestamp_.resize(buffer_size);
  };
  ~TimeSyncPair(){};

  /**
   * @brief add Data, this function use Type reference. Its shared_ptr is 
   *        is used in data buffer, so a copy processed. For faster, you
   *        can use its shared_ptr directly.
   *
   * @param in_timestamp timestamp
   * @param in_data inputdata
   * @param data_index image_index, related to camera_index, start at 0.
   * @return true if success.
   */
  bool setData(const double &in_timestamp, const Type &in_data, const int &data_index) {
    if(IsLegal(in_timestamp, data_index) == false){
      return false;
    }
    // Create a shared_ptr to avoid memory leak.
    std::shared_ptr<Type const> in_data_ptr(new Type(in_data));
    return Insert(in_timestamp, in_data_ptr, data_index);
  };
  
  /**
   * @brief Add data, use its shared_ptr directly.
   *
   * @param in_timestamp timestamp
   * @param in_data inputdata
   * @param data_index image_index, related to camera_index, start at 0.
   * @return true if success.
   */
  bool setData(const double &in_timestamp, std::shared_ptr<Type const> in_data_ptr, const int &data_index) {
    if(IsLegal(in_timestamp, data_index) == false){
      return false;
    }    
    return Insert(in_timestamp, in_data_ptr, data_index);
  };

  /**
   * @brief All images_ has element, then return true.
   */
  bool IsPaired() {
    return !std::any_of(data_with_timestamp_.begin(),
                        data_with_timestamp_.end(),
                        [](std::pair<double, std::shared_ptr<Type const>> item) {
                          return item.second == nullptr;
                        });
  };

  double timestamp() { return timestamp_; };

  /**
   * @brief Show timestamp of all elements.
   */
  void ShowAllTime() {
    int i = 0;
    std::cout << "Pair Data: ";
    for (auto &one_data : data_with_timestamp_) {
      std::cout << "[" << i << "]: " << std::to_string(one_data.first) << "; ";
      ++i;
    }
    std::cout << std::endl;
  };

  /**
  * @brief  Get one elemnt.
  * 
  * @param index[in] data index
  * @param out_timestamp[out] timestamp for output.
  * @return data pointer
  */
  std::shared_ptr<Type const> getData(const int index, double *out_timestamp = nullptr) {
    assert(index < data_with_timestamp_.size());
    if(out_timestamp != nullptr){
      *out_timestamp = data_with_timestamp_[index].first;
    }
    return data_with_timestamp_[index].second;
  };

 private:
   /**
   * @brief Check the timestamp and index.
   * 
   * @return true if ok.
   */
   bool IsLegal(const double &in_timestamp, const int &data_index){
    if (data_index >= data_with_timestamp_.size()) {
      std::cerr << "Data index oversteped. " << data_index << ":"
                << data_with_timestamp_.size() << std::endl;
      return false;
   }
    // camera is 25Hz, interval is 0.04, default time_tol_ = 0.02
    if (timestamp_ > 0 && fabs(timestamp_ - in_timestamp) > time_tol_) {
      std::cerr << "Time span is too long. " << std::to_string(in_timestamp)
                << "(" << data_index << ") : " << std::to_string(timestamp_)
                << std::endl;
      return false;
    }
    return true;
  };
  
  /**
  * @brief Insert a data in its position.
  *        
  * @return true whatever
  */
  bool Insert(double in_timestamp, std::shared_ptr<Type const> in_data, 
              const int data_index){
    if (timestamp_ <= 0) {
      timestamp_ = in_timestamp;
    }
    data_with_timestamp_[data_index].first = in_timestamp;
    data_with_timestamp_[data_index].second = in_data;
    return true;    
  };
  
  /**
   * @brief The size depends its size of synchronization data.
   * @brief In stereo camera, size is two.
   */
  std::vector<std::pair<double, std::shared_ptr<Type const>>> data_with_timestamp_;
  /**
   * @brief the timestamp when first setData.
   */
  double timestamp_;
  /**
   * @brief camera is 25Hz, interval is 0.04, default time_tol_ = 0.02
   */
  double time_tol_;
};

#endif  // TIME_IMAGE_PAIR_H
