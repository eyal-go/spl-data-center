#include "MemoryBlock.h"

MemoryBlock::MemoryBlock(size_t size): size(size), refCount(0) {
    data = new int[size];
} 

int MemoryBlock::readFromBlock(size_t offset) const {
    return data[offset];
}

void MemoryBlock::writeToBlock(size_t offset, int value) {
    data[offset] = value;
}

void MemoryBlock::acquire() {
    refCount++;
}

IdleAction MemoryBlock::release() {
    refCount--;
    if(refCount == 0)
       return this->onLastRelease();
    return IdleAction::KeepCached;  
}

size_t MemoryBlock::getRefCount() const { return refCount; }

//Rule of 5: Handeling memory
MemoryBlock::~MemoryBlock() {
    delete[] data;
}

MemoryBlock::MemoryBlock(const MemoryBlock& other) : size(other.size), refCount(0) {
    data = new int[size];
    for(size_t i = 0; i < size; i ++)
        data[i] = other.data[i];   
}

MemoryBlock& MemoryBlock::operator=(const MemoryBlock& other) {
    if(this != &other) {
        delete[] data;
        size = other.size;
        refCount = 0;

        data = new int[size];
        for(size_t i = 0; i < size; i++)
            data[i] = other.data[i];
    }
    return (*this);
}

MemoryBlock::MemoryBlock(MemoryBlock&& other) : size(other.size), refCount(0) {
    data = other.data;
    other.data = nullptr;
    other.size = 0;
}

MemoryBlock& MemoryBlock::operator=(MemoryBlock&& other) {
    if(this != &other) {
        delete[] data;
        size = other.size;
        refCount = 0;
        data = other.data;
        other.data = nullptr;
        other.size = 0;
    }
    return (*this);
}

//KVBlock

KVBlock::KVBlock(size_t size) : MemoryBlock(size) {}

IdleAction KVBlock::onLastRelease() const {
    return IdleAction::KeepCached;
} 

MemoryBlock* KVBlock::clone() const{
    return new KVBlock(*this);
}

//EmbeddingWorkspace

EmbeddingWorkspace::EmbeddingWorkspace(size_t size) : MemoryBlock(size) {}

IdleAction EmbeddingWorkspace::onLastRelease() const {
    return IdleAction::Destroy;
}

MemoryBlock* EmbeddingWorkspace::clone() const {
    return new EmbeddingWorkspace(*this);
}