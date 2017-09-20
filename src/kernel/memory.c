#include "memory.h"
#include "main.h"
#include "string.h"

extern unsigned const __end__;

frames_buf_t free_mem_0 = { 1, NULL, { { 0x100, 0x1000, NULL, NULL } } };
frames_buf_t *free_mem = &free_mem_0; /* wolna pamiec */
frame_list_t free_mem_list = { 0, 0, &free_mem_0.frames[0], NULL };

frames_array_buf_t allocated_mem_0 = { 0, NULL };
frames_array_buf_t *allocated_mem = &allocated_mem_0; /* zajeta pamiec */

//PHYSICAL_ADDRESS **krnl_pgdir;

void init_memory()
{
    /*unsigned cr0;
    krnl_pgdir = (PHYSICAL_ADDRESS**)((__end__ & 0xFFFFF000) + 0x1000);
    PHYSICAL_ADDRESS *pte0 = ((PHYSICAL_ADDRESS*)krnl_pgdir) + 0x1000;
    
    for(unsigned i=1u; i<1024u; ++i)
        krnl_pgdir[i] = NULL; //not present
    
    krnl_pgdir[0] = (PHYSICAL_ADDRESS*)(((DWORD)pte0)|PAGE_PRESENT|PAGE_WRITABLE|PAGE_SUPERVISOR);
    
    for(unsigned i=0u; i<1024u; ++i)
        pte0[i] = i*PAGE_SIZE|PAGE_PRESENT|PAGE_WRITABLE|PAGE_SUPERVISOR; //not present
    
    __asm__ __volatile__("mov %0, %%cr3":: "b"(krnl_pgdir));
    __asm__ __volatile__("mov %%cr0, %0": "=b"(cr0));
    cr0 |= 0x80000000;
    //__asm__ __volatile__("mov %0, %%cr0":: "b"(cr0));*/
    free_mem_0.frames[0].prev = &free_mem_list;
}

PVOID MmAllocateContiguousMemory(SIZE_T NumberOfBytes, PHYSICAL_ADDRESS HighestAcceptableAddress)
{
    unsigned cPages = (NumberOfBytes + PAGE_SIZE - 1) / PAGE_SIZE, iHiPage = (HighestAcceptableAddress == 0xffffffff) ? (0x100000) : ((HighestAcceptableAddress + 1) / PAGE_SIZE); /* Zapobiegniecie przepelnieniu */
    frame_list_t *list, *frame;
    frames_array_buf_t *alloc_buf;
    unsigned max_c, iPage;
    
    if(!cPages)
        return NULL;
    
    /* Znajdowanie wolnej przsestrzeni */
    for(list = &free_mem_list; (frame = (list->next)); list = frame)
    {
        max_c = min(frame->count, iHiPage - frame->page);
        //printf("max_c: %u, frame->count: %x, frame: %x       ", max_c, frame->count, frame);
        if(cPages <= max_c)
        {
            iPage = frame->page;
            if(cPages == (frame->count)) /* Usuwanie bloku z list */
            {
                frames_buf_t *frames_buf;
                //printf("usuwane bloku :/    - ");
                for(frames_buf = free_mem; frames_buf; frames_buf = frames_buf->next)
                    if(frame >= (frames_buf->frames) && frame < (frames_buf->frames + frames_buf->c)) /* Bufor, w ktorym znajduje sie ramka */
                    {
                        //for(unsigned i=0;i<frames_buf->c;++i)
                        //  printf("FR %x.c=%x, next: %x prv: %x|", frames_buf->frames+i, frames_buf->frames[i].count, frames_buf->frames[i].next, frames_buf->frames[i].prev);
                        --frames_buf->c;
                        
                        /* Usuwanie dowiazan do ramki */
                        if(frame->prev)
                            frame->prev->next = frame->next;
                        if(frame->next)
                            frame->next->prev = frame->prev;
                        if(frame != (frames_buf->frames + frames_buf->c)) /* Jezeli usuwana ramka nie jest na koncu, trzeba przeniesc ostatnia ramke i zmienic dowiazania do niej */
                        {
                            (*frame) = frames_buf->frames[frames_buf->c];
                            if(frame->prev)
                                frame->prev->next = frame;
                            if(frame->next)
                                frame->next->prev = frame;
                        }
                    }
            }
            else
            {
                //printf("skracanie bloku :) - count=%u-%u page=%u+%u    frame: %x\n", frame->count, cPages, frame->page, cPages, frame);
                frame->count -= cPages;
                frame->page += cPages;
            }
            goto allocate_mem;
        }
    }
    return NULL;
    
allocate_mem: /* Alokowanie pamiêci */
    for(alloc_buf = allocated_mem; alloc_buf->next; alloc_buf = alloc_buf->next)
        if(alloc_buf->c < PAGES_IN_ARRAY_BUF)
            break;
    alloc_buf->frames[ alloc_buf->c ].page = iPage;
    alloc_buf->frames[ alloc_buf->c ].count = cPages;
    ++alloc_buf->c;
    if(alloc_buf->c == PAGES_IN_ARRAY_BUF - 1 && !(alloc_buf->next)) /* Nalezy zaalokowac nastepny bufor */
    {
        alloc_buf->next = (frames_array_buf_t*)MmAllocateContiguousMemory(sizeof(frames_array_buf_t), 0xffffffff);
        if(alloc_buf->next)
        {
            alloc_buf->next->c = 0;
            alloc_buf->next->next = NULL;
        }
    }
    return (PVOID)(iPage * PAGE_SIZE);
}

