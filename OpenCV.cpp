#include "eVehicle.hpp"

using namespace cv;
using namespace std;

int getPic(char * name) {
        VideoCapture stream(0);   //0 is the id of video device.0 if you have only one camera.

        stream.set(CV_CAP_PROP_FRAME_WIDTH,1280);
        stream.set(CV_CAP_PROP_FRAME_HEIGHT,720);

        if (!stream.isOpened()) { //check if video device has been initialised
                cout << "cannot open camera";
        }
        Mat cameraFrame;
        stream.read(cameraFrame);
        try {
                imwrite(name, cameraFrame);
        } catch (runtime_error& ex) {
                fprintf(stderr, "Exception converting image to PNG format: %s\n", ex.what());
                return -1;
        }
        usleep(500);
        return 0;
}