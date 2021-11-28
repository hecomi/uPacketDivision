#pragma once

#include <memory>
#include <vector>


namespace uPacketDivision
{


struct Chunk
{
    std::unique_ptr<char[]> buf = nullptr;
    uint32_t size = 0;
};


class Divider final
{
public:
    void SetMaxPacketSize(uint32_t size) { maxPacketSize_ = size; }
    void DivideData(const void *pData, uint32_t size);
    uint32_t GetChunkCount() const { return static_cast<uint32_t>(chunks_.size()); }
    uint32_t GetChunkSize(uint32_t index) const;
    const char * GetChunkData(uint32_t index) const;

private:
    uint32_t maxPacketSize_ = 1472;
    uint64_t frameIndex_ = 0;
    std::vector<Chunk> chunks_;
};


}
