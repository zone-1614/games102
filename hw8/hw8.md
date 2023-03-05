# 作业8
实现平面点集CVT的Lloyd算法

```
1. 在正方形内生成随机的点
2. 生成这些点的Voronoi剖分
3. 计算剖分的中心, 将采样点更新到重心
4. 重复2和3
```

## 尝试记录
首先尝试用Triangle库, 这个是编译成可执行文件, 然后自己输入一些点给他, 他会帮你生成Voronoi图, 但是自带的showme需要在linux下才能用, 我之前把ubuntu虚拟机删了, 现在只有wsl, 没法显示, 只能放弃这个库.

CGAL太庞大了, 我不怎么用. 去Github搜索Voronoi, 找star最多的那个`JCash/voronoi`