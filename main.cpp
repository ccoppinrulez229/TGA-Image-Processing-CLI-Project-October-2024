#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstring>
using namespace std;

//whenever you use .read(), c++ remembers the current byte position in the file and stays there until called again
//cout << "image height" << static_cast<short>(header.image_height) << endl;
struct Header { //struct that contains a file's header content
    char id_length;
    char colormap_type;
    char image_type;
    short colormap_origin;
    short colormap_length;
    char colormap_depth;
    short x_origin;
    short y_origin;
    short image_width;
    short image_height;
    char pixel_depth;
    char image_descriptor;
};

Header getHeader(ifstream& file) { //reads the header portion of the parameter file and assigns everything to a header struct
    Header header;
    file.read((char*) &header.id_length, 1);
    file.read((char*) &header.colormap_type, 1);
    file.read((char*) &header.image_type,1);
    file.read((char*) &header.colormap_origin,2);
    file.read((char*) &header.colormap_length, 2);
    file.read((char*) &header.colormap_depth, 1);
    file.read((char*) &header.x_origin, 2);
    file.read((char*) &header.y_origin, 2);
    file.read((char*) &header.image_width, 2);
    file.read((char*) &header.image_height, 2);
    file.read((char*) &header.pixel_depth, 1);
    file.read((char*) &header.image_descriptor, 1);

    /*cout << "-----" << endl;
    cout << static_cast<char>(header.id_length) << endl;
    cout << static_cast<char>(header.colormap_type) << endl;
    cout << static_cast<char>(header.image_type) << endl;
    cout << static_cast<short>(header.colormap_origin) << endl;
    cout << static_cast<short>(header.colormap_length) << endl;
    cout << static_cast<char>(header.colormap_depth) << endl;
    cout << static_cast<short>(header.x_origin) << endl;
    cout << static_cast<short>(header.y_origin) << endl;
    cout << static_cast<short>(header.image_width) << endl;
    cout << static_cast<short>(header.image_height) << endl;
    cout << static_cast<char>(header.pixel_depth) << endl;
    cout << static_cast<char>(header.image_descriptor) << endl;*/

    return header;
};
//this function seems to work as intended
vector<vector<unsigned char>> readImageData(ifstream& file,Header& headerparam) { //reads image data at every pixel and stores what's there to a vector containing <b,g,r> vectors
    int number_of_pixels = static_cast<int>(headerparam.image_width) * static_cast<int>(headerparam.image_height); //number of pixels are found. static_cast is used to convert the width and height's variables into integers
    //cout << "Number of pixels: " << static_cast<int>(headerparam.image_width) << " x " << static_cast<int>(headerparam.image_height) << " = " << static_cast<int>(headerparam.image_width) * static_cast<int>(headerparam.image_height) << endl;
    vector<vector<unsigned char>>* datavec = new vector<vector<unsigned char>>; //allocates a vector to heap to pixel data to
    unsigned char bgr_value;
    vector<unsigned char> tempvec;
    for (int i=0 ; i<number_of_pixels ; i++) { //loops through every single pixel in the image
        for (int j=0 ; j<3 ; j++) { //reads the b,g and r values for one pixel and pushes them to a temp vector in that exact order
            file.read((char*) &bgr_value, 1);
            //cout << j << ": " << (int)bgr_value << endl;
            tempvec.push_back(bgr_value);
        }
        datavec->push_back(tempvec); //this vector is then pushed to the main data vector then cleared for the next iteration
        tempvec.clear();
    }
    return *datavec;
};

void writeHeaderToFile(ofstream& file,Header& headerparam) { //writes all the header's data to the binary file. confirmed working
    file.write((char*) &headerparam.id_length, 1);
    file.write((char*) &headerparam.colormap_type, 1);
    file.write((char*) &headerparam.image_type,1);
    file.write((char*) &headerparam.colormap_origin,2);
    file.write((char*) &headerparam.colormap_length, 2);
    file.write((char*) &headerparam.colormap_depth, 1);
    file.write((char*) &headerparam.x_origin, 2);
    file.write((char*) &headerparam.y_origin, 2);
    file.write((char*) &headerparam.image_width, 2);
    file.write((char*) &headerparam.image_height, 2);
    file.write((char*) &headerparam.pixel_depth, 1);
    file.write((char*) &headerparam.image_descriptor, 1);
};

