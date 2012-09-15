#include "Buffer.h"
#include "Log.h"

#include <cassert>
using namespace ACache;

Buffer::Buffer():buffer_(kInitVectorSize),
		writeable_index_(0),
		readable_index_(0)
{
}

Buffer::Buffer(const Buffer& buf):buffer_(buf.buffer_),
		                  writeable_index_(buf.writeable_index_),
						  readable_index_(buf.readable_index_)
{
}

Buffer::~Buffer()
{
}

//WriteBuffer
void Buffer::WriteBuffer(char *stackbuf, size_t total, size_t writeable)
{
    if( total < 0)
    {
	    FILE_LOG(logERROR);
    }
    else if(total < writeable)
    {
        UpdateReadableSize(total);
    }
    else
    {
        //expand the buffer,and append the bytes;
	    size_t len = total - writeable;
	    ExpandCapacity(len);
	    Append(stackbuf, len);
    }
}

void Buffer::Append(const char *s, size_t len)
{
    size_t length = buffer_.size() - writeable_index_;
    if(length < len) 
    {
        ExpandCapacity(len);
    }
	    
    assert(len <= WriteableBytes());
    std::copy(s, s + len, GetWriteStart());
    writeable_index_ += len;
}

void Buffer::ExpandCapacity(size_t len)
{
    if(PrereadableBytes() + WriteableBytes() < len)	
    {
        buffer_.resize(writeable_index_ + len);
    }
    else
    {
	    std::copy(buffer_.begin() + readable_index_,
        buffer_.begin() + writeable_index_,
		buffer_.begin());
	    writeable_index_ -= readable_index_;
      	readable_index_ = 0;
    }	
}

std::string Buffer::DrainAsString()
{
    FILE_LOG(logINFO)<<readable_index_<<" "<<writeable_index_;
    std::string str(GetReadStart(), ReadableBytes());
    FILE_LOG(logINFO)<<" msg: "<<str;
    RestoreInitialState();
    return str;
}

//RestoreInitState
void Buffer::RestoreInitialState()
{
    writeable_index_ = 0;
    readable_index_ = 0;   
}
