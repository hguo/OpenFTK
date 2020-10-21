#ifndef _FTK_CRITICAL_POINT_TRAJ_SET_HH
#define _FTK_CRITICAL_POINT_TRAJ_SET_HH

#include <ftk/filters/feature_curve.hh>
#include <map>

#if FTK_HAVE_VTK
#include <vtkUnsignedIntArray.h>
#include <vtkDoubleArray.h>
#include <vtkVertex.h>
#include <vtkSmartPointer.h>
#endif

namespace ftk {

struct feature_curve_set_t : public std::map<int, feature_curve_t>
{
  int add(const feature_curve_t&);
  std::vector<int> add(const std::vector<feature_curve_t>&);

  std::vector<int> split(int);
  void split_all();
  
  void foreach(std::function<void(const feature_curve_t&)> f) const {for (const auto& kv : *this) f(kv.second);}
  void foreach(std::function<void(feature_curve_t&)> f) {for (auto& kv : *this) f(kv.second);}
  void foreach(std::function<void(int, const feature_curve_t&)> f) const {for (const auto& kv : *this) f(kv.first, kv.second);}
  void foreach(std::function<void(int, feature_curve_t&)> f) {for (auto& kv : *this) f(kv.first, kv.second);}

  void filter(std::function<bool(const feature_curve_t&)> f);

  std::vector<feature_point_t> slice(int t) const;
  feature_curve_set_t intercept(int t0, int t1) const;

public: // IO
  void write(const std::string& format, const std::string& filename) const;
  void read(const std::string& format, const std::string& filename);

  void write_json(const std::string& filename, int indent=0) const;
  void read_json(const std::string& filename);

  void write_binary(const std::string& filename) const;
  void read_binary(const std::string& filename);

  void write_text(std::ostream& os, const int cpdims, const std::vector<std::string>& scalar_components) const;
  void write_text(const std::string& filename, const int cpdims, const std::vector<std::string>& scalar_components) const;

  void write_vtk(const std::string& filename) const;

#if FTK_HAVE_VTK
  vtkSmartPointer<vtkPolyData> to_vtp(const int ncdims, 
      const std::vector<std::string> &scalar_components, 
      double tfactor=1.0) const;
#endif

protected:
  int get_new_id() const {
    if (empty()) return 0; 
    else return rbegin()->first + 1;
  }
};

}

namespace nlohmann
{
  using namespace ftk;
  template <>
  struct adl_serializer<feature_curve_set_t> {
    static void to_json(json& j, const feature_curve_set_t& s) {
      j = {{"trajs", static_cast<std::map<int, feature_curve_t>>(s)}};
    }
   
    static void from_json(const json&j, feature_curve_set_t& s) {
      std::map<int, feature_curve_t> trajs = j["trajs"];
      s.clear();
      for (auto &kv : trajs)
        s.insert(kv);
      // s.insert(trajs.begin(), trajs.end());
    }
  };
}


// serialization
namespace diy {
  static void save(diy::BinaryBuffer& bb, const ftk::feature_curve_set_t &s) {
    diy::save(bb, s.size());
    for (const auto &kv : s) {
      diy::save(bb, kv.first);  // identifer
      diy::save(bb, kv.second); // traj
    }
  }

  static void load(diy::BinaryBuffer& bb, ftk::feature_curve_set_t &s) {
    size_t size;
    diy::load(bb, size);
    for (auto i = 0; i < size; i ++) {
      int id;
      diy::load(bb, id);
      
      ftk::feature_curve_t traj;
      diy::load(bb, traj);
      traj.relabel(id);

      s[id] = traj;
    }
  }
} // namespace diy

