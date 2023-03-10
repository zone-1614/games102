# 三次样条函数
> 这部分内容在史济怀的数学分析教程上册的附录中有详细推导

样条可以近似为分段三次函数, 三次函数只知道两个点, 要怎么求解?
## 求解思路
* 每段都是三次多项式, 有四个变量
$$
y_i(x) = a_i + b_ix + c_i x^2 + d_i x^3
$$
* 假设有n+1个点(n段), 则共有4n个变量
* 曲线要插值样本点, 可以得到n+1个约束条件
* 假设曲线$C^2$连续, 则相邻两段要满足3个条件($C^0, C^1, C^2$), 则共有3n-3个约束条件
* 以上加起来4n-2个方程, 还得再找两个条件, 才能唯一确定整条曲线. 
* 这两个条件一般加在两个端点, 称为边界条件, 下面会介绍
  

## 求解 
求解有两种方法, 第一种的中间变量是二阶导数值, 解三弯矩方程组, 另一种的中间变量是一阶导数值, 解三转角方程组. 

对角占优就满秩, 有唯一解. 可以用追赶法解三弯矩方程组

## 两个边界条件
* 自由端: 指定曲线在两个端点处的二阶导数值
  * 特别地, 两个端点的二阶导数值指定为0时称为自然三次样条
* 夹持端: 指定曲线在两个端点处的一阶导数值

## 曲线的几何连续性
几何连续性适合设计建模等等, 参数连续性依赖于参数化, 适合动画