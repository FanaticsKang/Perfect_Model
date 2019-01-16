/**
 * @file frame_synchronizer.h
 * @brief Header file for the frame Synchronizer class.
 * @author Yifei Kang, Peng Zhang
 */

#ifndef FRAME_SYNCHRONIZER_H
#define FRAME_SYNCHRONIZER_H

#include "localization_common.h"
#include "time_sync_pair.h"

typedef TimeSyncPair<Image> MultiFrame;
/**
 * @brief FrameSynchronizer stereo frame.
 * @brief The buffer size is defined by max_frame_sync_buffer_size.
 * @TODO need to change to multiple frame.
 *
 */
class FrameSynchronizer {
 public:
  explicit FrameSynchronizer(const double in_time_tol = 0.003);
  ~FrameSynchronizer(){};

  /**
   * @brief Add a new frame with image index,
   * @brief and it will return a pointer of MultiFrame in it buffer.
   * @warning It is not a thread safe.
   *
   * @param[in]  frame Image data.
   * @param[in]  frame_index
   * @param[out] multiFrame_ptr return a pointer copy of MultiFrame in it
   * buffer.
   * @return True if ok. False if input frame_ptr is nullptr.
   */
  bool AddNewFrame(ImageConstPtr frame_ptr, const int &frame_index,
                   std::shared_ptr<MultiFrame> *multiFrame_ptr);

 private:
  /**
   * @brief Use time to find data in buffer
   * @param[in]  timestamp
   * @param[out] out the position of found
   *
   * @return if find, return true.
   */
  bool FindFrameByTime(const double &timestamp, int *position) const;

  /// @brief Circular array for data buffer
  std::vector<std::shared_ptr<MultiFrame>> frame_buffer_;
  /// @brief Circular array index.
  int buffer_position_;
  /// @brief Time tolerant of two frame here.
  double time_tol_;
};

#endif  // FRAME_SYNCHRONIZER_H
