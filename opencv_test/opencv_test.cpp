// opencv_test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
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

    Point2f center(img.cols/2, img.rows/2);
    Mat RM = getRotationMatrix2D(center,angle,1);
    Rect2f bb = RotatedRect(cv::Point2f(), img.size(), angle).boundingRect2f();

    RM.at<double>(0, 2) = 0;
    RM.at<double>(0, 2) += bb.width / 2.0;
    RM.at<double>(1, 2) += bb.height / 2.0 - img.rows / 2;
    
    warpAffine(img, new_image, RM, bb.size());
    return EXIT_SUCCESS;
}
int scaleImage(const Mat& img) {
    return EXIT_SUCCESS;
}

/*< alpha = contrast control */
 /*< beta = brightness control */
int change_brightnessImage(Mat& new_image, const Mat& img) {
    double alpha = 1.0; 
    int beta = 0;      
    std::cout << " Change brightness of image by Linear Transforms " << std::endl;
    std::cout << "# Enter the alpha value [1.0-3.0]: "; 
    std::cin >> alpha;
    std::cout << "* Enter the beta value [0-100]: ";    
    std::cin >> beta;

    for (int y = 0; y < img.rows; y++) {
        for (int x = 0; x < img.cols; x++) {
            for (int c = 0; c < img.channels(); c++) {
                new_image.at<Vec3b>(y, x)[c] =
                    saturate_cast<uchar>(alpha * img.at<Vec3b>(y, x)[c] + beta);
            }
        }
    }
    return EXIT_SUCCESS;
}
int mirrorImage(const Mat& img) {
    return EXIT_SUCCESS;
}

int main()
{
    Mat img = imread("img.jpg");
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
