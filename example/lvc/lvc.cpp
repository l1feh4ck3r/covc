/*
 * Copyright (c) 2010 Alexey 'l1feh4ck3r' Antonov
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <iostream>
#include <fstream>
using namespace std;

#include "pictureinfo.h"

///////////////////////////////////////////////////////////////////////////////
// Function definitions.
///////////////////////////////////////////////////////////////////////////////
int load_jpg(const std::string & file_name);


///////////////////////////////////////////////////////////////////////////////
//! Main function
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char * argv[])
{
    if (argc < 3)
    {
        cout << "Wrong number of parameters!" << endl;
        cout << "Usage:" << endl;
        cout << "\t" << argv[0] << " meta_file_name output_file_name" << endl;
        return 1;
    }

    // 1. load all info
    ifstream meta_file;
    meta_file.open(argv[1]);
    if ( !meta_file.is_open() )
    {
        cerr << "Can't open file \"" << argv[1] << "\"!" << endl;
        return 1;
    }

    size_t image_number;
    meta_file >> image_number;

    meta_file.close();

    // 2. calculate bounding box
    // 3. color voxels
    // 4. save resulting voxel cube

    return 0;
}
