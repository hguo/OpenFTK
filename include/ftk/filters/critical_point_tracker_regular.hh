#ifndef _FTK_CRITICAL_POINT_TRACKER_REGULAR_HH
#define _FTK_CRITICAL_POINT_TRACKER_REGULAR_HH

#include <ftk/ndarray.hh>
#include <ftk/mesh/lattice_partitioner.hh>
#include <ftk/filters/critical_point_tracker.hh>
#include <ftk/filters/regular_tracker.hh>
#include <ftk/external/diy-ext/gather.hh>

namespace ftk {

// this is an abstract class, not for users
struct critical_point_tracker_regular : public critical_point_tracker, public regular_tracker {
  critical_point_tracker_regular(diy::mpi::communicator comm, int nd) : critical_point_tracker(comm), regular_tracker(comm, nd), tracker(comm) {}
  virtual ~critical_point_tracker_regular() {}

protected:
  typedef simplicial_regular_mesh_element element_t;
  
  std::map<element_t, feature_point_t> discrete_critical_points;
  std::vector<std::set<element_t>> connected_components;

public: // cp io
  const std::map<element_t, feature_point_t>& get_discrete_critical_points() const {return discrete_critical_points;}

  std::vector<feature_point_t> get_critical_points() const;
  void put_critical_points(const std::vector<feature_point_t>&);
};

/////
////
inline std::vector<feature_point_t> critical_point_tracker_regular::get_critical_points() const
{
  std::vector<feature_point_t> results;
  for (const auto &kv : discrete_critical_points) 
    results.push_back(kv.second);
  return results;
}

inline void critical_point_tracker_regular::put_critical_points(const std::vector<feature_point_t>& data) 
{
  for (const auto& cp : data) {
    element_t e(m, cpdims(), cp.tag);
    discrete_critical_points[e] = cp;
  }
}

}

#endif
