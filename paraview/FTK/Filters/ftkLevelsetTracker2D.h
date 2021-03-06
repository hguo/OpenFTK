#ifndef __ftkLevelsetTracker2D_h
#define __ftkLevelsetTracker2D_h

#include "vtkImageAlgorithm.h"
#include "vtkPolyDataAlgorithm.h"
#include <ftk/filters/contour_tracker_2d_regular.hh>

class vtkDataSet;

class ftkLevelsetTracker2D : public vtkImageAlgorithm
{
public:
  static ftkLevelsetTracker2D *New();
  vtkTypeMacro(ftkLevelsetTracker2D, vtkImageAlgorithm);

  vtkSetMacro(InputVariable, std::string);
  vtkGetMacro(InputVariable, std::string);
  
  vtkSetMacro(Threshold, double);
  vtkGetMacro(Threshold, double);

  vtkSetMacro(OutputType, int);
  vtkGetMacro(OutputType, int);

  vtkSetMacro(ZTimeScale, double);
  vtkGetMacro(ZTimeScale, double);

protected:
  ftkLevelsetTracker2D();
  ~ftkLevelsetTracker2D();

  int RequestUpdateExtent(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
  int RequestInformation(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
  int FillOutputPortInformation(int, vtkInformation*) override;
  
  int RequestSliced(vtkInformation*, vtkInformationVector**, vtkInformationVector*);

private:
  ftkLevelsetTracker2D(const ftkLevelsetTracker2D&);
  void operator=(const ftkLevelsetTracker2D&);

private:
  double Threshold;
  std::string InputVariable;
  int OutputType; // 0: sliced, 1: traced
  double ZTimeScale;

  int currentTimestep;
  int inputDataComponents;
  
  ftk::contour_tracker_2d_regular tracker;
  bool tracker_needs_recompute = true;
};

#endif
