# 微分坐标
## Laplace operator
拉普拉斯算子(梯度的散度)$\Delta f = \nabla \cdot \nabla f$
搞成离散的, 就是用差分代替微分
$$
\frac{\partial f}{\partial x} = \frac{y_{i+1}-y_i}{x_{i+1}-x_i}
$$
如果$x_{i+1}-x_i$都是1的话, 二阶差分就是$y_{i+1}-2y_i+y_{i-1}$

放到三维空间中, 就是上下左右的和减去中间的四倍
* 离散形式的Laplacian算子: $\delta_i = v_i - \sum_{j\in N(i)} w_jv_j$, (也叫**微分坐标**)

## 平均曲率流定理
$$
\lim_{len(\gamma)\rightarrow 0} \frac{1}{len(\gamma)} \int_{v\in \gamma} (v_i - v) ds = H(v_i)n_i
$$
极限号里面的东西就是微分坐标! 所以微分坐标刻画了平均曲率, 可以作为平均曲率的近似. 数学上可以推导, 权重$w_j = (\cot \alpha + \cot \beta)$是最好的. 后面的权重建议都取这个(因为有数学保证)

## Laplacian Smoothing Flow
$$
P_{new} \leftarrow P_{old} + \lambda L(P_{old})
$$
> 作业6就是在搞这个

## Discrete Mean Curvature Flow
$$
P_{new} \leftarrow P_{old} + \lambda H(P_{old}) n(P_{old}) \\
H_n = \frac{1}{4A} \sum (\cot \alpha_j + \cot \beta_j)(P-Q_j)
$$
兔子耳朵上顶点比较密, 拉普拉斯向量就短, 收敛就慢. 如果用平均曲率收敛速度就不会差别太大

如果微分坐标全为0, 也就是所有点都满足
$$
L(v_i) = v_i - \sum _{j\in N(i)} w_{ij} v_j = 0
$$
全部列出来, 变成一个很稀疏的线性方程组, 也就是网格曲面的整体Laplacian方程
$$
Ax = 0
$$

> 超大规模的稀疏矩阵

## Laplacian Matrix

$Lx = \delta_x, Ly=\delta_y, Lz = \delta_z$

## Reconstruction
知道L和$\delta$通过解方程就可以求出v(所有顶点), 如果把$\delta$缩小, 再重建, 其实就是作业6的另一种解法(整体方法), 令$\delta = 0$就可以直接得到极小曲面.
> 把点求出来怎么重建边的关系? 课上好像没讲

上面的矩阵L不满秩, 没有唯一解, 需要加约束. 约束太多可能就没有解了, 需要最小二乘

## 参数化
介绍Tutte文章的方法

参数化 -> 纹理映射 -> 显示到3D

## Adding Vertex Constraints
不加约束在迭代过程中, 曲面容易坍塌, 固定某些点就是加约束, 这里加软约束, 加完矩阵就会变大并且不是方阵, 就没唯一解, 要最小二乘$\min |Ax-b|$

## Laplacian Editing
编辑, 听起来简单, 改点的坐标就行, 但是你改一个点, 其他点不动就会很奇怪, 所以要让其他点也动起来. Laplacian度量了局部的性质, 所以还是解一个和Laplacian相关的方程