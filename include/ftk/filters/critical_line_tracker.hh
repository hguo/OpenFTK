#ifndef _FTK_CRITICAL_LINE_TRACKER_HH
#define _FTK_CRITICAL_LINE_TRACKER_HH

#include <ftk/config.hh>
#include <ftk/algorithms/cca.hh>
#include <ftk/filters/tracker.hh>
#include <ftk/features/feature_point.hh>
#include <ftk/features/feature_surface.hh>
#include <ftk/features/feature_volume.hh>
#include <ftk/geometry/points2vtk.hh>
#include <ftk/geometry/cc2curves.hh>
#include <ftk/utils/gather.hh>
#include <iomanip>

namespace ftk {

// we define critical lines as intersections of two isosurfaces in 3D
struct critical_line_tracker : public virtual tracker {
  critical_line_tracker(diy::mpi::communicator comm) : tracker(comm) {}

  int cpdims() const { return 3; }
  
  virtual void update() {}; 
  void reset() {
    field_data_snapshots.clear();
  }
  
public:
  bool advance_timestep();

public:
  bool pop_field_data_snapshot();
  void push_field_data_snapshot(const ndarray<float> &uv);
  
protected:
  struct field_data_snapshot_t {
    ndarray<float> uv; // shape is (2, width, height, depth)
  };
  std::deque<field_data_snapshot_t> field_data_snapshots;
};

/////
inline bool critical_line_tracker::advance_timestep()
{
  update_timestep();
  pop_field_data_snapshot();

  current_timestep ++;
  return field_data_snapshots.size() > 0;
}

inline bool critical_line_tracker::pop_field_data_snapshot()
{
  if (field_data_snapshots.size() > 0) {
    field_data_snapshots.pop_front();
    return true;
  } else return false;
}
  
inline void critical_line_tracker::push_field_data_snapshot(const ndarray<float> &uv)
{
  field_data_snapshot_t snapshot; 
  snapshot.uv = uv;
  field_data_snapshots.emplace_back(snapshot);
}

}

#endif