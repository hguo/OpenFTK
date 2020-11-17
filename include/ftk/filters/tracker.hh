#ifndef _FTK_TRACKER_HH
#define _FTK_TRACKER_HH

#include <ftk/ftk_config.hh>
#include <ftk/filters/filter.hh>
#include <ftk/external/diy/master.hpp>

namespace ftk {

enum {
  TRACKER_CRITICAL_POINT = 1,
  TRACKER_TDGL_VORTEX = 2,
  TRACKER_CONTOUR = 3
};

struct tracker : public filter
{
  tracker(diy::mpi::communicator comm) : filter(comm) {} // , master(comm) {}
  virtual ~tracker() {}
  
  virtual int cpdims() const = 0; // featutre dimension
  
  void set_start_timestep(int t) { start_timestep = t;}
  void set_end_timestep(int t) { end_timestep = t; }
  
  virtual void set_current_timestep(int t) {current_timestep = t;}
  int get_current_timestep() const {return current_timestep;}
 
  void set_input_array_partial(bool b) {is_input_array_partial = b;}
  void set_use_default_domain_partition(bool b) {use_default_domain_partition = true;}

  static int str2tracker(const std::string&);

public:
  virtual void initialize() = 0;
  virtual void finalize() = 0;
  
  virtual bool advance_timestep() = 0;
  virtual void update_timestep() = 0;

protected:
  // diy::Master master;
  
protected:
  int start_timestep = 0, 
      end_timestep = std::numeric_limits<int>::max();

  int current_timestep = 0;
 
protected:
  bool is_input_array_partial = false;
  bool use_default_domain_partition = true;

protected: // benchmark
  double accumulated_kernel_time = 0.0;
};

////////
inline int tracker::str2tracker(const std::string& s) 
{
  if (s == "cp" || s == "critical_point") 
    return TRACKER_CRITICAL_POINT;
  else if (s == "iso" || s == "isovolume" || s == "isosurface" || s == "isosurfaces")
    return TRACKER_CONTOUR;
  else if (s == "tdgl" || s == "tdgl_vortex" || s == "tdgl_vortices")
    return TRACKER_TDGL_VORTEX;
  else return 0;
}

}

#endif
