#pmake: build lcl os_linux cpu_386 trap x86
#
# Create the Linux std.trp
#
host_cpu = 386
host_os  = linux

target_cpu = x86
target_os  = linux

srv     = lcl
which   = TRAP

extra_objs = $(linux_trap_objs)

!include $(trap_dir)/lcl/linux/linux.mif
!include $(trap_dir)/lcl/lcl.mif
!include $(trap_dir)/master.mif
