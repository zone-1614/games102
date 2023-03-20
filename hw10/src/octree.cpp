#include <rbf/octree.h>

#include <queue>

namespace zone {

onode::onode() {
    p1 = {0.0, 0.0, 0.0};
    p2 = {0.0, 0.0, 0.0};
}

onode::onode(Point p1_, Point p2_) {
    p1 = p1_;
    p2 = p2_;
}

std::vector<Point> onode::gen_all_points() {
    std::vector<Point> ps;
    ps.push_back({ p2[0], p1[1], p2[2] });   // 0
    ps.push_back(p2);                        // 1
    ps.push_back({ p1[0], p2[1], p2[2] });   // 2
    ps.push_back({ p1[0], p1[1], p2[2] });   // 3
    ps.push_back({ p2[0], p1[1], p1[2] });   // 4 
    ps.push_back({ p2[0], p2[1], p1[2] });   // 5
    ps.push_back({ p1[0], p2[1], p1[2] });   // 6
    ps.push_back(p1);                        // 7
    return ps;
}

bool onode::ag0(const func& f) {
    auto ps = gen_all_points();

    for (auto p : ps) {
        if (f(p) <= 0.0)
            return false;
    }
    return true;
}

bool onode::al0(const func& f) {
    auto ps = gen_all_points();

    for (auto p : ps) {
        if (f(p) >= 0.0)
            return false;
    }
    return true;
}

Point onode::middle() {
    return (p1 + p2) / 2.0;
}

onode_ptr make_onode_ptr() {
    return std::make_shared<onode>();
}

onode_ptr make_onode_ptr(Point p1, Point p2) {
    return std::make_shared<onode>(p1, p2);
}

void make_children(onode_ptr op) {
    op->is_leaf = false;
    Point t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12;
    double x1 = op->p1[0], y1 = op->p1[1], z1 = op->p1[2],
          x2 = op->p2[0], y2 = op->p2[1], z2 = op->p2[2];
    t0  = { (x1 + x2) / 2.0 ,    (y1 + y2) / 2.0 ,   (z1 + z2) / 2.0  };
    t1  = {  x1             ,    (y1 + y2) / 2.0 ,    z1              };
    t2  = { (x1 + x2) / 2.0 ,     y2             ,   (z1 + z2) / 2.0  };
    t3  = { (x1 + x2) / 2.0 ,    (y1 + y2) / 2.0 ,    z1              };
    t4  = {  x2             ,     y2             ,   (z1 + z2) / 2.0  };
    t5  = { (x1 + x2) / 2.0 ,     y1             ,    z1              };
    t6  = {  x2             ,    (y1 + y2) / 2.0 ,   (z1 + z2) / 2.0  };
    t7  = {  x1             ,     y1             ,   (z1 + z2) / 2.0  };
    t8  = { (x1 + x2) / 2.0 ,    (y1 + y2) / 2.0 ,    z2              };
    t9  = {  x1             ,    (y1 + y2) / 2.0 ,   (z1 + z2) / 2.0  };
    t10 = { (x1 + x2) / 2.0 ,     y2             ,    z2              };
    t11 = { (x1 + x2) / 2.0 ,     y1             ,   (z1 + z2) / 2.0  };
    t12 = {  x2             ,    (y1 + y2) / 2.0 ,    z2              };
    op->children[0] = make_onode_ptr( op->p1,  t0     );
    op->children[1] = make_onode_ptr( t1,      t2     );
    op->children[2] = make_onode_ptr( t3,      t4     );
    op->children[3] = make_onode_ptr( t5,      t6     );
    op->children[4] = make_onode_ptr( t7,      t8     );
    op->children[5] = make_onode_ptr( t9,      t10    );
    op->children[6] = make_onode_ptr( t0,      op->p2 );
    op->children[7] = make_onode_ptr( t11,     t12    );
}

octree::octree(const func& f, const Point& p1_, const Point& p2_) {
    // 
    spdlog::info("init octree");
    threshold = (p1_[0] - p2_[0]) / 80.0;
    root = make_onode_ptr(p1_, p2_);
    //root = make_onode_ptr({ 1.2, -1.2, 1.2 }, { -1.2, 1.2, -1.2 });
    make_children(root);

    std::queue<onode_ptr> q;
    for (auto& op : root->children) {
        q.push(op);
    }
    spdlog::info("init octree success");

    // loop
    int i = 0;
    while (!q.empty()) {
        auto& curr = q.front();
        q.pop();
        double ls = curr->p1[0] - curr->p2[0]; // length of side
        if (ls < threshold) continue;
        if (curr->ag0(f) || curr->al0(f)) continue;
        make_children(curr); n_leaf += 7;
        for (auto& ch : curr->children) {
            q.push(ch);
        }
        i++;
        if (i % 100 == 0) {
            spdlog::info("make child {} times", i);
        }
    }
}

void octree::traversal(const std::function<void(onode_ptr)>& tf) {
    std::queue<onode_ptr> q;
    q.push(root);

    while (!q.empty()) {
        auto& t = q.front();
        q.pop();
        if (!t->is_leaf) {
            for (auto& ch : t->children) {
                q.push(ch);
            }
        }
        tf(t);
    }
}

double octree::get_threshold() const {
    return threshold;
}

}