VOID MmFreeContiguousMemory(PVOID BaseAddress)
{
    unsigned iPage = ((unsigned)BaseAddress) / PAGE_SIZE, i, cPages = 0;
    frames_array_buf_t *alloc_mem;
    frame_list_t *list, *frame;
    frames_buf_t *free_mem_buf;
    
    if(((unsigned)BaseAddress) & (PAGE_SIZE - 1)) /* bledny adres (nie podzielny przez rozmiar strony) */
        return;
        
    //printf("freeing\n");
    
    /* Znajdz naglowek bloku */
    for(alloc_mem = allocated_mem; alloc_mem; alloc_mem = alloc_mem->next)
    {
        for(i = 0u; i < alloc_mem->c; ++i)
        {
            if(alloc_mem->frames[i].page == iPage)
            {
                cPages = alloc_mem->frames[i].count;
                --alloc_mem->c;
                alloc_mem->frames[i] = alloc_mem->frames[alloc_mem->c];
                goto add_free_mem;
            }
        }
    }
    return;
    
add_free_mem: /* Dodaj zwolniona pamiec */
    for(list = &free_mem_list; (frame = (list->next)); list = frame)
    {
        if(iPage + cPages == (frame->page)) /* frame jest bezposrednio po naszej ramce */
        {
            frame->count += cPages;
            frame->page = iPage;
            if(frame->prev != (&free_mem_list) && (frame->prev->page + frame->prev->count) == iPage) /* frame->prev jest bezposrednio przed nasza ramka */
            {
                frame->prev->count += frame->count;
                frame->prev->next = frame->next;
                if(frame->next)
                    frame->next->prev = frame->prev;
                    
                /* Usuwanie z bufora */
                for(free_mem_buf = free_mem; free_mem_buf; free_mem_buf = free_mem_buf->next)
                    if(frame >= (free_mem_buf->frames) && frame < (free_mem_buf->frames + free_mem_buf->c)) /* Bufor, w ktorym znajduje sie ramka */
                    {
                        --free_mem_buf->c;
                        
                        /* Usuwanie dowiazan do ramki */
                        if(frame != (free_mem_buf->frames + free_mem_buf->c)) /* Jezeli usuwana ramka nie jest na koncu, trzeba przeniesc ostatnia ramke i zmienic dowiazania do niej */
                        {
                            (*frame) = free_mem_buf->frames[free_mem_buf->c];
                            if(frame->prev)
                                frame->prev->next = frame;
                            if(frame->next)
                                frame->next->prev = frame;
                        }
                    }
            }
            return; /* Ramki polaczone */
        }
        if(iPage < (frame->page))
            break;
    }
    frame = list; /* frame=miejsce po ktorym trzeba wstawic */
    for(free_mem_buf = free_mem; free_mem_buf->next; free_mem_buf = free_mem->next)
        if(free_mem_buf->c < FRAMES_IN_BUF)
            break;
    if(free_mem_buf->c == FRAMES_IN_BUF) /* Bufor sie skonczyl */
    {
        free_mem_buf = (free_mem_buf->next = (frames_buf_t*)MmAllocateContiguousMemory(sizeof(frames_buf_t), 0xffffffff));
        free_mem_buf->c = 0;
        free_mem_buf->next = NULL;
    }
    free_mem_buf->frames[free_mem_buf->c].page = iPage;
    free_mem_buf->frames[free_mem_buf->c].count = cPages;
    free_mem_buf->frames[free_mem_buf->c].next = frame->next;
    free_mem_buf->frames[free_mem_buf->c].prev = frame;
    if(frame->next)
        frame->next->prev = &free_mem_buf->frames[free_mem_buf->c];
    frame->next = &free_mem_buf->frames[free_mem_buf->c];
    ++free_mem_buf->c;
}

PHYSICAL_ADDRESS MmGetPhysicalAddress(PVOID BaseAddress)
{
    //if( krnl_pgdir[((DWORD)BaseAddress)>>22]&1 )
    //krnl_pgdir[((DWORD)BaseAddress)>>22];
    return (PHYSICAL_ADDRESS) BaseAddress;
}

MM_SYSTEMSIZE MmQuerySystemSize(VOID)
{
    return MmLargeSystem;
}

void *malloc(size_t size)
{
    return MmAllocateContiguousMemory(size, 0xffffffff);
}

void free(void *ptr)
{
    return MmFreeContiguousMemory(ptr);
}