void writeImageDataToFile(ofstream& file, vector<vector<unsigned char>>& imageData) { //writes all the image data to the binary file
    for (int i=0 ; i<imageData.size() ; i++) { //iterates through each pixel
        //cout << "iterating through pixel..." << endl;
        for (int j=0 ; j<3 ; j++) { //writes each pixel's b,g and r value to the file
            file.write((char*) &imageData[i][j],1);
            //cout << j << ": " << (int)imageData[i][j] << endl;
        }
    }
};

vector<vector<unsigned char>> Multiply(vector<vector<unsigned char>> file1data , vector<vector<unsigned char>> file2data) {

    vector<vector<unsigned char>>* BinaryFiledatavec = new vector<vector<unsigned char>>; //vector that will hold the new file's image data

    vector<unsigned char> tempvec;

    float normalized_rgbvalue_file1; //variables created for the normalized values of each pixel
    float normalized_rgbvalue_file2;

    for (int i=0 ; i<file1data.size() ; i++) { //iterates based on the number of nested vectors in the vector
        for (int j=0 ; j<3 ; j++) { //looks through the vector on the current iteration
            //operations are done to the current rgb value to normalize them
            normalized_rgbvalue_file1 = (float)file1data[i][j] / 255.0f;
            normalized_rgbvalue_file2 = (float)file2data[i][j] / 255.0f;

            float product = normalized_rgbvalue_file1 * normalized_rgbvalue_file2;
            product*=255;
            product+=0.5f;

            unsigned char final_val = static_cast<unsigned char>(product);

            tempvec.push_back(final_val);
        }
        BinaryFiledatavec->push_back(tempvec);
        tempvec.clear();
    }

    return *BinaryFiledatavec;
};

vector<vector<unsigned char>> Subtract(vector<vector<unsigned char>> file1data , vector<vector<unsigned char>> file2data) {

    vector<vector<unsigned char>>* BinaryFiledatavec = new vector<vector<unsigned char>>; //vector that will hold the new file's image data

    vector<unsigned char> tempvec;


    for (int i=0 ; i<file1data.size() ; i++) { //iterates based on the number of nested vectors in the vector
        for (int j=0 ; j<3 ; j++) { //looks through the vector on the current iteration
            //operations are done to the current rgb value to normalize them
            int difference = static_cast<int>(file1data[i][j]) - static_cast<int>(file2data[i][j]); //gets the difference of the current color value

            if (difference > 255) { //clamps the value if it's greater than 255 or less than 0
                difference = 255;
            }
            else if (difference < 0) {
                difference = 0;
            }

            unsigned char final_val = static_cast<unsigned char>(difference); //converts it back to an unsigned char

            tempvec.push_back(final_val);
        }
        BinaryFiledatavec->push_back(tempvec);
        tempvec.clear();
    }

    return *BinaryFiledatavec;
};

vector<vector<unsigned char>> Screen(vector<vector<unsigned char>> file1data , vector<vector<unsigned char>> file2data) {

    vector<vector<unsigned char>>* BinaryFiledatavec = new vector<vector<unsigned char>>; //vector that will hold the new file's image data

    vector<unsigned char> tempvec;

    float normalized_rgbvalue_file1; //variables created for the normalized values of each pixel
    float normalized_rgbvalue_file2;

    for (int i=0 ; i<file1data.size() ; i++) { //iterates based on the number of nested vectors in the vector
        for (int j=0 ; j<3 ; j++) { //looks through the vector on the current iteration
            //operations are done to the current rgb value to normalize them
            normalized_rgbvalue_file1 = (float)file1data[i][j] / 255.0f;
            normalized_rgbvalue_file2 = (float)file2data[i][j] / 255.0f;

            float screenvalue = (1-normalized_rgbvalue_file1)*(1-normalized_rgbvalue_file2);
            screenvalue = 1-screenvalue;
            screenvalue*=255;
            screenvalue+=0.5f;

            unsigned char final_val = static_cast<unsigned char>(screenvalue);

            tempvec.push_back(final_val);
        }
        BinaryFiledatavec->push_back(tempvec);
        tempvec.clear();
    }
    return *BinaryFiledatavec; //returns the resulting binary data vector
};

