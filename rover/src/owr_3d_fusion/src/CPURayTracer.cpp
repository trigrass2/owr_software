/*
 * Date Started: 26/12/15
 * Original Author: Harry J.E Day <harry@dayfamilyweb.com>
 * Editors:
 * ROS Node Name: camera_fusion_node
 * ROS Package: owr_3d_fusion
 * Purpose: A simple CPU based ray tracer used for testing
 */

#include "owr_3d_fusion/CPURayTracer.hpp"
#include <math.h>
#include "owr_3d_fusion/logitechC920.h"

// #define DEBUG

CPURayTracer::CPURayTracer() : RayTracer(), cld(new pcl::PointCloud<pcl::PointXYZRGB> ()) {
    
}

CPURayTracer::~CPURayTracer() {
    
}


void CPURayTracer::runTraces() {
    //NOTE: this function unfortuantly has to use two cordinate systems
    //a metric one in meters, and a pixel based one in pixels.
    int pixelX, pixelY;
    double metricX, metricY;
    //our accuracy does not require double precision
    float deltaX, deltaY;
    //calc this here so it only does the math once, #defines will run this many time
    const float focalLengthPx = (PX_TO_M/FOCAL_LENGTH_M);
    #ifdef DEBUG
        std::cout << "focalLengthPx:" << focalLengthPx << "PX_TO_M" <<PX_TO_M << std::endl;
    #endif
    const float pxToM = PX_TO_M;
    //NOTE: this is not the most efficient way to do this
    //but it is the simplest
    //see: http://docs.opencv.org/2.4/doc/tutorials/core/how_to_scan_images/how_to_scan_images.html
    //search the image
    for(pixelX = 0; pixelX < image.rows; pixelX++) {
        metricX= pixelX * pxToM;
        deltaX = tanh(metricX/FOCAL_LENGTH_M);
        for(pixelY = 0; pixelY < image.cols; pixelY++) {
            cv::Vec3b pt = image.at<cv::Vec3b>(pixelX,pixelY);
            metricY= pixelY * pxToM;
            deltaY = tanh(metricY/focalLengthPx);
            #ifdef DEBUG
                std::cout << "pixelY:" << pixelY
                    << "deltaY:" << deltaY 
                    <<  "metricY:" << metricY
                    << "tanInput:" << metricY*focalLengthPx << std::endl;
            #endif
            //gradient of z is the minimum resolution on the z axis of the point cloud
            simplePoint target;
            float dist = 0;
            //the node retrived
            octNode node;
            node.dimensions.x = 0;
            node.dimensions.y = 0;
            node.dimensions.z = 0;
            float incDist = RES;
            //this is in metric
            for(dist = 0; 
                dist < TRACE_RANGE;
                //use the larger of the dimension of the cell or half the resolution as an increase
//                 incDist = (RES < (node.dimensions.x/2)) ? (node.dimensions.x/2) : RES, dist+=incDist 
                dist+=RES
            ) {
                target.x = deltaX * dist;
                target.y = deltaY * dist;
                target.z = dist;
                #ifdef DEBUG
                    std::cout << target.x << "," << target.y << "," << target.z << " is target" << std::endl;
                #endif
                node = tree->getNode(target);
                #ifdef DEBUG
                    std::cout << node.orig.x << "," << node.orig.y << "," << node.orig.z << " is found" << std::endl;
                #endif
                
                //if a point is exists at our resolution then we have a match
                if(node.dimensions.x <= RES) {
                    //match
                    //NOTE: we have a loss of accuracy by using the target point here
                    //testing should be done to see if it is better to use the laserScan point.
                    std::cout << "match on small node" << std::endl;
                    match(target,pt);
                    break;
                //the next size up, match
                } else if (node.dimensions.x <= RES * 8) {
                    if(!node.getPointAt(tree->calculateIndex(target, node.orig)).isEmpty()) {
                        std::cout << "match on smallish node" << std::endl;
                        match(target,pt);
                        break;
                    }
                } else {
                    //check within required accuracy
                    simplePoint existingPoint = node.getPointAt(tree->calculateIndex(target, node.orig));
                    if(existingPoint.isEmpty()) {
                        #ifdef DEBUG
                            std::cout << "empty" << std::endl;
                        #endif
                        continue;
                    }
                    existingPoint = target-existingPoint;
                    
                    if(fabs(existingPoint.x) <= RES && fabs(existingPoint.y) <= RES && fabs(existingPoint.z) <= RES) {
                        std::cout << target.x << "," << target.y << "," << target.z << " is target" << std::endl;
                        //obviously this is a realy slow way to do this, but its only for debuging
                        std::cout << node.getPointAt(tree->calculateIndex(target, node.orig)).x << 
                            "," << node.getPointAt(tree->calculateIndex(target, node.orig)).y << 
                            "," << node.getPointAt(tree->calculateIndex(target, node.orig)).z << " is existing" << std::endl;
                        std::cout << existingPoint.x << "," << existingPoint.y << "," << existingPoint.z << " is diff" << std::endl;
                        std::cout << "match on comparision" << std::endl;
                        match(target,pt);
                        break;
                    }
                }
            }
        }
    }
}

void CPURayTracer::match ( simplePoint pt, cv::Vec3i pixel ) {
    pcl::PointXYZRGB newPt;
    newPt.x = pt.x;
    newPt.y = pt.y;
    newPt.z = pt.z;
    //BGR
    newPt.b = pixel[0];
    newPt.g = pixel[1];
    newPt.r = pixel[2];
    #ifdef DEBUG
        std::cout << newPt << std::endl;
    #endif
    cld->push_back(newPt);
}

shared_ptr< pcl::PointCloud< pcl::PointXYZRGB > > CPURayTracer::getResult() {
    return cld;
}