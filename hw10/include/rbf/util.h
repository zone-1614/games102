#pragma once

#include "config.h"
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <spdlog/spdlog.h>

namespace zone {

using Mesh = OpenMesh::TriMesh_ArrayKernelT<>;
using vh = OpenMesh::SmartVertexHandle;
using Point = Mesh::Point;

float distance(const Point& p1, const Point& p2);

}