//////
namespace ftk {

inline void feature_curve_set_t::write(const std::string& format, const std::string& filename) const
{
  // TODO
}

inline void feature_curve_set_t::read(const std::string& format, const std::string& filename)
{
  // TODO
}

inline void feature_curve_set_t::write_json(const std::string& filename, int indent) const
{
  // TODO
}

inline void feature_curve_set_t::read_json(const std::string& filename)
{
  // TODO
}

inline void feature_curve_set_t::write_binary(const std::string& filename) const
{
  // TODO
}

inline void feature_curve_set_t::read_binary(const std::string& filename)
{
  // TODO
}


inline void feature_curve_set_t::write_text(std::ostream& os, const int cpdims, const std::vector<std::string>& scalar_components) const
{
  os << "#trajectories=" << size() << std::endl;
  for (const auto &kv : *this) {
    const auto i = kv.first;
    const auto &curve = kv.second;
    
    os << "--trajectory " << i << ", ";
   
    if (scalar_components.size() > 0) {
      os << "min=(";
      for (int k = 0; k < scalar_components.size(); k ++)
        if (k < scalar_components.size()-1) os << curve.min[k] << ", ";
        else os << curve.min[k] << "), ";
      
      os << "max=(";
      for (int k = 0; k < scalar_components.size(); k ++)
        if (k < scalar_components.size()-1) os << curve.max[k] << ", ";
        else os << curve.max[k] << "), ";
      
      os << "persistence=(";
      for (int k = 0; k < scalar_components.size(); k ++)
        if (k < scalar_components.size()-1) os << curve.persistence[k] << ", ";
        else os << curve.persistence[k] << "), ";
    }

    os << "bbmin=(";
    for (int k = 0; k < cpdims; k ++)
      if (k < cpdims) os << curve.bbmin[k] << ", ";
      else os << curve.bbmin[k] << "), ";
    
    os << "bbmax=(";
    for (int k = 0; k < cpdims; k ++)
      if (k < cpdims) os << curve.bbmax[k] << ", ";
      else os << curve.bbmax[k] << "), ";
    
    os << "tmin=" << curve.tmin << ", tmax=" << curve.tmax << ", ";

    // os << "consistent_type=" << critical_point_type_to_string(cpdims, curve.consistent_type, scalar_components.size()) << ", ";
    os << "consistent_type=" << curve.consistent_type << ", ";
    os << "loop=" << curve.loop;
    os << std::endl;

    for (int k = 0; k < curve.size(); k ++) {
      os << "---";
      curve[k].print(os, cpdims, scalar_components) << std::endl;
    }
  }
}

inline void feature_curve_set_t::write_text(const std::string& filename, const int cpdims, const std::vector<std::string>& scalar_components) const
{
  // TODO
}


inline void feature_curve_set_t::write_vtk(const std::string& filename) const
{
  // TODO
}

#if FTK_HAVE_VTK
vtkSmartPointer<vtkPolyData> feature_curve_set_t::to_vtp(const int cpdims, const std::vector<std::string> &scalar_components, double tfactor) const
{
  vtkSmartPointer<vtkPolyData> polyData = vtkPolyData::New();
  vtkSmartPointer<vtkPoints> points = vtkPoints::New();
  vtkSmartPointer<vtkCellArray> lines = vtkCellArray::New();
  vtkSmartPointer<vtkCellArray> verts = vtkCellArray::New();

  foreach([&](const feature_curve_t& curve) {
    for (auto i = 0; i < curve.size(); i ++) {
      double p[3] = {curve[i][0], curve[i][1], cpdims == 2 ? curve[i].t * tfactor : curve[i][2]};
      points->InsertNextPoint(p);
    }
  });

  size_t nv = 0;
  foreach([&](const feature_curve_t& curve) {
    if (curve.empty()) { // skip
    } else if (curve.size() < 2) { // isolated vertex
      vtkSmartPointer<vtkVertex> obj = vtkVertex::New();
      obj->GetPointIds()->SetNumberOfIds(curve.size());
      for (int i = 0; i < curve.size(); i ++)
        obj->GetPointIds()->SetId(i, i+nv);
      verts->InsertNextCell(obj);
    } else { // lines
      vtkSmartPointer<vtkPolyLine> obj = vtkPolyLine::New();
      obj->GetPointIds()->SetNumberOfIds(curve.size());
      for (int i = 0; i < curve.size(); i ++)
        obj->GetPointIds()->SetId(i, i+nv);
      lines->InsertNextCell(obj);
    }
    nv += curve.size();
  });
 
  polyData->SetPoints(points);
  polyData->SetLines(lines);
  polyData->SetVerts(verts);

  // point data for types
  if (1) { // if (type_filter) {
    vtkSmartPointer<vtkUnsignedIntArray> types = vtkSmartPointer<vtkUnsignedIntArray>::New();
    types->SetNumberOfValues(nv);
    size_t i = 0;
    foreach([&](const feature_curve_t& curve) {
      for (auto j = 0; j < curve.size(); j ++)
        types->SetValue(i ++, curve[j].type);
    });
    types->SetName("type");
    polyData->GetPointData()->AddArray(types);
  }

  if (1) { // ids
    vtkSmartPointer<vtkUnsignedIntArray> ids = vtkSmartPointer<vtkUnsignedIntArray>::New();
    ids->SetNumberOfValues(nv);
    size_t i = 0;
    foreach([&](int id, const feature_curve_t& curve) {
      for (auto j = 0; j < curve.size(); j ++)
        ids->SetValue(i ++, id);
    });
    ids->SetName("id");
    polyData->GetPointData()->AddArray(ids);
  }
  
  if (1) { // velocities
    vtkSmartPointer<vtkDoubleArray> vels = vtkSmartPointer<vtkDoubleArray>::New();
    vels->SetNumberOfComponents(3);
    vels->SetNumberOfTuples(nv);
    size_t i = 0;
    foreach([&](const feature_curve_t& curve) {
      for (auto j = 0; j < curve.size(); j ++)
        vels->SetTuple3(i ++, curve[j].v[0], curve[j].v[1], curve[j].v[2]);
    });
    vels->SetName("velocity");
    polyData->GetPointData()->AddArray(vels);
  }
  
  if (1) { // time
    vtkSmartPointer<vtkDoubleArray> time = vtkSmartPointer<vtkDoubleArray>::New();
    time->SetNumberOfValues(nv);
    size_t i = 0;
    foreach([&](const feature_curve_t& curve) {
      for (auto j = 0; j < curve.size(); j ++)
        time->SetValue(i ++, curve[j].t);
    });
    time->SetName("time");
    polyData->GetPointData()->AddArray(time);
  }
  
  if (1) { // condition numbers
    vtkSmartPointer<vtkDoubleArray> conds = vtkSmartPointer<vtkDoubleArray>::New();
    conds->SetNumberOfValues(nv);
    size_t i = 0;
    foreach([&](const feature_curve_t& curve) {
      for (auto j = 0; j < curve.size(); j ++)
        conds->SetValue(i ++, curve[j].cond);
    });
    conds->SetName("cond");
    polyData->GetPointData()->AddArray(conds);
  }
  
  // point data for scalars
  // if (has_scalar_field) {
  for (auto k = 0; k < scalar_components.size(); k ++) {
    vtkSmartPointer<vtkDoubleArray> scalar = vtkSmartPointer<vtkDoubleArray>::New();
    scalar->SetNumberOfValues(nv);
    size_t i = 0;
    foreach([&](const feature_curve_t& curve) {
      for (auto j = 0; j < curve.size(); j ++)
        scalar->SetValue(i ++, curve[j].scalar[k]);
    });
    scalar->SetName(scalar_components[k].c_str());
    polyData->GetPointData()->AddArray(scalar);
  }

  return polyData;
}
#endif

inline void feature_curve_set_t::filter(std::function<bool(const feature_curve_t&)> f)
{
  for (auto it = cbegin(); it != cend(); ) {
    if (!f(it->second)) 
      erase(it ++);
    else 
      ++ it;
  }
}

inline int feature_curve_set_t::add(const feature_curve_t& t)
{
  const int id = get_new_id();
  insert(std::pair<int, feature_curve_t>(id, t));
  at(id).relabel(id);
  return id;
}

inline std::vector<int> feature_curve_set_t::add(const std::vector<feature_curve_t>& trajs)
{
  std::vector<int> ids;
  for (const auto &traj : trajs)
    ids.push_back(add(traj));
  return ids;
}

inline std::vector<int> feature_curve_set_t::split(int i)
{
  std::vector<int> result;
  const auto subtrajs = at(i).split();
  for (const auto &traj : subtrajs)
    result.push_back( add(traj) );
  erase(i);
  return result;
}

inline void feature_curve_set_t::split_all()
{
  std::vector<feature_curve_t> result;
  std::vector<int> to_erase;

  for (const auto &kv : *this) {
    if (!kv.second.consistent_type) {
      const auto subtrajs = kv.second.split();
      result.insert(result.end(), subtrajs.begin(), subtrajs.end());
      to_erase.push_back(kv.first);
    }
  }

  for (const auto k : to_erase)
    erase(k);

  for (const auto &traj : result)
    add(traj);
}

feature_curve_set_t feature_curve_set_t::intercept(int t0, int t1) const
{
  feature_curve_set_t result;
  for (const auto &kv : * this) {
    auto traj = kv.second.intercept(t0, t1);
    if (!traj.empty())
      result[kv.first] = traj;
  }
  return result;
}

} // namespace ftk

#endif