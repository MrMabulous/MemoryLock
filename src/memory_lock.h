#ifndef MEMORY_LOCK_H
#define MEMORY_LOCK_H

namespace memorylock {

struct MemoryRange {
  void* address;
  size_t length;
};

void LockMemory(void* addr, size_t length);

void UnlockMemory(void* addr, size_t length);

void PrefetchMemory(MemoryRange* ranges, size_t elements);

}  // namespace memorylock

#endif  // MEMORY_LOCK_H