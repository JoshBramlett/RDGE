//! \headerfile <rdge/util/memory/dynamic_component.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 04/28/2017

#pragma once

#include <rdge/core.hpp>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

// FIXME This isn't the correct implementation of Insomniac's DynamicComponent.
//       It's a copy/paste from the SO article
//
//       http://stackoverflow.com/questions/19385853
//       https://d3cw3dd2w32x2b.cloudfront.net/wp-content/uploads/2011/06/6-1-2010.pdf
//
//template<typename T, int SIZE>
//class ResourceManager
//{
    //T data[SIZE];
    //int indices[SIZE];
    //int back;

    //ResourceManager() : back(0)
    //{
        //for(int i=0; i<SIZE; i++)
            //indices[i] = i;
    //}

    //int Reserve()
    //{ return indices[back++]; }

    //void Release(int handle)
    //{
        //for(int i=0; i<back; i++)
        //{
            //if(indices[i] == handle)
            //{
                //back--;
                //std::swap(indices[i], indices[back]);
                //return;
            //}
        //}
    //}

    //T GetData(int handle)
    //{ return data[handle]; }
//};

} // namespace rdge
