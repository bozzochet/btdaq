#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x41086e, "module_layout" },
	{ 0x6980fe91, "param_get_int" },
	{ 0xff964b25, "param_set_int" },
	{ 0x4302d0eb, "free_pages" },
	{ 0x93fca811, "__get_free_pages" },
	{ 0x2e60bace, "memcpy" },
	{ 0x2da418b5, "copy_to_user" },
	{ 0x4f756975, "find_vma" },
	{ 0x3d229f1f, "do_mmap_pgoff" },
	{ 0x12da5bb2, "__kmalloc" },
	{ 0x884f145, "kmem_cache_alloc_trace" },
	{ 0xc288f8ce, "malloc_sizes" },
	{ 0x37a0cba, "kfree" },
	{ 0x5c760099, "mem_section" },
	{ 0xc1cf5f54, "do_munmap" },
	{ 0x7d0bea0b, "per_cpu__current_task" },
	{ 0x3c2c5af5, "sprintf" },
	{ 0x70b2af54, "create_proc_entry" },
	{ 0xcfdbba1a, "__register_chrdev" },
	{ 0x7997cb91, "__pci_register_driver" },
	{ 0x8f05adee, "remap_pfn_range" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0xec88c4a4, "pci_unregister_driver" },
	{ 0x8d8aacb9, "remove_proc_entry" },
	{ 0x859c6dc7, "request_threaded_irq" },
	{ 0xe168a33c, "pci_bus_read_config_byte" },
	{ 0x42c8de35, "ioremap_nocache" },
	{ 0x2107fe08, "pci_bus_write_config_word" },
	{ 0xde95d1ee, "pci_bus_read_config_word" },
	{ 0xb72397d5, "printk" },
	{ 0xf20dabd8, "free_irq" },
	{ 0xedc03953, "iounmap" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "2453E767C777C15F0C1A0DC");

static const struct rheldata _rheldata __used
__attribute__((section(".rheldata"))) = {
	.rhel_major = 6,
	.rhel_minor = 9,
	.rhel_release = 695,
};