vector<vector<unsigned char>> Addition(vector<vector<unsigned char>> file1data , vector<vector<unsigned char>> file2data) {

    vector<vector<unsigned char>>* BinaryFiledatavec = new vector<vector<unsigned char>>; //vector that will hold the new file's image data

    vector<unsigned char> tempvec;


    for (int i=0 ; i<file1data.size() ; i++) { //iterates based on the number of nested vectors in the vector
        for (int j=0 ; j<3 ; j++) { //looks through the vector on the current iteration
            //operations are done to the current rgb value to normalize them
            int difference = static_cast<int>(file1data[i][j]) + static_cast<int>(file2data[i][j]); //gets the difference of the current color value

            if (difference > 255) { //clamps the value if it's greater than 255 or less than 0
                difference = 255;
            }
            else if (difference < 0) {
                difference = 0;
            }

            unsigned char final_val = static_cast<unsigned char>(difference); //converts it back to an unsigned char

            tempvec.push_back(final_val);
        }
        BinaryFiledatavec->push_back(tempvec);
        tempvec.clear();
    }

    return *BinaryFiledatavec;
};

vector<vector<unsigned char>> Overlay(vector<vector<unsigned char>> file1data , vector<vector<unsigned char>> file2data) {

    vector<vector<unsigned char>>* BinaryFiledatavec = new vector<vector<unsigned char>>; //vector that will hold the new file's image data

    vector<unsigned char> tempvec;

    float normalized_rgbvalue_file1; //variables created for the normalized values of each pixel
    float normalized_rgbvalue_file2;

    for (int i=0 ; i<file1data.size() ; i++) { //iterates based on the number of nested vectors in the vector
        for (int j=0 ; j<3 ; j++) { //looks through the vector on the current iteration
            //operations are done to the current rgb value to normalize them
            normalized_rgbvalue_file1 = (float)file1data[i][j] / 255.0f;
            normalized_rgbvalue_file2 = (float)file2data[i][j] / 255.0f;

            float overlayvalue;
            if (normalized_rgbvalue_file2 <= 0.5) {
                overlayvalue = 2*normalized_rgbvalue_file1*normalized_rgbvalue_file2;
            }
            else if (normalized_rgbvalue_file2 > 0.5) {
                overlayvalue = 2*(1-normalized_rgbvalue_file1)*(1-normalized_rgbvalue_file2);
                overlayvalue = 1 - overlayvalue;
            }

            overlayvalue*=255;
            overlayvalue+=0.5f;

            unsigned char final_val = static_cast<unsigned char>(overlayvalue);

            tempvec.push_back(final_val);
        }
        BinaryFiledatavec->push_back(tempvec);
        tempvec.clear();
    }
    return *BinaryFiledatavec; //returns the resulting binary data vector
};

