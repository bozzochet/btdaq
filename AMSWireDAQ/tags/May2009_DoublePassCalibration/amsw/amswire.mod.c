#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

#undef unix
struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = __stringify(KBUILD_MODNAME),
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
};

static const struct modversion_info ____versions[]
__attribute_used__
__attribute__((section("__versions"))) = {
	{ 0x65b4e613, "struct_module" },
	{ 0xc192d491, "unregister_chrdev" },
	{ 0x6d172fe6, "pci_unregister_driver" },
	{ 0xdfb8b6d0, "remove_proc_entry" },
	{ 0x66a49e22, "create_proc_entry" },
	{ 0x6506d836, "register_chrdev" },
	{ 0xaaa30118, "pci_register_driver" },
	{ 0x1d26aa98, "sprintf" },
	{ 0xf20dabd8, "free_irq" },
	{ 0xedc03953, "iounmap" },
	{ 0x26e96637, "request_irq" },
	{ 0xd3624ac6, "pci_bus_read_config_byte" },
	{ 0x9eac042a, "__ioremap" },
	{ 0xb6ba13c7, "pci_bus_write_config_word" },
	{ 0xd19e6e47, "do_munmap" },
	{ 0x37a0cba, "kfree" },
	{ 0xd9682704, "do_mmap_pgoff" },
	{ 0x43ca7336, "mem_map" },
	{ 0x7da8156e, "__kmalloc" },
	{ 0x123d3b6a, "kmem_cache_alloc" },
	{ 0x6c3397fb, "malloc_sizes" },
	{ 0x9ef719e7, "remap_page_range" },
	{ 0xd7474566, "__copy_to_user_ll" },
	{ 0x49e79940, "__cond_resched" },
	{ 0x9925ce9c, "__might_sleep" },
	{ 0x9ba12770, "find_vma" },
	{ 0x9941ccb8, "free_pages" },
	{ 0x4784e424, "__get_free_pages" },
	{ 0xc6edbc73, "pci_bus_read_config_word" },
	{ 0x1b7d4074, "printk" },
};

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
"depends=";

