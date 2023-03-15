#pragma once

#include <rbf/util.h>

#include <memory>
#include <queue>

namespace zone {

struct kdnode;
using kdnode_ptr = std::shared_ptr<kdnode>;
using iter = std::vector<kdnode>::iterator;

struct kdnode {
    vh v;
    Point p;
    kdnode_ptr left;
    kdnode_ptr right;
    size_t index;
    float distance(const kdnode& n) const {
        auto pp = p - n.p;
        return std::sqrtf(pp[0] * pp[0] + pp[1] * pp[1] + pp[2] * pp[2]);
    }
    bool isLeaf() const { return left == nullptr && right == nullptr; }
};

struct kdnode_comparer {
    size_t idx;
    kdnode_comparer(size_t index) : idx(index) { }
    bool operator()(const kdnode& n1, const kdnode& n2) const { return n1.p[idx] < n2.p[idx]; }
};

class kdtree {
public:
    kdtree() {}
    kdtree(Mesh& mesh);
    std::vector<kdnode_ptr> k_nearest(size_t k, const vh& v);
private:
    void mesh_to_nodes();
    kdnode make_kdnode(const vh& v);
    kdnode_ptr make_tree(iter begin, iter end, size_t index);
// private:
public:
    std::shared_ptr<Mesh> mesh;
    std::vector<kdnode> nodes;
    kdnode_ptr root;

};

}