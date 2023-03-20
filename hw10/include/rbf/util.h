#pragma once

#include <sstream>
#include <algorithm>

#include "config.h"
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/Traits.hh>
#include <OpenMesh/Core/Utils/PropertyManager.hh>
#include <spdlog/spdlog.h>
#include <Eigen/Eigen>

namespace zone {

using Mesh = OpenMesh::TriMesh_ArrayKernelT<>;
using Normal = OpenMesh::DefaultTraits::Normal;
using vh = OpenMesh::SmartVertexHandle;
using Point = Mesh::Point;

std::string PointToString(const Point& p);

// rbf kernels
double kernel1(double x); // x
double kernel2(double x); // x^3
double kernel3(double x); // guassian 
double kernel4(double x); // x^2 log x

std::string EigenToString(const Eigen::MatrixXd& mat);


}