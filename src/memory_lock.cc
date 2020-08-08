#ifdef _WIN32
#include <windows.h>
#include <memoryapi.h>
#endif
#ifdef linux
#include <unistd.h>
#endif
#include <exception>
#include "memory_lock.h"

namespace {

//Rounds an integer down to the nearest multiple of a specified power of two.
//@value: The value to be rounded.
//@align: The power of two around which the value is rounded.
template <typename T>
constexpr T AlignDown(T value, T align) { return value & ~(align - 1); }

//Rounds an integer up to the nearest multiple of a specified power of two.
//@value: The value to be rounded.
//@align: The power of two around which the value is rounded.
template <typename T>
constexpr T AlignUp(T value, T align) { return (value + (align - 1)) & ~(align - 1); }

}

namespace memorylock {

void LockMemory(void* addr, size_t len) {
#if defined(_unix_)
  const size_t pageSize = GetPageSize();
  if (mlock(AlignDown(addr, pageSize), AlignUp(len, pageSize)))                                                                                     
    throw std::exception(LastSystemErrorText());
#elif defined(_win_)
  HANDLE hndl = GetCurrentProcess();
  size_t min, max;
  if (!GetProcessWorkingSetSize(hndl, &min, &max))
    throw std::exception(LastSystemErrorText());
  if (!SetProcessWorkingSetSize(hndl, min + len, max + len))
    throw std::exception(LastSystemErrorText());
  if (!VirtualLock(addr, len))
    throw std::exception(LastSystemErrorText());
#endif
}

void UnlockMemory(void* addr, size_t len) {
#if defined(_unix_)
  const size_t pageSize = GetPageSize();
  if (munlock(AlignDown(addr, pageSize), AlignUp(len, pageSize)))                                                                                     
    throw std::exception(LastSystemErrorText());
#elif defined(_win_)
  HANDLE hndl = GetCurrentProcess();
  if (!VirtualLock(addr, len))
    throw std::exception(LastSystemErrorText());
  size_t min, max;
  if (!GetProcessWorkingSetSize(hndl, &min, &max))
    throw std::exception(LastSystemErrorText());
  if (!SetProcessWorkingSetSize(hndl, min - len, max - len))
    throw std::exception(LastSystemErrorText());
#endif
}

void PrefetchMemory(MemoryRange* ranges, size_t elements) {
#if defined(_unix_)
  // TODO
#elif defined(_win_)
  HANDLE hndl = GetCurrentProcess();
  ULONG_PTR n = elements;
  PWIN32_MEMORY_RANGE_ENTRY* entries = ranges;
  if (!PrefetchVirtualMemory(hndl, n, entries, 0))
    throw std::exception(LastSystemErrorText());
#endif
}

}  // namespace memorylock