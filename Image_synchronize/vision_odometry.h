#ifndef VISIONODOMETRY_H
#define VISIONODOMETRY_H
#include "odometry_base.h"
#include "safe_queue.h"
#include "stereo_image_pair.h"
#include <opencv2/calib3d.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

class VisionOdometry : public OdometryBase {
public:
  VisionOdometry() {
    detector = cv::ORB::create();
    image_buffer_.resize(max_image_sync_buffer_size);
    buffer_position_ = 0;
  };
  ~VisionOdometry(){};

  // All sensor input.
  virtual void SetImuInput(const ImuConstPtr &imu){};
  virtual void SetOdometryInput(const OdomConstPtr &odom){};

  // camera_index start by 1.
  virtual void SetCameraImageInput(ImageConstPtr image,
                                          const int camera_index);

  virtual void
  SetMultiLidarsInput(const sensor_msgs::PointCloud2ConstPtr &lidar_points){};

  virtual Odometry GetOdometryLocalPose(){};

  inline void Shutdown() { imageData.Shutdown(); }

  /**
  * @brief This function will cycle until Shutdown()
  *
  */
  void Run();

private:
  /**
  * @brief Find the target in circular Queue "image_buffer_"
  *
  * @param in_timestamp use timestamp to find the nearby target
  * @param out_position output the position
  * @return ture if find.
  */
  bool FindImageByTime(const double in_timestamp, int *out_position);

private:
  ConcurrentSafeQueue<ImageConstPtr> imageData;
  cv::Ptr<cv::Feature2D> detector;
  
  typedef TimeSyncPair<ImageConstPtr> ImagePair;
  // synchronizer
  std::mutex image_synchronizer_mutex_;
  std::vector<std::shared_ptr<ImagePair>> image_buffer_;
  int buffer_position_;
  static const int max_image_sync_buffer_size = 5;
  // camera is 25Hz, interval is 0.04
  static constexpr double time_tol = 0.005;
};

#endif // VISIONODOMETRY_H
