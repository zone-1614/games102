#include <iostream>
#include <rbf/RBF.h>
#include <rbf/kdtree.h>

using namespace zone;

int main() {
    RBF rbf("Arma_04.obj", "output.obj");
    func f = rbf.fit();
}