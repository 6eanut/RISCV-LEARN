#include "../include/os.h"

extern ptr_t HEAP_START;
extern ptr_t HEAP_SIZE;
extern ptr_t TEXT_START;
extern ptr_t TEXT_END;
extern ptr_t RODATA_START;
extern ptr_t RODATA_END;
extern ptr_t DATA_START;
extern ptr_t DATA_END;
extern ptr_t BSS_START;
extern ptr_t BSS_END;

#define PAGE_SIZE 4096
#define PAGE_ORDER 12

#define PAGE_TAKEN (1 << 0)
#define PAGE_LAST (1 << 1)

static ptr_t _alloc_start = 0;
static ptr_t _alloc_end = 0;
static uint32_t _num_pages = 0;

struct Page_index
{
    uint8_t flag;
};

static inline void _clear(struct Page_index *pi)
{
    pi->flag = 0;
}

static inline int _is_free(struct Page_index *pi)
{
    if (pi->flag & PAGE_TAKEN)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

static inline int _is_last(struct Page_index *pi)
{
    if (pi->flag & PAGE_LAST)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static inline void _set_flag(struct Page_index *pi, uint8_t flag)
{
    pi->flag |= flag;
}

static inline ptr_t
_align_ptr(ptr_t address)
{
    ptr_t order = (1 << PAGE_ORDER) - 1;
    return (address + order) & (~order);
}

void page_init()
{
    ptr_t _heap_start_aligned = _align_ptr(HEAP_START);
    uint32_t num_reserved_pages = LENGTH_RAM / (PAGE_SIZE * PAGE_SIZE);
    _alloc_start = _heap_start_aligned + num_reserved_pages * PAGE_SIZE;
    _num_pages = (HEAP_SIZE - (_heap_start_aligned - HEAP_START)) / PAGE_SIZE - num_reserved_pages;
    _alloc_end = _alloc_start + _num_pages * PAGE_SIZE;

    // printf("index nums = %d\n", _alloc_start - HEAP_START);
    printf("HEAP_START = %p (aligned to %p), HEAP_SIZE = 0x%lx, \n"
           "num of reserved pages = %d, num of pages to be allocated for heap = %d\n",
           HEAP_START, _heap_start_aligned, HEAP_SIZE, num_reserved_pages, _num_pages);

    struct Page_index *pi = (struct Page_index *)HEAP_START;
    for (int i = 0; i < _num_pages; ++i)
    {
        _clear(pi);
        pi++;
    }

    printf("HEAP    :   %p -> %p\n", _alloc_start, _alloc_end);
    printf("BSS     :   %p -> %p\n", BSS_START, BSS_END);
    printf("DATA    :   %p -> %p\n", DATA_START, DATA_END);
    printf("RODATA  :   %p -> %p\n", RODATA_START, RODATA_END);
    printf("TEXT    :   %p -> %p\n", TEXT_START, TEXT_END);
}

void *page_alloc(int npages)
{
    int found = 0;
    struct Page_index *pi = (struct Page_index *)HEAP_START;
    for (int i = 0; i <= _num_pages - npages; i++)
    {
        if (_is_free(pi))
        {
            found++;
            struct Page_index *pi_j = pi + 1;
            for (int j = 0; j < npages - 1; j++)
            {
                if (_is_free(pi_j))
                    pi_j++;
                else
                {
                    found = 0;
                    break;
                }
            }
            if (found)
            {
                for (struct Page_index *page = pi; page < pi_j; page++)
                    _set_flag(page, PAGE_TAKEN);
                _set_flag(pi_j, PAGE_LAST);
                return (void *)(_alloc_start + i * PAGE_SIZE);
            }
        }
        pi++;
    }
    return NULL;
}

void page_free(void *p)
{
    ptr_t page = (ptr_t)p;
    struct Page_index *pi = (struct Page_index *)(HEAP_START + (page - _alloc_start) / PAGE_SIZE);
    while (1)
    {
        _clear(pi);
        pi++;
        if (_is_last(pi))
            return;
    }
}

// ******debug******

void *page_alloc_debug(int npages)
{
    printf("******page_alloc start******\n");
    int found = 0;
    struct Page_index *pi = (struct Page_index *)HEAP_START;
    for (int i = 0; i <= _num_pages - npages; i++)
    {
        if (_is_free(pi))
        {
            found++;
            struct Page_index *pi_j = pi + 1;
            for (int j = 0; j < npages - 1; j++)
            {
                if (_is_free(pi_j))
                    pi_j++;
                else
                {
                    found = 0;
                    break;
                }
            }
            if (found)
            {
                for (struct Page_index *page = pi; page < pi_j; page++)
                {
                    _set_flag(page, PAGE_TAKEN);
                    printf("alloc page_index = %p\n", page);
                }
                _set_flag(pi_j, PAGE_LAST);
                printf("******page_alloc end******\n");
                return (void *)(_alloc_start + i * PAGE_SIZE);
            }
        }
        pi++;
    }

    printf("******page_alloc end******\n");
    return NULL;
}

void page_free_debug(void *p)
{

    printf("******page_free start******\n");
    ptr_t page = (ptr_t)p;
    struct Page_index *pi = (struct Page_index *)(HEAP_START + (page - _alloc_start) / PAGE_SIZE);
    while (1)
    {
        printf("free page_index = %p\n", pi);
        _clear(pi);
        pi++;
        if (_is_last(pi))
        {
            printf("******page_free end******\n");
            return;
        }
    }
    printf("******page_free end******\n");
}

void page_test()
{
    void *p0 = page_alloc_debug(1);
    printf("p0 = %p\n", p0);

    void *p1 = page_alloc_debug(4);
    printf("p1 = %p\n", p1);

    void *p2 = page_alloc_debug(2);
    printf("p2 = %p\n", p2);

    page_free_debug((void *)p1);

    void *p3 = page_alloc_debug(3);
    printf("p3 = %p\n", p3);

    void *p4 = page_alloc_debug(3);
    printf("p4 = %p\n", p4);

    void *p5 = page_alloc_debug(1);
    printf("p5 = %p\n", p5);
}