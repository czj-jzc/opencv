#include<iostream>
#include<opencv2/opencv.hpp>
using namespace std;
using namespace cv;

int main()
{
    Mat image = imread("/home/caozhijia/Desktop/iamge/image.jpg");
    if(image.empty()){
        perror("imread");
        exit(1);
    }
    resize(image,image,Size(0,0),0.2,0.2);
    imshow("imag",image);
    
    Mat image_gray;
    cvtColor(image,image_gray,COLOR_BGR2GRAY);
    imshow("gray",image_gray);

    vector<Mat> image_split;
    split(image_gray,image_split);
    image_gray = image_split.at(0);

    Mat image_thre;
    threshold(image_gray,image_thre,200,255,THRESH_BINARY);
    imshow("thre",image_thre);

     Mat pre;
    // 用矩阵获取结构元素，第一个参数返回椭圆卷积核，第二个参数为结构元素的尺寸，一般尺寸越大腐蚀越明显，第三个参数设置为中心点的位置
    Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3), Point(-1, -1));
    morphologyEx(image_thre, pre, MORPH_OPEN, kernel); // 输入图像，输出图像，开操作，结构元素

    imshow("pre",pre);
    Mat image_clone = image.clone();
    vector<Vec4i> hierarchy;    
    vector<vector<Point>> contours;
    findContours(pre,contours,RETR_EXTERNAL,CHAIN_APPROX_NONE);
    drawContours(image_clone,contours,-1,Scalar(0,0,255),2,4);
    imshow("image_draw",image_clone);

    Mat image_clone2 = image.clone();
    vector<RotatedRect> rotated_rect;
    for(const auto s : contours){
        auto minrota = minAreaRect(s);
        double width = minrota.size.width;
        double height = minrota.size.height;
        double area = width * height;
        if(width / height > 0.4)    continue;
        if(area < 10)   continue;
        else    rotated_rect.emplace_back(minrota);
    }
    vector<vector<Point2f>> Point_ret;
    for (const auto& rect : rotated_rect) {
        vector<Point2f> point(4); 
        rect.points(&point[0]); 
        line(image_clone2,point[0],point[1],Scalar(0,0,255),2,4,0);
        line(image_clone2,point[1],point[2],Scalar(0,0,255),2,4,0);
        line(image_clone2,point[2],point[3],Scalar(0,0,255),2,4,0);
        line(image_clone2,point[3],point[0],Scalar(0,0,255),2,4,0);
    }
    //here cant use drawcontours or polylines(they should use point but here is point2f)
    imshow("image_rota",image_clone2);
    
    Mat res_image = image.clone();
    vector<Point2f> pt(4);
    if(rotated_rect.size()<2){
        for(int i = 0 ; i < rotated_rect.size();++i){
            rotated_rect[i].points(&pt[0]);
            line(res_image,pt[0],pt[1],Scalar(0,0,255),2,4,0);
            line(res_image,pt[1],pt[2],Scalar(0,0,255),2,4,0);
            line(res_image,pt[2],pt[3],Scalar(0,0,255),2,4,0);
            line(res_image,pt[3],pt[0],Scalar(0,0,255),2,4,0);
        }
        imshow("res",res_image);
    }
    else{
        vector<RotatedRect> res_;
        for(int i = 0 ; i < rotated_rect.size() ; ++i)
        {
            for(int k = i+1 ; k < rotated_rect.size(); ++k)
            {
                double angle = abs(rotated_rect[i].angle - rotated_rect[k].angle);
                bool if1 = (angle < 5);
                if(if1)
                {
                    RotatedRect res;
                    res.center.x = (rotated_rect[k].center.x + rotated_rect[i].center.x) / 2;
                    res.center.y = (rotated_rect[k].center.y + rotated_rect[i].center.y) / 2;
                    res.angle = (rotated_rect[i].angle + rotated_rect[k].angle) / 2;
                    res.size.height = rotated_rect[i].size.height*1.5;
                    res.size.width = abs(rotated_rect[i].center.x - rotated_rect[k].center.x);
                    res_.emplace_back(res);
                }
            }
        }
        for(int j = 0 ; j < res_.size(); ++j){
            vector<Point2f> pt(4);
            res_[j].points(&pt[0]);
            line(res_image,pt[0],pt[1],Scalar(0,0,255),2,LINE_AA,0);
            line(res_image,pt[1],pt[2],Scalar(0,0,255),2,LINE_AA,0);
            line(res_image,pt[2],pt[3],Scalar(0,0,255),2,LINE_AA,0);
            line(res_image,pt[3],pt[0],Scalar(0,0,255),2,LINE_AA,0);
        }
    }
    imshow("res",res_image);
    // vector<RotatedRect> light;
    // for(int i = 0 ; i < contours.size() ; i++){
    //     RotatedRect minRotated = minAreaRect(contours[i]);
    //     double width = minRotated.size.width;
    //     double height = minRotated.size.height;
    //     double area = width * height;
    //     if (area < 10)
    //         continue; // 如果面积太小，忽略
    //     if (width / height > 0.4)
    //         continue; // 如果长宽比不符合，忽略
    //     else
    //         light.push_back(minRotated);   
    // }
    // Point2f pt[4];
    // for(int i = 0 ; i < 4 ;i++){
    //     pt[i].x = 0;
    //     pt[i].y = 0;
    // }
    // light[0].points(pt);
    // line(image, pt[0], pt[1], Scalar(0, 0, 255), 2, 4, 0);  
    // line(image, pt[1], pt[2], Scalar(0, 0, 255), 2, 4, 0);  
    // line(image, pt[2], pt[3], Scalar(0, 0, 255), 2, 4, 0);  
    // line(image, pt[3], pt[0], Scalar(0, 0, 255), 2, 4, 0);  
    
    //imshow("dis",image);

    waitKey(0);
    destroyAllWindows();
    return 0;
}