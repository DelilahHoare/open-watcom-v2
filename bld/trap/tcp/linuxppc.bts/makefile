#pmake: serv remote os_linux cpu_ppc tcp ppc monolithic

bootstrap = 1

host_os  = linux
host_cpu = ppc

target_cpu = ppc
target_os  = linux

srv   = tcp
which = SERVER
remote_link = 1
builtin_trap = 1

!include $(trap_dir)/lcl/linux/linux.mif
!include $(trap_dir)/lcl/lcl.mif
!include $(trap_dir)/master.mif
