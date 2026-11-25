#include "Request.h"

// ==================== Request ====================

Request::Request(int id, const Model& model, int admissionCycle, std::string prompt)
    : id(id), model(model), admissionCycle(admissionCycle), reservedBlocks(0) {
    (void)prompt;
}

void Request::deallocateMemory(GPU& gpu) {
    (void)gpu;
}

int Request::getId() const {
    return 0;
}

const Model& Request::getModel() const {
    return model;
}

int Request::getAdmissionCycle() const {
    return 0;
}

// ==================== GenerativeRequest ====================

GenerativeRequest::GenerativeRequest(int id, const Model& model, int admissionCycle, std::string prompt,
                                     std::string scriptedOutput, std::size_t maxOutputTokens)
    : Request(id, model, admissionCycle, prompt),
      scriptedOutput(scriptedOutput),
      maxOutputTokens(maxOutputTokens) {}

// ==================== NEW IMPLEMENTATIONS START HERE ====================
std::size_t GenerativeRequest::getNeededMemory(const GPU& gpu) const {
    (void)gpu;
    return 0;
}

std::size_t GenerativeRequest::getMaxNeededMemory() const {
    return 0;
}

std::vector<std::string> GenerativeRequest::getCacheableTokens() const {
    return {};
}

void GenerativeRequest::allocateOneBlock(GPU& gpu, std::size_t slot) const {
    (void)gpu;
    (void)slot;
}

Request* GenerativeRequest::clone(const Model& model) const {
    (void)model;
    return nullptr;
}

void GenerativeRequest::act(GPU& gpu) {
    (void)gpu;
}

bool GenerativeRequest::finished() const {
    return false;
}

void GenerativeRequest::reserveMemory(GPU& gpu) {
    (void)gpu;
}

std::string GenerativeRequest::getOutput() const {
    return "";
}

std::size_t GenerativeRequest::getNeedToUnreserve() const {
    return 0;
}

// ==================== EmbeddingRequest ====================

EmbeddingRequest::EmbeddingRequest(int id, const Model& model, int admissionCycle, std::string prompt)
    : Request(id, model, admissionCycle, prompt) {}

// ==================== NEW IMPLEMENTATIONS START HERE ====================
std::size_t EmbeddingRequest::getNeededMemory(const GPU& gpu) const {
    (void)gpu;
    return 0;
}

std::size_t EmbeddingRequest::getMaxNeededMemory() const {
    return 0;
}

std::vector<std::string> EmbeddingRequest::getCacheableTokens() const {
    return {};
}

void EmbeddingRequest::allocateOneBlock(GPU& gpu, std::size_t slot) const {
    (void)gpu;
    (void)slot;
}

Request* EmbeddingRequest::clone(const Model& model) const {
    (void)model;
    return nullptr;
}

void EmbeddingRequest::act(GPU& gpu) {
    (void)gpu;
}

bool EmbeddingRequest::finished() const {
    return false;
}

void EmbeddingRequest::reserveMemory(GPU& gpu) {
    (void)gpu;
}

std::string EmbeddingRequest::getOutput() const {
    return "";
}

std::size_t EmbeddingRequest::getNeedToUnreserve() const {
    return 0;
}