int main(int argc, char* argv[]) {
    vector<vector<unsigned char>> tracking_image_data;
    Header tracking_image_header;
    string tracking_image_name;

    int current_argument_position=3;
    while (true) {
        //first check if the number of arguments is empty or the help command is used

        if (argc<2) { //checks if there is only 1 argument passed. if so, print the help message
            cout << "Project 2: Image Processing, Fall 2024" << endl << endl << "Usage:" << endl << "\t./project2.out [output] [firstImage] [method] [...]" << endl;
            return 0;
        }
        else if (strcmp(argv[1],"--help")==0) { //compares the first argument to see if it's --help. if it's help, the value will be 0
            cout << "Project 2: Image Processing, Fall 2024" << endl << endl << "Usage:" << endl << "\t./project2.out [output] [firstImage] [method] [...]" << endl;
            return 0;
        }
        if (argc==2) { //checks if only the output file name argument is present, that being the output file name
            cout << "Invalid file name." << endl;
            return 1;
        }
        bool Argument_is_valid = static_cast<string>(argv[1]).find(".tga")!=string::npos; //checks whether or not the first argument is a tga file name containing the .tga extension
        if (Argument_is_valid==false) { //if it doesnt contain .tga, print invalid.
            cout << "Invalid file name." << endl;
            return 1;
        }
        else {
            tracking_image_name = static_cast<string>(argv[1]);
        }

        Argument_is_valid = static_cast<string>(argv[2]).find(".tga")!=string::npos; //checks the next argument, which will be the source file for the tracking image
        if (Argument_is_valid==false) { //if it doesnt contain .tga, print invalid.
            cout << "Invalid file name." << endl;
            return 1;
        }
        ifstream SourceFile(static_cast<string>(argv[2]),ios::binary);
        if (!SourceFile.is_open()) { //if the source file does not open, print an error.
            cout << "File does not exist." << endl;
            return 1;
        }
        else { //otherwise, load the header and image data from the source file
            tracking_image_header = getHeader(SourceFile);
            tracking_image_data = readImageData(SourceFile,tracking_image_header);
            //writeImageDataToFile(TrackingImageFile,tracking_image_data);

        }


        //once the file is created and the source file is open, now comes time for the methods
        while (argv[current_argument_position]!=nullptr) { //loops through all available arguments until a nullptr is reached
            //cout << current_argument_position << endl;
            if (static_cast<string>(argv[current_argument_position])=="multiply") {
                if (argv[current_argument_position+1]==nullptr) {
                    cout << "Missing argument." << endl;
                    return 1;
                }
                Argument_is_valid = static_cast<string>(argv[current_argument_position+1]).find(".tga")!=string::npos; //checks the next argument, which will be the source file for the tracking image
                if (Argument_is_valid==false) { //if it doesnt contain .tga, print invalid.
                    cout << "Invalid argument, invalid file name." << endl;
                    return 1;
                }
                ifstream OtherFile(static_cast<string>(argv[current_argument_position+1]),ios::binary);
                if (!OtherFile.is_open()) { //if the source file does not open, print an error.
                    cout << "Invalid argument, file does not exist." << endl;
                    return 1;
                }
                if (OtherFile.is_open()) {
                    Header header = getHeader(OtherFile);
                    vector<vector<unsigned char>> otherfile_data = readImageData(OtherFile,header);
                    tracking_image_data = Multiply(tracking_image_data,otherfile_data);

                    //writeImageDataToFile(TrackingImageFile,tracking_image_data);
                    current_argument_position+=2;
                    //cout << current_argument_position << endl;
                }
            }

            else if (static_cast<string>(argv[current_argument_position])=="subtract") {
                if (argv[current_argument_position+1]==nullptr) {
                    cout << "Missing argument." << endl;
                    return 1;
                }
                Argument_is_valid = static_cast<string>(argv[current_argument_position+1]).find(".tga")!=string::npos; //checks the next argument, which will be the source file for the tracking image
                if (Argument_is_valid==false) { //if it doesnt contain .tga, print invalid.
                    cout << "Invalid argument, invalid file name." << endl;
                    return 1;
                }
                ifstream OtherFile(static_cast<string>(argv[current_argument_position+1]),ios::binary);
                if (!OtherFile.is_open()) { //if the source file does not open, print an error.
                    cout << "Invalid argument, file does not exist." << endl;
                    return 1;
                }
                if (OtherFile.is_open()) {
                    Header header = getHeader(OtherFile);
                    vector<vector<unsigned char>> otherfile_data = readImageData(OtherFile,header);
                    tracking_image_data = Subtract(tracking_image_data,otherfile_data);

                    //writeImageDataToFile(TrackingImageFile,tracking_image_data);
                    current_argument_position+=2;
                    //cout << current_argument_position << endl;
                }
            }

            else if (static_cast<string>(argv[current_argument_position])=="overlay") {
                if (argv[current_argument_position+1]==nullptr) {
                    cout << "Missing argument." << endl;
                    return 1;
                }
                Argument_is_valid = static_cast<string>(argv[current_argument_position+1]).find(".tga")!=string::npos; //checks the next argument, which will be the source file for the tracking image
                if (Argument_is_valid==false) { //if it doesnt contain .tga, print invalid.
                    cout << "Invalid argument, invalid file name." << endl;
                    return 1;
                }
                ifstream OtherFile(static_cast<string>(argv[current_argument_position+1]),ios::binary);
                if (!OtherFile.is_open()) { //if the source file does not open, print an error.
                    cout << "Invalid argument, file does not exist." << endl;
                    return 1;
                }
                if (OtherFile.is_open()) {
                    Header header = getHeader(OtherFile);
                    vector<vector<unsigned char>> otherfile_data = readImageData(OtherFile,header);
                    tracking_image_data = Overlay(tracking_image_data,otherfile_data);

                    //writeImageDataToFile(TrackingImageFile,tracking_image_data);
                    current_argument_position+=2;
                    //cout << current_argument_position << endl;
                }
            }

            else if (static_cast<string>(argv[current_argument_position])=="screen") {
                if (argv[current_argument_position+1]==nullptr) {
                    cout << "Missing argument." << endl;
                    return 1;
                }
                Argument_is_valid = static_cast<string>(argv[current_argument_position+1]).find(".tga")!=string::npos; //checks the next argument, which will be the source file for the tracking image
                if (Argument_is_valid==false) { //if it doesnt contain .tga, print invalid.
                    cout << "Invalid argument, invalid file name." << endl;
                    return 1;
                }
                ifstream OtherFile(static_cast<string>(argv[current_argument_position+1]),ios::binary);
                if (!OtherFile.is_open()) { //if the source file does not open, print an error.
                    cout << "Invalid argument, file does not exist." << endl;
                    return 1;
                }
                if (OtherFile.is_open()) {
                    Header header = getHeader(OtherFile);
                    vector<vector<unsigned char>> otherfile_data = readImageData(OtherFile,header);
                    tracking_image_data = Screen(otherfile_data,tracking_image_data);

                    //writeImageDataToFile(TrackingImageFile,tracking_image_data);
                    current_argument_position+=2;
                    //cout << current_argument_position << endl;
                }
            }

            else if (static_cast<string>(argv[current_argument_position])=="combine") {
                vector<vector<unsigned char>> greendata;
                vector<vector<unsigned char>> bluedata;

                //first check following argument to get data from a file that will be used for green values
                if (argv[current_argument_position+1]==nullptr) {
                    cout << "Missing argument." << endl;
                    return 1;
                }
                Argument_is_valid = static_cast<string>(argv[current_argument_position+1]).find(".tga")!=string::npos; //checks the next argument, which will be the source file for the tracking image
                if (Argument_is_valid==false) { //if it doesnt contain .tga, print invalid.
                    cout << "Invalid argument, invalid file name." << endl;
                    return 1;
                }
                ifstream OtherFile(static_cast<string>(argv[current_argument_position+1]),ios::binary);
                if (!OtherFile.is_open()) { //if the source file does not open, print an error.
                    cout << "Invalid argument, file does not exist." << endl;
                    return 1;
                }
                if (OtherFile.is_open()) {
                    Header header = getHeader(OtherFile);
                    greendata = readImageData(OtherFile,header);
                    current_argument_position+=2; //argument position gets moved to the next file to be read
                }

                //next, check arguments for blue pixel data
                if (argv[current_argument_position]==nullptr) {
                    cout << "Missing argument." << endl;
                    return 1;
                }
                Argument_is_valid = static_cast<string>(argv[current_argument_position]).find(".tga")!=string::npos; //checks the next argument, which will be the source file for the tracking image
                if (Argument_is_valid==false) { //if it doesnt contain .tga, print invalid.
                    cout << "Invalid argument, invalid file name." << endl;
                    return 1;
                }
                ifstream BlueFile(static_cast<string>(argv[current_argument_position]),ios::binary);
                if (!BlueFile.is_open()) { //if the source file does not open, print an error.
                    cout << "Invalid argument, file does not exist." << endl;
                    return 1;
                }
                if (BlueFile.is_open()) {
                    Header header = getHeader(BlueFile);
                    bluedata = readImageData(BlueFile,header);
                }

                //with all the files read and data stored, it's time to combine them
                vector<vector<unsigned char>> BinaryFileVec;
                vector<unsigned char> tempvec;

                for (int i=0 ; i<bluedata.size() ; i++) {
                    tempvec.push_back(bluedata[i][0]);
                    tempvec.push_back(greendata[i][1]);
                    tempvec.push_back(tracking_image_data[i][2]);
                    BinaryFileVec.push_back(tempvec);
                    tempvec.clear();
                }
                tracking_image_data = BinaryFileVec;
                current_argument_position+=1; //current position moves up 1 once the other argument file is read
            }

            else if (static_cast<string>(argv[current_argument_position])=="flip") {
                reverse(tracking_image_data.begin(),tracking_image_data.end());
                current_argument_position+=1;
            }

            else if (static_cast<string>(argv[current_argument_position])=="onlyred") {
                vector<vector<unsigned char>> NewBinaryFileVec;
                vector<unsigned char> tempvec;

                unsigned char channelval;
                for (int i=0 ; i<tracking_image_data.size() ; i++) { //iterates based on the number of nested vectors in the vector
                    channelval = tracking_image_data[i][2];
                    for (int j=0 ; j<3 ; j++) { //adds the same pixel color value three times to the tempvec
                        tempvec.push_back(channelval);
                    }
                    NewBinaryFileVec.push_back(tempvec);
                    tempvec.clear();
                }

                tracking_image_data=NewBinaryFileVec;
                current_argument_position+=1;
            }

            else if (static_cast<string>(argv[current_argument_position])=="onlyblue") {
                vector<vector<unsigned char>> NewBinaryFileVec;
                vector<unsigned char> tempvec;

                unsigned char channelval;
                for (int i=0 ; i<tracking_image_data.size() ; i++) { //iterates based on the number of nested vectors in the vector
                    channelval = tracking_image_data[i][0];
                    for (int j=0 ; j<3 ; j++) { //adds the same pixel color value three times to the tempvec
                        tempvec.push_back(channelval);
                    }
                    NewBinaryFileVec.push_back(tempvec);
                    tempvec.clear();
                }

                tracking_image_data=NewBinaryFileVec;
                current_argument_position+=1;
            }

            else if (static_cast<string>(argv[current_argument_position])=="onlygreen") {
                vector<vector<unsigned char>> NewBinaryFileVec;
                vector<unsigned char> tempvec;

                unsigned char channelval;
                for (int i=0 ; i<tracking_image_data.size() ; i++) { //iterates based on the number of nested vectors in the vector
                    channelval = tracking_image_data[i][1];
                    for (int j=0 ; j<3 ; j++) { //adds the same pixel color value three times to the tempvec
                        tempvec.push_back(channelval);
                    }
                    NewBinaryFileVec.push_back(tempvec);
                    tempvec.clear();
                }

                tracking_image_data=NewBinaryFileVec;
                current_argument_position+=1;
            }

            else if (static_cast<string>(argv[current_argument_position])=="addblue") {
                if (argv[current_argument_position+1]==nullptr) {
                    cout << "Missing argument." << endl;
                    return 1;
                }

                int numtoadd;
                try { //will check if the following argument is a valid integer
                    numtoadd = stoi(argv[current_argument_position+1]);
                }
                catch (std::invalid_argument) {
                    cout << "Invalid argument, expected number." << endl;
                    return 1;
                }

                vector<vector<unsigned char>> NewBinaryFileVec;
                vector<unsigned char> tempvec;
                int add;
                for (int i=0 ; i<tracking_image_data.size() ; i++) { //iterates based on the number of nested vectors in the vector
                    for (int j=0 ; j<3 ; j++) { //looks through the vector on the current iteration
                        if (j==0) { //will only add 200 to the current data bit if it's the green one
                            add = numtoadd;
                        }
                        else {add = 0;}

                        int difference = static_cast<int>(tracking_image_data[i][j]) + add;

                        if (difference > 255) { //clamps the value if it's greater than 255 or less than 0
                            difference = 255;
                        }
                        else if (difference < 0) {
                            difference = 0;
                        }

                        unsigned char final_val = static_cast<unsigned char>(difference); //converts it back to an unsigned char

                        tempvec.push_back(final_val);
                    }
                    NewBinaryFileVec.push_back(tempvec);
                    tempvec.clear();
                }

                tracking_image_data = NewBinaryFileVec;
                current_argument_position+=2;
            }

            else if (static_cast<string>(argv[current_argument_position])=="addgreen") {
                if (argv[current_argument_position+1]==nullptr) {
                    cout << "Missing argument." << endl;
                    return 1;
                }

                int numtoadd;
                try { //will check if the following argument is a valid integer
                    numtoadd = stoi(argv[current_argument_position+1]);
                }
                catch (std::invalid_argument) {
                    cout << "Invalid argument, expected number." << endl;
                    return 1;
                }

                vector<vector<unsigned char>> NewBinaryFileVec;
                vector<unsigned char> tempvec;
                int add;
                for (int i=0 ; i<tracking_image_data.size() ; i++) { //iterates based on the number of nested vectors in the vector
                    for (int j=0 ; j<3 ; j++) { //looks through the vector on the current iteration
                        if (j==1) { //will only add 200 to the current data bit if it's the green one
                            add = numtoadd;
                        }
                        else {add = 0;}

                        int difference = static_cast<int>(tracking_image_data[i][j]) + add;

                        if (difference > 255) { //clamps the value if it's greater than 255 or less than 0
                            difference = 255;
                        }
                        else if (difference < 0) {
                            difference = 0;
                        }

                        unsigned char final_val = static_cast<unsigned char>(difference); //converts it back to an unsigned char

                        tempvec.push_back(final_val);
                    }
                    NewBinaryFileVec.push_back(tempvec);
                    tempvec.clear();
                }

                tracking_image_data = NewBinaryFileVec;
                current_argument_position+=2;
            }

            else if (static_cast<string>(argv[current_argument_position])=="addred") {
                if (argv[current_argument_position+1]==nullptr) {
                    cout << "Missing argument." << endl;
                    return 1;
                }

                int numtoadd;
                try { //will check if the following argument is a valid integer
                    numtoadd = stoi(argv[current_argument_position+1]);
                }
                catch (std::invalid_argument) {
                    cout << "Invalid argument, expected number." << endl;
                    return 1;
                }

                vector<vector<unsigned char>> NewBinaryFileVec;
                vector<unsigned char> tempvec;
                int add;
                for (int i=0 ; i<tracking_image_data.size() ; i++) { //iterates based on the number of nested vectors in the vector
                    for (int j=0 ; j<3 ; j++) { //looks through the vector on the current iteration
                        if (j==2) { //will only add 200 to the current data bit if it's the green one
                            add = numtoadd;
                        }
                        else {add = 0;}

                        int difference = static_cast<int>(tracking_image_data[i][j]) + add;

                        if (difference > 255) { //clamps the value if it's greater than 255 or less than 0
                            difference = 255;
                        }
                        else if (difference < 0) {
                            difference = 0;
                        }

                        unsigned char final_val = static_cast<unsigned char>(difference); //converts it back to an unsigned char

                        tempvec.push_back(final_val);
                    }
                    NewBinaryFileVec.push_back(tempvec);
                    tempvec.clear();
                }

                tracking_image_data = NewBinaryFileVec;
                current_argument_position+=2;
            }

            else if (static_cast<string>(argv[current_argument_position])=="scaleblue") {
                if (argv[current_argument_position+1]==nullptr) {
                    cout << "Missing argument." << endl;
                    return 1;
                }

                int numtomultiply;
                try { //will check if the following argument is a valid integer
                    numtomultiply = stoi(argv[current_argument_position+1]);
                }
                catch (std::invalid_argument) {
                    cout << "Invalid argument, expected number." << endl;
                    return 1;
                }

                vector<vector<unsigned char>> NewBinaryFileVec;
                vector<unsigned char> tempvec;

                int scale;
                float normalized_rgbvalue_file1; //variables created for the normalized values of each pixel

                for (int i=0 ; i<tracking_image_data.size() ; i++) { //iterates based on the number of nested vectors in the vector
                    for (int j=0 ; j<3 ; j++) { //looks through the vector on the current iteration
                        //operations are done to the current rgb value to normalize them
                        normalized_rgbvalue_file1 = (float)tracking_image_data[i][j] / 255.0f;

                        if (j == 0) { //checks the current color type and determines what the scale will be
                            scale = numtomultiply;
                        }
                        else {scale = 1;}

                        float product = normalized_rgbvalue_file1 * scale;
                        product*=255;
                        product+=0.5f;

                        if (product > 255) { //clamps the value if it's greater than 255 or less than 0
                            product = 255;
                        }
                        else if (product < 0) {
                            product = 0;
                        }

                        unsigned char final_val = static_cast<unsigned char>(product);

                        tempvec.push_back(final_val);
                    }
                    NewBinaryFileVec.push_back(tempvec);
                    tempvec.clear();
                }

                tracking_image_data = NewBinaryFileVec;
                current_argument_position+=2;
            }

            else if (static_cast<string>(argv[current_argument_position])=="scalegreen") {
                if (argv[current_argument_position+1]==nullptr) {
                    cout << "Missing argument." << endl;
                    return 1;
                }

                int numtomultiply;
                try { //will check if the following argument is a valid integer
                    numtomultiply = stoi(argv[current_argument_position+1]);
                }
                catch (std::invalid_argument) {
                    cout << "Invalid argument, expected number." << endl;
                    return 1;
                }

                vector<vector<unsigned char>> NewBinaryFileVec;
                vector<unsigned char> tempvec;

                int scale;
                float normalized_rgbvalue_file1; //variables created for the normalized values of each pixel

                for (int i=0 ; i<tracking_image_data.size() ; i++) { //iterates based on the number of nested vectors in the vector
                    for (int j=0 ; j<3 ; j++) { //looks through the vector on the current iteration
                        //operations are done to the current rgb value to normalize them
                        normalized_rgbvalue_file1 = (float)tracking_image_data[i][j] / 255.0f;

                        if (j == 1) { //checks the current color type and determines what the scale will be
                            scale = numtomultiply;
                        }
                        else {scale = 1;}

                        float product = normalized_rgbvalue_file1 * scale;
                        product*=255;
                        product+=0.5f;

                        if (product > 255) { //clamps the value if it's greater than 255 or less than 0
                            product = 255;
                        }
                        else if (product < 0) {
                            product = 0;
                        }

                        unsigned char final_val = static_cast<unsigned char>(product);

                        tempvec.push_back(final_val);
                    }
                    NewBinaryFileVec.push_back(tempvec);
                    tempvec.clear();
                }

                tracking_image_data = NewBinaryFileVec;
                current_argument_position+=2;
            }

            else if (static_cast<string>(argv[current_argument_position])=="scalered") {
                if (argv[current_argument_position+1]==nullptr) {
                    cout << "Missing argument." << endl;
                    return 1;
                }

                int numtomultiply;
                try { //will check if the following argument is a valid integer
                    numtomultiply = stoi(argv[current_argument_position+1]);
                }
                catch (std::invalid_argument) {
                    cout << "Invalid argument, expected number." << endl;
                    return 1;
                }

                vector<vector<unsigned char>> NewBinaryFileVec;
                vector<unsigned char> tempvec;

                int scale;
                float normalized_rgbvalue_file1; //variables created for the normalized values of each pixel

                for (int i=0 ; i<tracking_image_data.size() ; i++) { //iterates based on the number of nested vectors in the vector
                    for (int j=0 ; j<3 ; j++) { //looks through the vector on the current iteration
                        //operations are done to the current rgb value to normalize them
                        normalized_rgbvalue_file1 = (float)tracking_image_data[i][j] / 255.0f;

                        if (j == 2) { //checks the current color type and determines what the scale will be
                            scale = numtomultiply;
                        }
                        else {scale = 1;}

                        float product = normalized_rgbvalue_file1 * scale;
                        product*=255;
                        product+=0.5f;

                        if (product > 255) { //clamps the value if it's greater than 255 or less than 0
                            product = 255;
                        }
                        else if (product < 0) {
                            product = 0;
                        }

                        unsigned char final_val = static_cast<unsigned char>(product);

                        tempvec.push_back(final_val);
                    }
                    NewBinaryFileVec.push_back(tempvec);
                    tempvec.clear();
                }

                tracking_image_data = NewBinaryFileVec;
                current_argument_position+=2;
            }

            else {
                cout << "Invalid method name." << endl;
                current_argument_position+=1;
                return 1;
            }
        }

        //When the loop exhausts and all arguments have succesfully been read, create the file and write the final data to it.
        ofstream TrackingImageFile;
        TrackingImageFile.open(tracking_image_name,ios::binary); //creates a new file using the previously specified name
        writeHeaderToFile(TrackingImageFile,tracking_image_header);
        writeImageDataToFile(TrackingImageFile,tracking_image_data);
        return 0;
    }
};