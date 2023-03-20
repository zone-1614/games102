#include <rbf/kdtree.h>

#include <stack>

namespace zone {

kdtree::kdtree(Mesh& mesh_) {
    this->mesh = std::make_shared<Mesh>(mesh_);
    mesh_to_nodes();
    root = make_tree(nodes.begin(), nodes.end(), 0);
}

void kdtree::mesh_to_nodes() {
    nodes.clear();
    for (auto v : mesh->vertices()) {
        auto node = make_kdnode(v);
        nodes.push_back(node);
    }
}

kdnode kdtree::make_kdnode(const vh& v) {
    kdnode node;
    node.left = nullptr;
    node.right = nullptr;
    node.v = v;
    node.index = 0;
    node.p = mesh->point(v);
    return node;
}

kdnode_ptr kdtree::make_tree(iter begin, iter end, size_t index) {
    if (end <= begin) 
        return nullptr;
    auto mid = begin + (end - begin) / 2;
    int m = mid - nodes.begin();
    std::nth_element(begin, mid, end, kdnode_comparer(index));
    nodes[m].index = index;
    index = (index + 1) % 3;
    nodes[m].left = make_tree(begin, mid, index);
    nodes[m].right = make_tree(mid + 1, end, index);
    return std::make_shared<kdnode>(nodes[m]);
}

bool operator<(std::pair<kdnode_ptr, double> p1, std::pair<kdnode_ptr, double> p2) {
    return p1.second > p2.second;
}

std::vector<kdnode_ptr> kdtree::k_nearest(size_t k, const Point& p) const {
    if (root == nullptr) 
        return {};

    std::priority_queue<std::pair<kdnode_ptr, double>> q;

    // 1. search path
    kdnode_ptr curr = root;
    std::stack<kdnode_ptr> stk;
    // add the ptr and the suitable child of ptr to the stack
    auto add_path = [&](kdnode_ptr ptr) {
        while (ptr != nullptr) {
            stk.push(ptr);
            if (p[ptr->index] < ptr->p[ptr->index]) {
                ptr = ptr->left;
            } else {
                ptr = ptr->right;
            }
        }    
    };
    add_path(curr);

    // 2. backtracking and find knn
    while (!stk.empty()) {
        curr = stk.top();
        stk.pop();

        double dist = (p - curr->p).norm();
        if (curr->p == p) continue;
        // try to push curr
        if (q.size() < k) {
            q.push({ curr, dist });
        } else {
            auto [_, d] = q.top();
            if (d > dist) {
                q.pop();
                q.push({ curr, dist });
            }
        }
        
        // have children
        if (!curr->isLeaf()) {
            double dx = std::abs(curr->p[curr->index] - p[curr->index]);
            if (q.size() < k || dx < q.top().second) {
                kdnode_ptr child_ptr;
                // deep into another child
                if (p[curr->index] < curr->p[curr->index]) {
                    child_ptr = curr->right;
                } else {
                    child_ptr = curr->left;
                }
                add_path(child_ptr);
            }
        }
    }

    // 3. construct the result
    std::vector<kdnode_ptr> result;
    while (!q.empty()) {
        auto [nn, d] = q.top();
        result.push_back(nn);
        q.pop();
    }
    return result;
}

double kdtree::nearest_distance(const Point& p) const {
    double min_dis = DBL_MAX;
    kdnode_ptr curr = root;
    kdnode_ptr nearest = root;
    std::stack<kdnode_ptr> stk;
    auto add_path = [&](kdnode_ptr ptr) {
        while (ptr != nullptr) {
            stk.push(ptr);
            if (p[ptr->index] < ptr->p[ptr->index]) {
                ptr = ptr->left;
            } else {
                ptr = ptr->right;
            }
        }
    };
    add_path(curr);

    while (!stk.empty()) {
        curr = stk.top();
        stk.pop();

        double dis = (curr->p - p).norm();
        if (dis < min_dis) {
            min_dis = dis;
            nearest = curr;
        }
        if (!curr->isLeaf()) {
            double dx = std::abs(curr->p[curr->index] - p[curr->index]);
            if (dx < min_dis) {
                kdnode_ptr child_ptr;
                // deep into another child
                if (p[curr->index] < curr->p[curr->index]) {
                    child_ptr = curr->right;
                } else {
                    child_ptr = curr->left;
                }
                add_path(child_ptr);
            }
        }
    }
    return min_dis;
}

}