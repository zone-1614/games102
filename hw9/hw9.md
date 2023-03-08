# 作业9
实现网格简化的QEM方法

> 参考论文[Garland & Heckbert 1997](https://graphics.stanford.edu/courses/cs348a-04-winter/Papers/garland-heckbert.pdf)

## Application of mesh simplification
* LOD (Level of Detail)
* simulation proxy: run the simulation on a simplified model, and interpolate results across a more complicated model to be used for rendering

## Methodology of mesh simplification
* vertex removal
  * v = v - 1
  * f = f - 2
* edge collapse
  * v = v - 1
  * f = f - 2
* triangle (face) collapse
  * v = v - 2
  * f = f - 4

## Error Control
* local error
  * fast, but low mesh quality
  * memory-less
* global error
  * slow, but better mesh quality
  * must clone the old mesh (use more memory)


# Surface Simplification Using Quadric Error Metrics
* aggregation: join unconnected regions of the model together
* the algorithm is based on the iterative contraction of vertex pairs
* error approximation is represented by quadric matrices
* the algorithm do not assume that the topology of the model must be maintained (can't even be a manifold)

## Some relevant algorithms: 
1. **Vertex Decimation**: find a vertex for removal, remove all adjacent faces, and retriangulate the hole. This algorithm inherently limited to manifold surfaces. (manifold)
2. **Vertex Clustering**: use bounding box. the vertices of cell are clustered together into a single vertex. This algorithm is fast, but make drastic topological alterations to the model. (poor mesh quality)
3. **Iterative Edge Contraction** (can not support aggregation)


None of these previously developed algorithms provide the combination of **efficiency**, **quality** and **generality**

## Decimation via Pair Contraction
> def: the atomic operation of the algorithm:  *pair contraction*: $$(v_1, v_2) \rightarrow v$$

If $(v_1, v_2)$ is an edge, then one or more faces will be removed. Otherwise, two previously separate sections of the model will be joined at v.

* Pair Selection
  * a pair $(v_1, v_2)$ is *valid* if
    1. $(v_1, v_2)$ is an edge or
    2. $||v_1 - v_2|| < t$, $t$ is a threshold parameter

## Approximating Error With Quadrics
### def *cost* of a contraction
> 大的要来了, 用中文.

对每一个顶点, 维护一个4x4的对称阵Q(后面再说怎么定义这个矩阵), 定义顶点的误差
$$
\Delta(v) = v'Qv, \ v = [x, y, z, 1]'
$$
对每一个边坍缩$(v_1, v_2) \rightarrow v$, 定义新顶点的矩阵$Q = Q_1 + Q_2$, 新顶点的坐标求法
$$\min \Delta(v)\\ s.t. \ \partial \Delta / \partial x = \partial \Delta / \partial y = \partial \Delta / \partial z = 0$$得益于$\Delta$的定义, 其优化问题是线性的, 所以求解pde等价于求解线性方程组. 如果线性方程组不可解, 那就按照优化的套路, 做个最小二乘. 或者直接从$v_1, v_2, (v_1 + v_2)/2$ 三选一, 使得$\Delta(v)$ 比较小

## Algorithm
1. 对每一个顶点求Q
2. 选出所有 valid pair
3. 对每一个 pair 求 v, 再求 error
4. 根据 error, 把所有 pair 存于最小堆
5. 删除堆顶, 并坍缩对应点对, 更新 costs

## 唯一遗留的问题: Q
每一个顶点都是一系列平面的交, 用这些平面来度量误差. 首先定义一些量, 对平面p: $ax+by+cz+d=0$限制$a^2+b^2+c^2=1$(单位化法向), 定义$\bold{p} = [a, b, c, d]'$ 
$$
\Delta(\bold{v}) = \Delta([x, y, z, 1]') = \sum_{p\in planes(\bold{v})}(\bold{p}'\bold{v})^2 \\ = \sum_{p\in planes(\bold{v})}(\bold{v}'\bold{p})(\bold{p}'\bold{v}) = \sum_{p\in planes(\bold{v})}\bold{v}'(\bold{p}\bold{p}')\bold{v} \\
= \bold{v}'(\sum_{p\in planes(\bold{v})}K_p)\bold{v} 
$$
其中
$$
K_p = \bold{pp}' = \begin{bmatrix}
a^2 & ab & ac & ad \\ 
ab & b^2 & bc & bd \\
ac & bc & c^2 & cd\\
ad & bd & cd & d^2
\end{bmatrix}
$$
def
$$
Q = \sum_{p\in planes(\bold{v})}K_p
$$
从以上推导可以看出, 直接把坍缩后的矩阵Q设为Q1+Q2其实是错误的, 只不过这样省时间也省空间, 并且一个面的权重至多翻三倍, 不至于效果太差