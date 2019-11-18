#ifndef _FTK_CRITICAL_POINT_TRACKER_HH
#define _FTK_CRITICAL_POINT_TRACKER_HH

#include <ftk/ftk_config.hh>
#include <ftk/filters/filter.hh>
#include <ftk/geometry/points2vtk.hh>

namespace ftk {

template <int N, typename T>
struct critical_point_t {
  T operator[](size_t i) const {if (i >= N) return 0; else return x[i];}
  T x[N];
  T scalar = T(0);
  int type = 0;
};

struct critical_point_tracker : public filter {
  virtual void update() = 0;

#if FTK_HAVE_VTK
  virtual vtkSmartPointer<vtkPolyData> get_traced_critical_points_vtk() const = 0;
  virtual vtkSmartPointer<vtkPolyData> get_discrete_critical_points_vtk() const = 0;
#endif
  void write_traced_critical_points_vtk(const std::string& filename);
  void write_discrete_critical_points_vtk(const std::string& filename);
};

//////
#if FTK_HAVE_VTK
void critical_point_tracker::write_traced_critical_points_vtk(const std::string& filename)
{
  if (comm.rank() == 0) {
    auto poly = get_traced_critical_points_vtk();
    write_vtp(filename, poly);
  }
}

void critical_point_tracker::write_discrete_critical_points_vtk(const std::string& filename)
{
  if (comm.rank() == 0) {
    auto poly = get_discrete_critical_points_vtk();
    write_vtp(filename, poly);
  }
}
#else
void critical_point_tracker::write_traced_critical_points_vtk(const std::string& filename)
{
  if (comm.rank() == 0)
    fprintf(stderr, "[FTK] fatal: FTK not compiled with VTK.\n");
}

void critical_point_tracker::write_discrete_critical_points_vtk(const std::string& filename)
{
  if (comm.rank() == 0)
    fprintf(stderr, "[FTK] fatal: FTK not compiled with VTK.\n");
}
#endif

}

#endif
