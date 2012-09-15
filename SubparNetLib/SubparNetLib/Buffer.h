#ifndef BUFFER_H_
#define BUFFER_H_
#include <vector>
#include <string>
#include <algorithm>
#include <errno.h>
#include <stdio.h>

namespace Subpar
{
class Buffer
{
public:
    static const size_t kInitVectorSize = 4096;

    Buffer();

    ~Buffer();

    explicit Buffer(const Buffer& buf);

    void WriteBuffer(char *stackbuf, size_t total, size_t writeable);
    

    size_t PrereadableBytes()
    {
        return readable_index_;	
    }

    size_t ReadableBytes()
    {
	return  writeable_index_ - readable_index_;
    }

    size_t WriteableBytes()
    {
	return buffer_.size() - writeable_index_;
    }

    std::string DrainAsString();

    void RestoreInitialState();

    void Append(const char *s, size_t len);
    
    //the function name is not properly
    char *GetWriteStart()
    {
	return Begin() + writeable_index_;
    }

    char *GetReadStart()
    {
    	return Begin() + readable_index_;
    }
    //FIXME:UpdateReadablIndex(size_t n)
    void UpdateReadableSize(size_t n)
    {
	writeable_index_ += n;
    }
private:
    char *Begin()
    {
	return &*buffer_.begin();
    }

    void ExpandCapacity(size_t len);

private:
    std::vector<char> buffer_;
    size_t writeable_index_;
    size_t readable_index_;	
};
}
#endif
