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
	{ 0xe0b4aa45, "find_get_pid" },
	{ 0xa42a278d, "pid_task" },
	{ 0xe8213e80, "_printk" },
	{ 0xce45c1ac, "param_ops_uint" },
	{ 0xcea2bb81, "module_layout" },
};

static const u32 ____version_ext_crcs[]
__used __section("__version_ext_crcs") = {
	0xe0b4aa45,
	0xa42a278d,
	0xe8213e80,
	0xce45c1ac,
	0xcea2bb81,
};
static const char ____version_ext_names[]
__used __section("__version_ext_names") =
	"find_get_pid\0"
	"pid_task\0"
	"_printk\0"
	"param_ops_uint\0"
	"module_layout\0"
;

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "018C67212CF434F1D62243A");
