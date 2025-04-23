#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <chrono>
#include <numeric>
// What is the difference between namespace vs include???


/*
 * Never programmed in C++ before I'm just goin with it, thanks!
 * I'm going full guesses so probably everything or most of it is wrong
 *
 */


// I absolutely yipede yipeedoo this code is now ours ( I copied it )
// I somewhat understand it... I still just don't understand wtf is the point of :: maybe for static members?
// Looks like it... at least but why that and not .
bool isBrightEnough(const cv::Mat& frame, int radius = 10, double threshold = 100.0) {
    cv::Mat gray;
    if (frame.channels() == 3) // idk, don't ask me
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    else
        gray = frame;

    int cx = gray.cols / 2;
    int cy = gray.rows / 2;

    cv::Mat mask = cv::Mat::zeros(gray.size(), CV_8UC1);
    cv::circle(mask, cv::Point(cx, cy), radius, cv::Scalar(255), -1);
    double brightness = cv::mean(gray, mask)[0]; // basically just the average of the mask, which is a circle... I guess

    return brightness >= threshold; // sounds about right
}

// why is it a int? is it some error code thingy ?? (I didn't read anything I'm just going for it)
int main() {

    // why :: and not . ???????????????????????????
    cv::VideoCapture cap(0, cv::CAP_V4L2); // Using CAP_V4L2 (idk what it is) but it works

    // looks fine to me
    if (!cap.isOpened()) {
        // wtf is cerr, looks like cout so I guess console out, in this case console erorr?
        std::cerr << "cannot open webcam!" << std::endl; // why "<<" AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAa
        return -1; // why -1 ? (I should probably read the doc) I'm going full guess, if something is wrong we interupt the program with no error code?
    }

    // Camera settings
    /* I'm happy that some way to write the code is at least similar to C# or other languages
     * It's more about learning the syntax and words...
     * I'm not waiting for the moment where I will need to learn pointers and craps like that
     */
    cap.set(cv::CAP_PROP_AUTO_EXPOSURE, 1); // Why is 1 disabling it and not 0 wtf?
    cap.set(cv::CAP_PROP_AUTOFOCUS, 0); // and why this one is 0 to disable it WHAAAAAA
    cap.set(cv::CAP_PROP_FOCUS, 0); // okayish
    std::cout << cap.get(cv::CAP_PROP_FOCUS) << std::endl; // had to print it because idk if it was changing at all...
    cap.set(cv::CAP_PROP_ZOOM, 5); // Very zoomed cuz it's like a gun pew pew
    cap.set(cv::CAP_PROP_FPS, 60); // idk what this do but seems to cap the fps... don't work for me lol
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 128);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 128);
    cap.set(cv::CAP_PROP_EXPOSURE, 5);
    cap.set(cv::CAP_PROP_GAIN, 0);
    cap.set(cv::CAP_PROP_BRIGHTNESS, 0);
    cap.set(cv::CAP_PROP_CONTRAST, 0);
    cap.set(cv::CAP_PROP_SATURATION, 0);
    cap.set(cv::CAP_PROP_CONTRAST, 1);
    // ^^^ I completely and randomly tried random values, apparently no choices for that as each cameras is different....
    // I'm ashamed to admit it at least took me 2 hours to get the values right...

    int screenWidth = 1920;
    int screenHeight = 1080;




    // I love CTRL + C / CTRL + V
    int squareSize = std::gcd(screenWidth, screenHeight); // gcd seems to be "Greatest Common Divisor" seems to be basic math but too tired to even think rn, I just want my idea to work
    squareSize = std::min(screenWidth / 10, screenHeight / 10); // I love copypaste

    // Here we just do some mathy stuff and we do it so it give math stuff and gives us cols and rows
    int cols = screenWidth / squareSize;
    int rows = screenHeight / squareSize;

    // Recalculate actual usable screen area based on box size, again copy paste
    int fullWidth = cols * squareSize;
    int fullHeight = rows * squareSize;

    // Here we seem to create something
    cv::Mat flashWindow(fullHeight, fullWidth, CV_8UC3, cv::Scalar(0, 0, 0));
    // In C# it could be something like cv.Mat flashWindow = new(fullHeight,fullWidth, CV-8UC3, cv.Scalar(0,0,0));
    // Again I might be completely wrong as I barely understand C++ correctly

    // Prepare box positions
    std::vector<cv::Rect> boxes;
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            boxes.emplace_back(x * squareSize, y * squareSize, squareSize, squareSize);
        }
    }


    // at first it was blocking the camera, now it works, do I know why, yes, did I still copy someone else's code, yes
    int currentBox = 0;
    auto lastTime = std::chrono::steady_clock::now();

    cv::namedWindow("Flash Window", cv::WINDOW_NORMAL);
    cv::setWindowProperty("Flash Window", cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);

    cv::namedWindow("Webcam");

    while (true) {
        const int radius = 2;
        cv::Mat frame;
        cap >> frame;
        if (frame.empty()) break;

        bool bright = isBrightEnough(frame, radius, 75);

        // Clear window
        flashWindow.setTo(cv::Scalar(0, 0, 0));

        // Draw boxes
        for (int i = 0; i < boxes.size(); ++i) {
            if (i == currentBox) {
                flashWindow(boxes[i]).setTo(bright ? cv::Scalar(0, 0, 255) : cv::Scalar(255, 255, 255));
            }
        }

        cv::imshow("Flash Window", flashWindow);
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTime).count() > 25) {
            currentBox = (currentBox + 1) % boxes.size();
            lastTime = now;
        }

        int cx = frame.cols / 2;
        int cy = frame.rows / 2;
        cv::circle(frame, cv::Point(cx, cy), radius, cv::Scalar(0, 255, 0), 1);
        cv::imshow("Webcam", frame);



        if (cv::waitKey(1) == 27) break; // OH OH I KNOW THAT, waitkey is like... if we press a key right? and the key 27 is... ESC? (in ascii)
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}
