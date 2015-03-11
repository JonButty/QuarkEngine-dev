/*************************************************************************/
/*!
\file		DataFileIO.cpp
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "Simian/DataFileIO.h"
#include "Simian/Utility.h"
#include "Simian/FileObjectSubNode.h"

namespace Simian
{
    DataFileIO::DataFileIO() : 
        root_(new FileRootSubNode()),
        flags_(0)
    {
    }

    DataFileIO::~DataFileIO()
    {
        Close();
    }

    //--------------------------------------------------------------------------

    FileRootSubNode* DataFileIO::Root() const
    {
        return root_;
    }

    void DataFileIO::Root( FileRootSubNode* val )
    {
        *root_ = *val;
    }

    Simian::u8 DataFileIO::Flags() const
    {
        return flags_;
    }

    void DataFileIO::Flags( u8 val )
    {
        flags_ = val;
    }

    //--------------------------------------------------------------------------

    void DataFileIO::Close()
    {
        delete root_;
        root_ = 0;
    }
}
