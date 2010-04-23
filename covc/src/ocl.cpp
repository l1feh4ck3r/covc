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

#include "ocl.h"

#include <iostream>

Ocl::Ocl()
{
}

bool Ocl::build_program(const std::string & path_to_file_with_program)
{
    bool result = false;

    try
    {
        ocl_program = ocl_context->createProgramFromFile(path_to_file_with_program);

        result = ocl_program->buildProgram("-cl-mad-enable");

        if (!result)
            std::cerr << ocl_program->getBuildLog() << std::endl;
    }
    catch(Exception ex)
    {
        result = false;
    }

    return result;
}

bool Ocl::prepare_opencl()
{
    bool result = false;

    try
    {
        if (!ocl_context.use_count())
            ocl_context = Context::createContext(Device::GPU);
        result = true;
    }
    catch(Exception ex)
    {
        std::cerr << ex.what() << std::endl;
        result = false;
    }

    return result;
}
