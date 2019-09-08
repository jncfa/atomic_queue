/* -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
#include "huge_pages.h"

#include <cstdio>

#include <sys/mman.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

atomic_queue::HugePages::HugePages(Type t, size_t total_size) {
    size_t const page_size = 1u << t;
    total_size = (total_size + (page_size - 1)) & ~(page_size - 1); // Round up to the page size.
    void* p = ::mmap(nullptr, total_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB | (t << MAP_HUGE_SHIFT) | MAP_LOCKED, -1, 0);
    if(p == MAP_FAILED) {
        std::fprintf(stderr, "Warning: Failed to allocate huge pages. Run \"sudo hugeadm --pool-pages-min 1GB:1 --pool-pages-max 1GB:1\".\n");
        p = ::mmap(nullptr, total_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_LOCKED, -1, 0);
        if(p == MAP_FAILED)
            throw std::bad_alloc();
    }

    beg_ = static_cast<unsigned char*>(p);
    cur_ = beg_;
    end_ = beg_ + total_size;
}

atomic_queue::HugePages::~HugePages() {
    if(beg_)
        ::munmap(beg_, end_ - beg_);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
