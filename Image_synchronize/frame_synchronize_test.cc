#include <gtest/gtest.h>
#include <unistd.h>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>
#include "frame_synchronizer.h"

// To protect FrameSynchronizer
std::mutex frameSynchronizer_mutex_;
// This is the data to synchronize two frame.
FrameSynchronizer frame_synchronizer;
// The size of iteration times
size_t iter_num = 6;
// When frame is synchronized, save all data here
std::vector<std::shared_ptr<MultiFrame>> my_all_data;

int data_index = 0;
int contain_index = 0;

void SendImageData(int camera_index) {
  int origin_time = 0;
  for (size_t i = 0; i < iter_num; ++i) {
    std::shared_ptr<MultiFrame> multi_frame_ptr;
    {
      std::lock_guard<std::mutex> lock(frameSynchronizer_mutex_);
      std::shared_ptr<jdx::localization::Image> tmp_image_ptr(
          new Image(++origin_time));
      frame_synchronizer.AddNewFrame(tmp_image_ptr, camera_index - 1,
                                     &multi_frame_ptr);
      contain_index++;

      // Is here need to be protected by frameSynchronizer_mutex_?
      if (multi_frame_ptr != nullptr && multi_frame_ptr->IsPaired()) {
        auto image_l = (multi_frame_ptr->getData(0));
        auto image_r = (multi_frame_ptr->getData(1));
        my_all_data[data_index++] = multi_frame_ptr;
      }
    }
    usleep(100);
  }
}

TEST(FrameSynchronizer_test, test_frame_sync) {
  my_all_data.resize(iter_num);
  // Create two thread, each send a image.
  std::thread t1(SendImageData, 1);
  std::thread t2(SendImageData, 2);
  std::cout << "Create all Thread" << std::endl;
  t1.join();
  t2.join();
  std::cout << "Thread Finished" << std::endl;

  // All Thread finished, print synchronize result.
  for (auto iter = my_all_data.begin(); iter != my_all_data.end(); iter++) {
    std::shared_ptr<MultiFrame> multi_frame_ptr = (*iter);
    if (multi_frame_ptr->IsPaired()) {
      multi_frame_ptr->ShowAllTime();
    }
    ImageConstPtr image_l = multi_frame_ptr->getData(0);
    ImageConstPtr image_r = multi_frame_ptr->getData(1);
    EXPECT_EQ(image_l->header.timestamp, image_r->header.timestamp);
  }
}

int main(int argc, char *argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
