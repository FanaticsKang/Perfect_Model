#include "frame_synchronizer.h"

static const int max_frame_sync_buffer_size = 5;
FrameSynchronizer::FrameSynchronizer(const double in_time_tol)
    : buffer_position_(0), time_tol_(in_time_tol) {
  frame_buffer_.resize(max_frame_sync_buffer_size);
}

void FrameSynchronizer::AddNewFrame(
    ImageConstPtr frame_ptr, const int frame_index,
    std::shared_ptr<MultiFrame> *multiFrame_ptr) {
  double frame_stamp = frame_ptr->header.timestamp;
  int position;
  if (FindFrameByTime(frame_stamp, &position)) {
    // When find insert image
    frame_buffer_[position]->setData(frame_stamp, frame_ptr,
                                     frame_index);
  } else {
    // not find, then create new member, then insert
    buffer_position_ = (buffer_position_ + 1) % max_frame_sync_buffer_size;
    position = buffer_position_;
    // TODO impove to multi frame synchronize.
    frame_buffer_[position] = std::shared_ptr<MultiFrame>(new MultiFrame(2));
    frame_buffer_[position]->setData(frame_stamp, frame_ptr,
                                     frame_index);
  }
  *multiFrame_ptr = frame_buffer_[position];
}

bool FrameSynchronizer::FindFrameByTime(const double &timestamp,
                                        int *position) const {
  int p;
  for (int i = 0; i < max_frame_sync_buffer_size; i++) {
    p = (buffer_position_ + i) % max_frame_sync_buffer_size;
    if (frame_buffer_[p] == nullptr) {
      continue;
    }
    double image_timestamp = frame_buffer_[p]->timestamp();
    if (image_timestamp == timestamp ||
        fabs(image_timestamp - timestamp) < time_tol_) {
      *position = p;
      return true;
    }
  }
  return false;
}

