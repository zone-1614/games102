#include <iostream>

#include "config.h"

#include <pcl/io/pcd_io.h>
#include <pcl/io/obj_io.h>
#include <pcl/PolygonMesh.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/features/normal_3d.h>
#include <pcl/surface/poisson.h>

pcl::PointCloud<pcl::PointXYZ>::Ptr read_obj_and_convert_to_pcd(std::string obj_filename, std::string pcd_filename);
pcl::PointCloud<pcl::Normal>::Ptr compute_normal(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud);
pcl::PolygonMesh Poisson(pcl::PointCloud<pcl::PointNormal>::Ptr cloud_with_normals);

int main() {
    auto cloud = read_obj_and_convert_to_pcd("model/dragon.obj", "model/dragon.pcd");
    auto normals = compute_normal(cloud);
    pcl::PointCloud<pcl::PointNormal>::Ptr cloud_with_normals(new pcl::PointCloud<pcl::PointNormal>);
    pcl::concatenateFields(*cloud, *normals, *cloud_with_normals);

    auto mesh = Poisson(cloud_with_normals);
    pcl::io::saveOBJFile(std::string(project_path) + "model/output_dragon.obj", mesh);
}

pcl::PointCloud<pcl::PointXYZ>::Ptr read_obj_and_convert_to_pcd(std::string filename, std::string pcd_filename) {
    pcl::PolygonMesh mesh;
    pcl::io::loadOBJFile(std::string(project_path) + filename, mesh);
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
    pcl::fromPCLPointCloud2(mesh.cloud, *cloud);
    pcl::io::savePCDFileASCII(std::string(project_path) + pcd_filename, *cloud);
    return cloud;
}

pcl::PointCloud<pcl::Normal>::Ptr compute_normal(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud) {
    pcl::NormalEstimation<pcl::PointXYZ, pcl::Normal> ne;
    ne.setInputCloud(cloud);

    pcl::search::KdTree<pcl::PointXYZ>::Ptr tree(new pcl::search::KdTree<pcl::PointXYZ>());
    ne.setSearchMethod(tree);

    pcl::PointCloud<pcl::Normal>::Ptr cloud_normals(new pcl::PointCloud<pcl::Normal>);

    ne.setRadiusSearch(0.03);
    ne.compute(*cloud_normals);

    return cloud_normals;
}

pcl::PolygonMesh Poisson(pcl::PointCloud<pcl::PointNormal>::Ptr cloud_with_normals) {
    pcl::search::KdTree<pcl::PointNormal>::Ptr kdt(new pcl::search::KdTree<pcl::PointNormal>);
    kdt->setInputCloud(cloud_with_normals);

    pcl::Poisson<pcl::PointNormal> pn;
    pn.setSearchMethod(kdt);
    pn.setInputCloud(cloud_with_normals);
    pn.setConfidence(false);
    pn.setManifold(false);
    pn.setOutputPolygons(false);
    pn.setIsoDivide(8);
    pn.setSamplesPerNode(5.0);
    pcl::PolygonMesh mesh;
    pn.performReconstruction(mesh);
    return mesh;
}