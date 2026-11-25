#include "GPU.h"
#include <stdexcept>

GPU::GPU(size_t blocksNum, size_t blockSize): blocksNum(blocksNum), blockSize(blockSize), memory(blocksNum, nullptr), 
totalReserved(0), requestReservations(), cacheHits(0), evictions(0), slotPrefixes() {}

void GPU::addMemoryBlock(const GenerativeRequest& request, size_t slot) {
    memory[slot] = new KVBlock(blockSize);
}

void GPU::addMemoryBlock(const EmbeddingRequest& request, size_t slot) {
    memory[slot] = new EmbeddingWorkspace(blockSize);
}

void GPU::reserve(size_t n, const Request& request) {
    if(n > availableSlots())
        throw std::runtime_error("Not enough available slots");

    if(n == 0) 
        return;
    totalReserved = totalReserved + n;
    requestReservations[request.getId()] += n;
}

void GPU::unreserve(size_t n, const Request& request) {
    if(n == 0)
        return;
    
    auto it = requestReservations.find(request.getId());
    if(it == requestReservations.end() || n > it->second || n > totalReserved)
        throw std::runtime_error("Invalid unreserve");

    totalReserved = totalReserved - n;
    it->second = it->second - n;

    if(it->second == 0)
        requestReservations.erase(it);
}

size_t GPU::availableSlots() const {
    size_t activeSlots = 0;
    for(MemoryBlock* mptr : memory) {
        if(mptr != nullptr && mptr->getRefCount() > 0)
            activeSlots++;
    }

    return blocksNum - activeSlots - totalReserved;
}

size_t GPU::allocateBlock(const Request& request) {
    auto it = requestReservations.find(request.getId());

    //first make sure the request reserved a block
    if (it == requestReservations.end() || it->second < 1) {
        throw std::runtime_error("Not enough reserved blocks");
    }


    //look for an empty slot
    std::size_t lowestEmptyIndex = blocksNum;
    std::size_t lowestIdleIndex = blocksNum;
    for (std::size_t i = 0; i < blocksNum; i++) {
        if (memory[i] == nullptr) {
            lowestEmptyIndex = i;
            break;
        }
        if (memory[i]->getRefCount() == 0 && lowestIdleIndex > i) {
            lowestIdleIndex = i;
        }
    }


    //check for empty slot
    std::size_t targetSlot = blocksNum;
    if (lowestEmptyIndex != blocksNum) {
        targetSlot = lowestEmptyIndex;
    } else if (lowestIdleIndex != blocksNum) {
        evict(lowestIdleIndex);
        targetSlot = lowestIdleIndex;
    } else {
        throw std::runtime_error("No slot available");
    }

    request.allocateOneBlock(*this, targetSlot);
    memory[targetSlot]->acquire();
    unreserve(1, request);
    return targetSlot;
}

void GPU::evict(std::size_t slot) {
    delete memory[slot];
    memory[slot] = nullptr;
    slotPrefixes.erase(slot);
    evictions++;
}

size_t GPU::getBlocksForMemory(size_t memory) const {
    return (blockSize + memory - 1)/blockSize;
}

void GPU::acquire(size_t slot) {
    memory[slot]->acquire();
}

void GPU::release(size_t slot) {
    IdleAction action = memory[slot]->release();
    if (action == IdleAction::KeepCached) {
        auto it = slotPrefixes.find(slot);
        if (it == slotPrefixes.end() || it->second.prefixTokens.empty()) {
            delete memory[slot];
            memory[slot] = nullptr;
            slotPrefixes.erase(it);
        }
    }
    else if(action == IdleAction::Destroy) {
        delete memory[slot];
        memory[slot] = nullptr;
    }
}

void GPU::assignPrefixToBlock(std::size_t block, const std::vector<std::string>& prefixTokens, const std::string& modelId) {
    SlotPrefix currentSlotPrefix = {modelId, prefixTokens};
    slotPrefixes[block] = currentSlotPrefix;
}