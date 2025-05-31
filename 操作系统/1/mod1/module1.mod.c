#include <linux/module.h>
#include <linux/export-internal.h>
#include <linux/compiler.h>

MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x3ddef400, "init_task" },
	{ 0xe8213e80, "_printk" },
	{ 0xcea2bb81, "module_layout" },
};

static const u32 ____version_ext_crcs[]
__used __section("__version_ext_crcs") = {
	0x3ddef400,
	0xe8213e80,
	0xcea2bb81,
};
static const char ____version_ext_names[]
__used __section("__version_ext_names") =
	"init_task\0"
	"_printk\0"
	"module_layout\0"
;

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "2E90E3609EBFE41E9A45BAB");
