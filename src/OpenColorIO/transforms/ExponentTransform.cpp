/*
Copyright (c) 2003-2010 Sony Pictures Imageworks Inc., et al.
All Rights Reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:
* Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.
* Neither the name of Sony Pictures Imageworks nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <cstring>

#include <OpenColorIO/OpenColorIO.h>

#include "ops/Exponent/ExponentOps.h"
#include "OpBuilders.h"


OCIO_NAMESPACE_ENTER
{
    ExponentTransformRcPtr ExponentTransform::Create()
    {
        return ExponentTransformRcPtr(new ExponentTransform(), &deleter);
    }
    
    void ExponentTransform::deleter(ExponentTransform* t)
    {
        delete t;
    }
    
    
    class ExponentTransform::Impl : public ExponentOpData
    {
    public:
        TransformDirection dir_;
        
        Impl() :
            ExponentOpData(),
            dir_(TRANSFORM_DIR_FORWARD)
        { }
        
        ~Impl()
        { }
        
        Impl& operator= (const Impl & rhs)
        {
            if (this != &rhs)
            {
                ExponentOpData::operator=(rhs);
                dir_ = rhs.dir_;
            }
            return *this;
        }

    private:        
        Impl(const Impl & rhs);
    };
    
    ///////////////////////////////////////////////////////////////////////////
    
    
    
    ExponentTransform::ExponentTransform()
        : m_impl(new ExponentTransform::Impl)
    {
    }
    
    TransformRcPtr ExponentTransform::createEditableCopy() const
    {
        ExponentTransformRcPtr transform = ExponentTransform::Create();
        *(transform->m_impl) = *m_impl;
        return transform;
    }
    
    ExponentTransform::~ExponentTransform()
    {
        delete m_impl;
        m_impl = NULL;
    }
    
    ExponentTransform& ExponentTransform::operator= (const ExponentTransform & rhs)
    {
        if (this != &rhs)
        {
            *m_impl = *rhs.m_impl;
        }
        return *this;
    }
    
    TransformDirection ExponentTransform::getDirection() const
    {
        return getImpl()->dir_;
    }
    
    void ExponentTransform::setDirection(TransformDirection dir)
    {
        getImpl()->dir_ = dir;
    }
    
    void ExponentTransform::validate() const
    {
        Transform::validate();

        // TODO: Uncomment in upcoming PR that contains the OpData validate.
        //       getImpl()->data_->validate()
        //       
        //       OpData classes are the enhancement of some existing 
        //       structures (like Lut1D and Lut3D) by encapsulating
        //       all the data and adding high-level behaviors.
    }

    void ExponentTransform::setValue(const float * vec4)
    {
        if(vec4)
        {
            for(unsigned i=0; i<4; ++i)
            {
                getImpl()->m_exp4[i] = vec4[i];
            }
        }
    }
    
    void ExponentTransform::getValue(float * vec4) const
    {
        if(vec4)
        {
            for(unsigned i=0; i<4; ++i)
            {
                vec4[i] = float(getImpl()->m_exp4[i]);
            }
        }
    }
    
    std::ostream& operator<< (std::ostream& os, const ExponentTransform& t)
    {
        float exp[4];
        t.getValue(exp);

        os << "<ExponentTransform ";
        os << "direction=" << TransformDirectionToString(t.getDirection()) << ", ";
        os << "value=" << exp[0];
        for (int i = 1; i < 4; ++i)
        {
          os << " " << exp[i];
        }

        os << ">";
        return os;
    }
    
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    
    void BuildExponentOps(OpRcPtrVec & ops,
                          const Config& /*config*/,
                          const ExponentTransform & transform,
                          TransformDirection dir)
    {
        TransformDirection combinedDir = CombineTransformDirections(dir,
            transform.getDirection());
        
        float vec4[4];
        transform.getValue(vec4);
        
        CreateExponentOp(ops,
                         vec4,
                         combinedDir);
    }
}
OCIO_NAMESPACE_EXIT
