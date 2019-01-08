#include "cv_bridge/cv_bridge.h"
#include "image_transport/image_transport.h"
#include "vision_odometry.h"
#include <memory>

void VisionOdometry::Run() {
}

bool VisionOdometry::FindImageByTime(const double in_timestamp, int *position) {
  int p;
  for (int i = 0; i < max_image_sync_buffer_size; i++) {
    p = (buffer_position_ + i) % max_image_sync_buffer_size;
    if (image_buffer_[p] == nullptr) {
      continue;
    }
    double image_timestamp = image_buffer_[p]->timestamp();
    if (image_timestamp == in_timestamp ||
        fabs(image_timestamp - in_timestamp) < time_tol) {
      *position = p;
      return true;
    }
  }
  return false;
}

void VisionOdometry::SetCameraImageInput(ImageConstPtr image,
                                         const int camera_index) {
  assert(camera_index > 0);
  std::lock_guard<std::mutex> lock(image_synchronizer_mutex_);
  int position;
  std::cerr << camera_index << ": " << std::to_string(image->header.timestamp)
            << std::endl;
  if (FindImageByTime(image->header.timestamp, &position)) {
    // When find insert image
    std::cerr << "\033[33m"
              << "Use old data(" << camera_index << ")\033[37m" << std::endl;
    //camera_index starts at 1.
    image_buffer_[position]->AddNewFrame(image->header.timestamp, image, camera_index - 1);
  } else {
    // not find, then create new member, then insert
    std::cerr << "\033[31m"
              << "Create new data(" << camera_index << ")\033[37m" << std::endl;
    buffer_position_ = (buffer_position_ + 1) % max_image_sync_buffer_size;
    position = buffer_position_;
    image_buffer_[position] =
        std::shared_ptr<ImagePair>(new ImagePair(2));
    image_buffer_[position]->AddNewFrame(image->header.timestamp, image, camera_index - 1);
  }
  if (image_buffer_[position]->IsPaired()) {
    // if current position has two image, then paired.
    image_buffer_[position]->ShowAllTime();
    // clear the current position.
    image_buffer_[position] = nullptr;
  }
}
