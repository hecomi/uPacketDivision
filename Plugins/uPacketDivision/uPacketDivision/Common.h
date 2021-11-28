#pragma once

#include <cstdint>


namespace uPacketDivision
{


constexpr const char * GetVersion() { return "uPacketDivider v1.0.0"; }


constexpr uint8_t GetPacketHeaderVersion() { return 0x01; }


enum class PacketType : uint8_t
{
    Chunk = 0x01,
};


struct PacketHeader
{
    uint8_t version;
    uint8_t type;
    uint32_t totalSize;
    uint32_t offsetSize;
    uint64_t timestamp;
    uint64_t frameIndex;
    uint16_t chunkCount;
    uint16_t chunkIndex;
    uint32_t chunkSize;
};


}
