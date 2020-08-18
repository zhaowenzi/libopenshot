/**
 * @file
 * @brief Track an object selected by the user
 * @author Jonathan Thomas <jonathan@openshot.org>
 *
 * @ref License
 */

/* LICENSE
 *
 * Copyright (c) 2008-2019 OpenShot Studios, LLC
 * <http://www.openshotstudios.com/>. This file is part of
 * OpenShot Library (libopenshot), an open-source project dedicated to
 * delivering high quality video editing and animation solutions to the
 * world. For more information visit <http://www.openshot.org/>.
 *
 * OpenShot Library (libopenshot) is free software: you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * OpenShot Library (libopenshot) is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with OpenShot Library. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef OPENSHOT_CVTRACKER_H
#define OPENSHOT_CVTRACKER_H

#include <google/protobuf/util/time_util.h>

#define int64 opencv_broken_int
#define uint64 opencv_broken_uint
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/core.hpp>
#undef uint64
#undef int64

#include <fstream>
#include "Clip.h"
#include "KeyFrame.h"
#include "Frame.h"
#include "Json.h"
#include "ProcessingController.h"
#include "trackerdata.pb.h"

#include "../src/sort_filter/sort.hpp"

using namespace std;
using google::protobuf::util::TimeUtil;

// Tracking info struct
struct FrameData{
  size_t frame_id = -1;
  float rotation = 0;
  float x1 = -1;
  float y1 = -1;
  float x2 = -1;
  float y2 = -1;

  // Constructors
  FrameData()
  {}

  FrameData( size_t _frame_id)
  {frame_id = _frame_id;}

  FrameData( size_t _frame_id , float _rotation, float _x1, float _y1, float _x2, float _y2)
  {
      frame_id = _frame_id;
      rotation = _rotation;
      x1 = _x1;
      y1 = _y1;
      x2 = _x2;
      y2 = _y2;
  }
};

class CVTracker { 
  private:
    std::map<size_t, FrameData> trackedDataById; // Save tracked data       
    std::string trackerType; // Name of the chosen tracker
    cv::Ptr<cv::Tracker> tracker; // Pointer of the selected tracker

    cv::Rect2d bbox; // Bounding box coords 
    SortTracker sort;

    std::string protobuf_data_path; // Path to protobuf data file

    uint progress; // Pre-processing effect progress

    /// Will handle a Thread safely comutication between ClipProcessingJobs and the processing effect classes
    ProcessingController *processingController;

    bool json_interval;
    size_t start;
    size_t end;
  

    // Initialize the tracker
    bool initTracker(cv::Mat &frame, size_t frameId);
    
    // Update the object tracker according to frame 
    bool trackFrame(cv::Mat &frame, size_t frameId);

  public:

    // Constructor
    CVTracker(std::string processInfoJson, ProcessingController &processingController);
    
    // Set desirable tracker method
    cv::Ptr<cv::Tracker> selectTracker(std::string trackerType);

    // Track object in the hole clip or in a given interval
    // If start, end and process_interval are passed as argument, clip will be processed in [start,end) 
    void trackClip(openshot::Clip& video, size_t _start=0, size_t _end=0, bool process_interval=false);

    // Get tracked data for a given frame
    FrameData GetTrackedData(size_t frameId);
    
    /// Protobuf Save and Load methods
    // Save protobuf file
    bool SaveTrackedData();
    // Add frame tracked data into protobuf message.
    void AddFrameDataToProto(libopenshottracker::Frame* pbFrameData, FrameData& fData);

    /// Get and Set JSON methods
    void SetJson(const std::string value); ///< Load JSON string into this object
    void SetJsonValue(const Json::Value root); ///< Load Json::Value into this object

    // Load protobuf file (ONLY FOR MAKE TEST)
    bool _LoadTrackedData();
};



#endif