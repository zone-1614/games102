#pragma once

#include <rbf/util.h>
#include <rbf/RBF.h>

#include <array>
#include <memory>
#include <functional>

namespace zone {

struct onode;
using onode_ptr = std::shared_ptr<onode>;

struct onode {
    std::array<onode_ptr, 8> children;
    Point p1, p2; // p1: left up front, p2: right down back
    bool is_leaf = true;

    onode();
    onode(Point p1_, Point p2_);

    std::vector<Point> gen_all_points();
    bool ag0(const func& f); // all greater than 0
    bool al0(const func& f); // all less than 0

    Point middle();
};

onode_ptr make_onode_ptr();
onode_ptr make_onode_ptr(Point p1, Point p2);
void make_children(onode_ptr op);

class octree {
public:
    onode_ptr root;
    int n_leaf = 8;
    octree(const func& f, const Point& p1_, const Point& p2_); // bounding box 
    void traversal(const std::function<void(onode_ptr)>& tf);
    double get_threshold() const;

private:
    double threshold;
};

}