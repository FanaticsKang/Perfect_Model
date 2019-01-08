# How to use

## `time_sync_pair.h`
`class TimeSyncPair`用于同步不同的数据.

## vision_odometry
这是实现的案例, 需要进行以下几个地方的处理

### 声明
1. 线程锁`std::mutex image_synchronizer_mutex_;`
2. 需要一个循环数组, 例子里面使用
```
typedef TimeSyncPair<ImageConstPtr> ImagePair;
std::vector<std::shared_ptr<ImagePair>> image_buffer_;
int buffer_position_;
```
3. 需要在初始化的时候设定buffer的大小
4. 需要设置可以容忍的时间差.

### 实现
1. `FindImageByTime()`, 通过时间索引buffer
2. `SetCameraImageInput` 参考这个函数的算法, 最终通过`image_buffer_[position]->IsPaired()`同步线程.
