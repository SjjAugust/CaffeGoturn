#include "loader_visdrone_sot.h"

#include <opencv/cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "helper/helper.h"

using std::string;
using std::vector;
namespace bfs = boost::filesystem;

// If true, only load a small number of videos.
const bool kDoTest = false;

// What fraction of videos should be included in the validation set (the
// remainder are part of the training set).
// Note - after using the validation set to choose hyperparameters, set val_ratio to 0
// to train the final model on the training set + validation set (not the test set!)
const double val_ratio = 0.2;

LoaderVisdroneSot::LoaderVisdroneSot(const string& video_folder, const string& annotations_folder)
{
  if (!bfs::is_directory(annotations_folder)) {
    printf("Error - %s is not a valid directory!\n", annotations_folder.c_str());
    return;
  }

  // Find all video subcategories.
  // find all video annotations_files
  const boost::regex annotation_filter(".*\\.txt");
  vector<string> annotation_files;
  find_matching_files(annotations_folder, annotation_filter, &annotation_files);

  const int max_vid_num = kDoTest ? 3 : annotation_files.size();

  printf("Found %d annotations...\n", max_vid_num);
  Category category;
  // for all annotations
  for (size_t i = 0; i < max_vid_num; ++i) {
    
    const string& anno_name = annotation_files[i];
    printf("Processing annotation file: %s\n", anno_name.c_str());

    // Read annotations for a given video.
    Video video;

    // Get the path to the video image files.
    const string video_path = video_folder + "/" + 
        anno_name.substr(0, anno_name.length() - 4);
    video.path = video_path;

    // Add all image files
    const boost::regex image_filter(".*\\.jpg");
    find_matching_files(video_path, image_filter, &video.all_frames);

    // Open the annotation file.
    const string& annotation_file_path = annotations_folder + "/" + anno_name;
    FILE* annotation_file_ptr = fopen(annotation_file_path.c_str(), "r");
    int xmin, ymin, w, h;
    int frame_num = 0;

    while (true) {
      // Read a line from the annotation file.
      const int status = fscanf(annotation_file_ptr, "%d,%d,%d,%d\n",
                    &xmin, &ymin, &w, &h);
      if (status == EOF) {
        break;
      }

      // Convert the annotation data into frame and bounding box format.
      Frame frame;
      BoundingBox& bbox = frame.bbox;
      frame.frame_num = frame_num;
      bbox.x1_ = static_cast<double>(xmin);
      bbox.y1_ = static_cast<double>(ymin);
      bbox.x2_ = static_cast<double>(xmin + w);
      bbox.y2_ = static_cast<double>(ymin + h);

      // Save the annotation data.
      video.annotations.push_back(frame);
      ++frame_num;
    } // Process annotation file

    // Save the video.
    fclose(annotation_file_ptr);
    videos_.push_back(video);
    category.videos.push_back(video);

  } // Process all categories
  // Save the video category.
  categories_.push_back(category);
}

void LoaderVisdroneSot::get_videos(const bool get_train, std::vector<Video>* videos) const {
  // categories_.size() is always 1
  for (size_t category_num = 0; category_num < categories_.size(); ++category_num) {
    const Category& category = categories_[category_num];

    // Number of videos in this category.
    size_t num_videos = category.videos.size();

    // Number of videos from this category to use in the validation set (the rest go into the training set).
    const int num_val = static_cast<int>(val_ratio * num_videos);

    // Number of videos from this category to use in the training set.
    const int num_train = num_videos - num_val;

    int start_num;
    int end_num;
    if (get_train) {
      // Get the start and end video num for training.
      start_num = 0;
      end_num = num_train - 1;
    } else {
      // Get the start and end video num for validation.
      start_num = num_train;
      end_num = num_videos - 1;
    }

    // Add the appropriate videos from this category to the list of videos
    // to return.
    const std::vector<Video>& category_videos = category.videos;
    for (size_t i = start_num; i <= end_num; ++i) {
      const Video& video = category_videos[i];
      videos->push_back(video);
    }
  }

  // Count the total number of labeled frames.
  size_t num_annotations = 0;
  for (size_t i = 0; i < videos->size(); ++i) {
    const Video& video = (*videos)[i];
    num_annotations += video.annotations.size();
  }
  printf("Total annotated video frames: %zu\n", num_annotations);
}

