// opencv_test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "stdint.h"
#include<opencv2/opencv.hpp>

using namespace cv;

enum class command
{
    NOTHING, ROTATE, SCALE, CHANGE_BRIGHTNESS, MIRROR
};

//Define operator>> for enum
std::istream& operator>>(std::istream& is, command& i) {
    int temp;
    if (is >> temp) {
        i = static_cast<command>(temp);
    }
    return is;
}
int rotateImage(Mat& new_image, const Mat& img) {
    double angle = 0;
    std::cout << " Change image rotation by linear transform " << std::endl;
    std::cout << "# Enter an angle to rotate to: ";
    std::cin >> angle;
    
    Point2d center(img.size().width/2, img.size().height/2);
    Rect2f bb = RotatedRect(center, img.size(), angle).boundingRect(); //bb - bounding box
    //Don't let the bounding box becme lower in height than the original image, it needs to fit the original image inside of itself before rotation
    if (bb.height < img.size().height) {
        bb.height = img.size().height;
    }
    Mat resized = Mat::zeros(bb.size(), img.type());
    //the distance to the new, bigger Mat from the image placed in the center
    //
    //               OffsetX
    //               <-->
    //               _ _ _ _ _ _ _
    // OffsetY {    |             |
    //              |   #- - -    |
    //              |   |     |   |
    //              |   |     |   |
    //              |   |_ _ _|   |
    //              |_ _ _ _ _ _ _|

    //Create a ROI - region of interest, in the bounding box matrix that will take into itself the original image to rotate, this is to place the original image in the center of the bounding box
    double OffsetX = (bb.width - img.size().width) / 2;
    double OffsetY = (bb.height - img.size().height) / 2;
    Rect roi = Rect(OffsetX, OffsetY, img.size().width, img.size().height);
    //Copy the original image (img) to the ROI inside the larger image - resized
    img.copyTo(resized(roi));
    //update the center position, since now the original image is inside the resized image.
    center += Point2d(OffsetX, OffsetY);
    //Get the rotation matrix with the given center and angle and apply it
    Mat M = getRotationMatrix2D(center, angle, 1.0);
    warpAffine(resized, new_image, M, resized.size());   
    return EXIT_SUCCESS;
}
int scaleImage(const Mat& img) {
    return EXIT_SUCCESS;
}
int change_brightnessImage(Mat& new_image, const Mat& img) {
    // alpha = contrast control 
    // beta = brightness control
    double alpha = 1.0; 
    int beta = 0;      
    std::cout << " Change brightness of image by Linear Transforms " << std::endl;
    std::cout << "# Enter the alpha value [1.0-3.0]: "; 
    std::cin >> alpha;
    std::cout << "* Enter the beta value [0-100]: ";    
    std::cin >> beta;

    //Mat::*.forEach method is the fastest way to iterate over an image, multithreaded function
    new_image.forEach<Vec3b>([&alpha, &beta, &img](Vec3b& c, const int position[]) -> void {
        for (int i = 0; i < 3; i++) {
            c[i] = saturate_cast<uint8_t>(alpha * img.at<Vec3b>(position[0], position[1])[i] + beta);
        }    
     });
    //Mat::*.at<Vec3b>(r, c) not the most efficient but more readable than foreach
    /*for (int y = 0; y < img.rows; y++) {
        for (int x = 0; x < img.cols; x++) {
            for (int c = 0; c < img.channels(); c++) {
                new_image.at<Vec3b>(y, x)[c] =
                    saturate_cast<uint8_t>(alpha * img.at<Vec3b>(y, x)[c] + beta);
            }
        }
    }*/
    return EXIT_SUCCESS;
}
int mirrorImage(const Mat& img) {
    return EXIT_SUCCESS;
}

int main()
{
    Mat img = imread("img.jpg", 1);
    if (img.empty())
        {
            std::cout << "failed to open img.jpg" << std::endl;
            return 1;
        }
    else
        {
            std::cout << "img.jpg loaded OK" << std::endl;
        }
    //move to each function because its not always the same size
    Mat new_image = Mat::zeros(img.size(), img.type());

    //Ask user for operation, rotate, scale, brightness, mirror
    command value = command::NOTHING;
    std::cout << "Specify operation: '1' rotate, '2' scale, '3' change brightness or '4' mirror image" << std::endl;
    std::cin >> value;

    int result = EXIT_FAILURE;
    //based on the user provided value, choose the right operation
    switch (value)
    {
    case command::NOTHING:
        //do nothing
        break;
    case command::ROTATE:
        result = rotateImage(new_image, img);
        break;
    case command::SCALE:
        scaleImage(new_image);
        break;
    case command::CHANGE_BRIGHTNESS:
        result = change_brightnessImage(new_image, img);
        break;
    case command::MIRROR:
        result = mirrorImage(new_image);
        break;
    default:
        break;
    }
    //Check to see if the operation was successfull or not
    if (result == EXIT_FAILURE) {
        return 0;
    }
    
    namedWindow("Original Image", WINDOW_NORMAL);
    namedWindow("New Image", WINDOW_NORMAL);
    imshow("Original Image", img);
    imshow("New Image", new_image);

    //namedWindow("image", WINDOW_NORMAL);
    //imshow("image", img);
    waitKey(0);
    return 0;
}
