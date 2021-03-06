// Copyright 2016 Vlad Vesa
// Author: hello@vladvesa.ro (Vlad Vesa)

// EasyMock: a tool for mock global function, member function, class static function, virtual member function.
//
// Check os and include the special implement.

#ifndef EASYMOCK_RUNTIME_PATCH_H_
#define EASYMOCK_RUNTIME_PATCH_H_

#include <vector>

namespace EasyMock {

    namespace RuntimePatcherImpl {
        // Need impl in architecture relevant file.
        // OS
        static int UnprotectMemoryForOnePage(void* const address);
        // CPU
        static int SetJump(void* const address, const void* const destination, std::vector<char>& binary_backup);
        static void RevertJump(void* address, const std::vector<char>& binary_backup);
    }

} // namespace EasyMock

#if defined(__x86_64__) || defined(__i386__)
#include "x86/runtime_patch_impl.h"
#endif

#ifdef __APPLE__
#include "posix/runtime_patch_impl.h"
#elif __linux__
#include "posix/runtime_patch_impl.h"
#elif __WIN32__
#include "posix/runtime_patch_impl_win32.h"
#endif

namespace EasyMock {

    // Provide 2 interface, GraftFunction and RevertGraft.
    struct RuntimePatcher
    {
        template <typename F >
        static int GraftFunction(void* address, F destination, std::vector<char>& binary_backup) {
            if (0 != RuntimePatcherImpl::UnprotectMemoryForOnePage(address)) {
                std::abort();
            } else {
                // For mock std::abort, this need not after the 'if'.
                return RuntimePatcherImpl::SetJump(address, reinterpret_cast<void*>((std::size_t&)destination), binary_backup);
            }
        }

        static void RevertGraft(void* address, const std::vector<char>& binary_backup) {
            RuntimePatcherImpl::RevertJump(address, binary_backup);
        }
    };

} // namespace EasyMock

#endif // EASYMOCK_RUNTIME_PATCH_H_
