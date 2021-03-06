# FTK's n-dimensional array

Note: this document is only useful if one wants to use FTK's C++ API and hack FTK impelemntations.  

We use `ftk::ndarray<T>` as the n-dimensional template container in the implementation of FTK, `T` being a trivially copyable type (e.g. `float`, `double`, and `int`).   

## Ordering, indexing, and conventions

We use the **row-major order** (a.k.a the "C" order) in  `ftk::ndarray`.  For example, in order to represent a WxH image, the array stores the every scanline of the image by the width, followed by the next scanline.

#### Handling multiple components

If an array is representing a vector field or tensor field, it is a convention to use the first couple dimensions as the "component" dimension.  For example, if the shape of a 3D array `scalar` is WxHxD,

* The gradient field array `grad` has the shape of 3xWxHxD
* The Hessian field array `hess` has the shape of 3x3xWxHxD

If you would like to convert this volume to VTK image data or other compatible formats, it is important to let `ftk::ndarray` know the how many dimensions are used to represent the components.  In the case of `grad`, the gradient components are one-dimensional; use`set_multicomponents(1)`.  In the case of `hess`, the Hessian components (3x3) are two-dimensional; use`set_multicomponents(2)`. 

## Usage

### Data access

We provide two ways to access individual elements.  Assuming the dimensionality of `array` is WxHxD, the following methods lead to the same element:

* `array.at(i, j, k)`
* `array(i, j, k)`
* `array[i + W*(j + D*k)]`

To access the raw pointer of the array, use `array.data()`.

### (Re)shaping

Functions are provided to get the size of all dimensions:

* `array.nd()` returns the dimensionality of the array
* `array.shape()` returns a vector of sizes of each dimension
* `array.shape(i)` returns the size of the i-th dimension
* `array.reshape(n0, n1, ..., n_d)` reshapes the array into the designated shape and reallocate the memory if necessary
* `array.nelem()` and `array.size()` return the total number of elements
* `array.empty()` is true if the array is empty

## (Serial) I/O

We ease the I/O with inline functions.  The use of VTK, NetCDF, HDF5, and ADIOS2 are required to compile and link FTK with corresponding external libraries

* `from_file(filename, [varname, communicator])` read data from file based on the filename extension.  Supported filename extensions include `.bp`, `.h5`, `.vti`, `.nc`.   The communicator is `MPI_COMM_WORLD` by default.

### VTK

Refer to [VTK documentation](https://vtk.org/documentation/) for more details in VTK data structures.

#### VTK Image Data

* `from_vtk_image_data_file(filename, varname) ` uses the `vtkXMLImageDataReader` to read a `.vti` file
* `to_vtk_image_data(varname)` converts `ndarray` to `vtkImageData`.  The `varname` and data components will be handled in the same manner of `to_vtk_data_array`
* `from_vtk_image_data(image_data, varname)` converts `vtkImageData` to `ndarray`.  If `varname` is specified, the designated array in the image data will be used; otherwise will use  `GetArray(0)` to determine which array to read

#### VTK Data Array

* `to_vtk_data_array(varname)` converts `ndarray` to `vtkDataArray`  If `varname` is specified, the name of `vtkDataArray` will be set to `varname`.   If there are multipe components, the  `vtkDataArray` will have multiple components as well.  The data type of the `vtkDataArray` will be automatically determined by `T`.  For example, `ndarray<double>` will use `vtkDataArray::CreateDataArray(VTK_DOUBLE)` to create the VTK data array.  

### NetCDF

Refer to [NetCDF documentation](https://www.unidata.ucar.edu/software/netcdf/docs/netcdf_documentation.html) for more details on NetCDF I/O. 

* `read_netcdf(filename/ncid, varname/varid, [starts, sizes])` read NetCDF variable with the designated file name / file identifier, variable name / variable identifier, and optionally starts and sizes of the read
* `to_netcdf(ncid, varid, [starts, sizes])` writes the array to an NetCDF variable

### HDF5

Refer to [HDF5 documentation](https://support.hdfgroup.org/HDF5/doc/) for more details on HDF5 I/O.

* `read_h5(filename/fid, dataname/did)` read HDF5 data with the designated filename / file identifer and data name / data identifer. 

### ADIOS2

Refer to [ADIOS2 documentation](https://adios2.readthedocs.io/en/latest/) for more details on ADIOS2.

* `read_bp(filename, varname, [communicator])` read `.bp ` file with the designated file name and variable name
* `read_bp(adios2::IO, adios2::Engine, varname, [timestep])` read ADIOS2 data with designated variable name and timestep

### POSIX

* `read_binary_file(filename/FILE, [offset])` read data from a raw binary file with POSIX I/O
* `to_binary_file(filename/FILE)` write data to a raw binary file with POSIX I/O

## Data conversion

Conversion from the following data structures are supported:

* Raw data pointer.  The shape of the array needs to be specified 
* `std::vector` and `std::array`.  The output will be a 1D array; reshape as needed
* `pybind11::array_t<T, pybind11::array::c_style | pybind11::array::forcecast>`.  Conversion between `numpy` arrays with `pybind11`.  The numpy array must be in C style.
* `vtkDataArray` and `vtkImageData`. See previous I/O section for more details.



