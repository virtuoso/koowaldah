#include <koowaldah.h>

#include <thread.h>
#include <mem_area.h>
#include <mm_zone.h>

u64 __volatile__ jiffies;

extern void putchar(char);

/* shut it for late init calls */
static int __late_inits __attribute__((section(".data")));
extern int late_init_start __attribute__((weak,alias("__late_inits")));
extern int late_init_end __attribute__((weak,alias("__late_inits")));

int dummy_get_mem_size();

/* MMU, paging */
void arch_init_boot_zone(struct mem_zone *zone, struct mem_zone *user)
{
	u32 phys_mem, total_mem;
	
	phys_mem = total_mem = dummy_get_mem_size();
	kprintf("Got %dM (0x%x) bytes of memory\n",
			phys_mem >> 20, total_mem);

	zone->base = NULL;
	zone->total_pages = 0xf0000 >> PAGE_SHIFT;
	kprintf("Total pages: %d, phys_pgs: %d\n", zone->total_pages);

	mem_zone_init(zone);

	user->total_pages = (total_mem >> PAGE_SHIFT) - zone->total_pages;
	user->base = (u32 *)((u32)zone->base + (zone->total_pages << PAGE_SHIFT));

	kprintf("Total pages: %d, base: %x\n", user->total_pages, user->base);
	mem_zone_init(user);
}

void paging_init()
{
}

void map_page(struct mapping *map, u32 virt, u32 phys, u16 flags)
{
}

void unmap_page(struct mapping *map, u32 virt)
{
}

void map_pages(struct mapping *map, u32 virt, u32 phys, u32 n, u16 flags)
{
}

struct mapping root_map;

void switch_map(struct mapping *from, struct mapping *to)
{
}

void clone_map(struct mapping *dst, struct mapping *map)
{
}

void free_map(struct mapping *map)
{
}

/* IO */
void console_put_char(char c)
{
	putchar(c);
}

/* debug */
void display_thread()
{
}

__noreturn void dummy_noreturn();

void dummy_create_thread(void *t, void *tc, thread_t func, void *data);

__noreturn void dummy_start_user(u32 pc, u32 sp);

void start_user(u32 eip, u32 ebp, u32 esp, u32 val)
{
	dummy_start_user(eip, esp);
}

void thread_init_stack(struct thread *t, thread_t func, void *data)
{
	dummy_create_thread((void *)t, (void *)t->context.placeholder,
			func, data);
}

void dummy_switch_context(void *, void *);

void thread_switch_context(struct thread *from, struct thread *to)
{
	dummy_switch_context((void *)from->context.placeholder,
			(void *)to->context.placeholder);
}

void dummy_start(void *);

void thread_switch_to(struct thread *thread)
{
	dummy_start((void *)thread->context.placeholder);
}

void mach_start()
{
}

void mach_running()
{
}

int register_irq_handler(u32 number, void (*handler)(u32 number))
{
	return 1;
}

void unregister_irq_handler(u32 number)
{
}

