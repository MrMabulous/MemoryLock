#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
  #if !defined(_WIN32_WINNT) || (_WIN32_WINNT < 0x0602)
    #pragma message ( "To use PrefetchMemory(), compile with _WIN32_WINNT at least 0x602 (aka Windows 8)." )
  #else
    #define WINPREFETCHMEMORY 1
  #endif
#include <windows.h>
#include <memoryapi.h>
#define WINMEM 1
#elif defined(__linux__) || defined(__unix__)
#include <unistd.h>
#define UNIXMEM 1
#endif
#include <stdexcept>
#include <string>
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

std::string LastSystemErrorText() {
  char err[256];
  FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(),
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), err, 255, NULL);
  return std::string(err);
}

}

namespace memorylock {

void LockMemory(void* addr, size_t len) {
#if defined(UNIXMEM)
  const size_t pageSize = GetPageSize();
  if (mlock(AlignDown(addr, pageSize), AlignUp(len, pageSize)))                                                                                     
    throw std::runtime_error(LastSystemErrorText());
#elif defined(WINMEM)
  HANDLE hndl = GetCurrentProcess();
  size_t min, max;
  if (!GetProcessWorkingSetSize(hndl, &min, &max))
    throw std::runtime_error(LastSystemErrorText());
  if (!SetProcessWorkingSetSize(hndl, min + len, max + len))
    throw std::runtime_error(LastSystemErrorText());
  if (!VirtualLock(addr, len))
    throw std::runtime_error(LastSystemErrorText());
#endif
}

void UnlockMemory(void* addr, size_t len) {
#if defined(UNIXMEM)
  const size_t pageSize = GetPageSize();
  if (munlock(AlignDown(addr, pageSize), AlignUp(len, pageSize)))                                                                                     
    throw std::runtime_error(LastSystemErrorText());
#elif defined(WINMEM)
  HANDLE hndl = GetCurrentProcess();
  if (!VirtualLock(addr, len))
    throw std::runtime_error(LastSystemErrorText());
  size_t min, max;
  if (!GetProcessWorkingSetSize(hndl, &min, &max))
    throw std::runtime_error(LastSystemErrorText());
  if (!SetProcessWorkingSetSize(hndl, min - len, max - len))
    throw std::runtime_error(LastSystemErrorText());
#endif
}

void PrefetchMemory(MemoryRange* ranges, size_t elements) {
#if defined(UNIXMEM)
  // TODO
#elif defined(WINPREFETCHMEMORY)
  HANDLE hndl = GetCurrentProcess();
  ULONG_PTR n = elements;
  WIN32_MEMORY_RANGE_ENTRY* entries = (WIN32_MEMORY_RANGE_ENTRY*)ranges;
  if (!PrefetchVirtualMemory(hndl, n, entries, 0))
    throw std::runtime_error(LastSystemErrorText());
#endif
}

void SetWorkingSetSize(size_t min, size_t max) {
#if defined(UNIXMEM)
  // TODO
#elif defined(WINMEM)
  HANDLE hndl = GetCurrentProcess();
  if (!SetProcessWorkingSetSize(hndl, min, max))
    throw std::runtime_error(LastSystemErrorText());
#endif
}

}  // namespace memorylock