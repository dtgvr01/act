cmd_drivers/dahdi/xpp/xbus-core.o := bfin-uclinux-gcc -Wp,-MD,drivers/dahdi/xpp/.xbus-core.o.d  -nostdinc -isystem /opt/uClinux/bfin-uclinux/lib/gcc/bfin-uclinux/4.1.2/include -D__KERNEL__ -Iinclude  -include include/linux/autoconf.h -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -O2 -mno-fdpic -mcpu=bf533-0.5 -fomit-frame-pointer  -fno-stack-protector -Wdeclaration-after-statement -Wno-pointer-sign  -DDEBUG -DPOLL_DIGITAL_INPUTS -DDEBUG_PCMTX -DPROTOCOL_DEBUG -g   -DMODULE -mlong-calls -I -Idrivers/dahdi -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(xbus_core)"  -D"KBUILD_MODNAME=KBUILD_STR(xpp)" -c -o drivers/dahdi/xpp/xbus-core.o drivers/dahdi/xpp/xbus-core.c

deps_drivers/dahdi/xpp/xbus-core.o := \
  drivers/dahdi/xpp/xbus-core.c \
    $(wildcard include/config/proc/fs.h) \
  include/linux/version.h \
  include/linux/kernel.h \
    $(wildcard include/config/preempt/voluntary.h) \
    $(wildcard include/config/debug/spinlock/sleep.h) \
    $(wildcard include/config/printk.h) \
    $(wildcard include/config/numa.h) \
  /opt/uClinux/bfin-uclinux/lib/gcc/bfin-uclinux/4.1.2/include/stdarg.h \
  include/linux/linkage.h \
  include/asm/linkage.h \
  include/linux/stddef.h \
  include/linux/compiler.h \
    $(wildcard include/config/enable/must/check.h) \
  include/linux/compiler-gcc4.h \
    $(wildcard include/config/forced/inlining.h) \
  include/linux/compiler-gcc.h \
  include/linux/types.h \
    $(wildcard include/config/uid16.h) \
    $(wildcard include/config/lbd.h) \
    $(wildcard include/config/lsf.h) \
    $(wildcard include/config/resources/64bit.h) \
  include/linux/posix_types.h \
  include/asm/posix_types.h \
  include/asm/types.h \
  include/linux/bitops.h \
  include/asm/bitops.h \
  include/asm/byteorder.h \
  include/linux/byteorder/little_endian.h \
  include/linux/byteorder/swab.h \
  include/linux/byteorder/generic.h \
  include/asm/system.h \
    $(wildcard include/config/bfin/icache.h) \
    $(wildcard include/config/debug/hwerr.h) \
    $(wildcard include/config/smp.h) \
  include/asm/mach/anomaly.h \
  include/asm/blackfin.h \
  include/asm/mach/blackfin.h \
  include/asm/mach/bf533.h \
    $(wildcard include/config/bank/1.h) \
    $(wildcard include/config/bank/0.h) \
    $(wildcard include/config/bank/3.h) \
    $(wildcard include/config/bank/2.h) \
    $(wildcard include/config/c/amben/all.h) \
    $(wildcard include/config/c/amben.h) \
    $(wildcard include/config/c/amben/b0.h) \
    $(wildcard include/config/c/amben/b0/b1.h) \
    $(wildcard include/config/c/amben/b0/b1/b2.h) \
    $(wildcard include/config/c/amcken.h) \
    $(wildcard include/config/c/cdprio.h) \
    $(wildcard include/config/bf533.h) \
    $(wildcard include/config/bf532.h) \
    $(wildcard include/config/bf531.h) \
  include/asm/mach/mem_map.h \
    $(wildcard include/config/bfin/dcache.h) \
    $(wildcard include/config/bfin/dcache/banka.h) \
  include/asm/mach/defBF532.h \
  include/asm/mach-common/def_LPBlackfin.h \
  include/asm/mach/anomaly.h \
  include/asm/mach/cdefBF532.h \
  include/asm/mach-common/cdef_LPBlackfin.h \
  include/asm/bfin-global.h \
    $(wildcard include/config/dma/uncached/2m.h) \
    $(wildcard include/config/dma/uncached/1m.h) \
    $(wildcard include/config/mtd/uclinux.h) \
  include/asm-generic/sections.h \
  include/asm/ptrace.h \
    $(wildcard include/config/binfmt/elf/fdpic.h) \
  include/asm/user.h \
  include/asm-generic/bitops/ffs.h \
  include/asm-generic/bitops/__ffs.h \
  include/asm-generic/bitops/sched.h \
  include/asm-generic/bitops/ffz.h \
  include/asm-generic/bitops/find.h \
  include/asm-generic/bitops/hweight.h \
  include/asm-generic/bitops/ext2-atomic.h \
  include/asm-generic/bitops/ext2-non-atomic.h \
  include/asm-generic/bitops/le.h \
  include/asm-generic/bitops/minix.h \
  include/asm-generic/bitops/fls.h \
  include/asm-generic/bitops/fls64.h \
  include/linux/log2.h \
    $(wildcard include/config/arch/has/ilog2/u32.h) \
    $(wildcard include/config/arch/has/ilog2/u64.h) \
  include/asm/bug.h \
  include/asm-generic/bug.h \
    $(wildcard include/config/bug.h) \
    $(wildcard include/config/generic/bug.h) \
    $(wildcard include/config/debug/bugverbose.h) \
  include/linux/module.h \
    $(wildcard include/config/modules.h) \
    $(wildcard include/config/modversions.h) \
    $(wildcard include/config/unused/symbols.h) \
    $(wildcard include/config/module/unload.h) \
    $(wildcard include/config/kallsyms.h) \
    $(wildcard include/config/sysfs.h) \
  include/linux/spinlock.h \
    $(wildcard include/config/debug/spinlock.h) \
    $(wildcard include/config/preempt.h) \
    $(wildcard include/config/debug/lock/alloc.h) \
  include/linux/preempt.h \
    $(wildcard include/config/debug/preempt.h) \
  include/linux/thread_info.h \
  include/asm/thread_info.h \
  include/asm/page.h \
  include/asm/setup.h \
  include/asm/page_offset.h \
    $(wildcard include/config/blackfin.h) \
  include/asm/io.h \
  include/asm-generic/page.h \
  include/asm/entry.h \
  include/asm/l1layout.h \
  include/linux/stringify.h \
  include/linux/bottom_half.h \
  include/linux/spinlock_types.h \
  include/linux/lockdep.h \
    $(wildcard include/config/lockdep.h) \
    $(wildcard include/config/trace/irqflags.h) \
    $(wildcard include/config/generic/hardirqs.h) \
    $(wildcard include/config/prove/locking.h) \
  include/linux/spinlock_types_up.h \
  include/linux/spinlock_up.h \
  include/linux/spinlock_api_up.h \
  include/asm/atomic.h \
  include/asm-generic/atomic.h \
  include/linux/list.h \
    $(wildcard include/config/debug/list.h) \
  include/linux/poison.h \
  include/linux/prefetch.h \
  include/asm/processor.h \
    $(wildcard include/config/bf52x.h) \
    $(wildcard include/config/bf/rev/0/0.h) \
    $(wildcard include/config/bf/rev/0/1.h) \
    $(wildcard include/config/bf/rev/0/2.h) \
    $(wildcard include/config/bf/rev/0/3.h) \
    $(wildcard include/config/bf/rev/0/4.h) \
    $(wildcard include/config/bf/rev/0/5.h) \
    $(wildcard include/config/bf/rev/any.h) \
  include/asm/segment.h \
  include/asm/cache.h \
    $(wildcard include/config/cacheline/aligned/l1.h) \
  include/linux/stat.h \
  include/asm/stat.h \
  include/linux/time.h \
  include/linux/seqlock.h \
  include/linux/cache.h \
  include/linux/kmod.h \
    $(wildcard include/config/kmod.h) \
  include/linux/errno.h \
  include/asm/errno.h \
  include/asm-generic/errno.h \
  include/asm-generic/errno-base.h \
  include/linux/elf.h \
  include/linux/auxvec.h \
  include/asm/auxvec.h \
  include/linux/elf-em.h \
  include/asm/elf.h \
  include/linux/kobject.h \
    $(wildcard include/config/hotplug.h) \
  include/linux/sysfs.h \
  include/linux/kref.h \
  include/linux/wait.h \
  include/asm/current.h \
  include/linux/moduleparam.h \
  include/linux/init.h \
    $(wildcard include/config/hotplug/cpu.h) \
    $(wildcard include/config/memory/hotplug.h) \
    $(wildcard include/config/acpi/hotplug/memory.h) \
  include/asm/local.h \
  include/asm-generic/local.h \
  include/linux/percpu.h \
  include/linux/slab.h \
    $(wildcard include/config/slab/debug.h) \
    $(wildcard include/config/slab.h) \
    $(wildcard include/config/slub.h) \
    $(wildcard include/config/debug/slab.h) \
  include/linux/gfp.h \
    $(wildcard include/config/zone/dma.h) \
    $(wildcard include/config/zone/dma32.h) \
    $(wildcard include/config/highmem.h) \
  include/linux/mmzone.h \
    $(wildcard include/config/force/max/zoneorder.h) \
    $(wildcard include/config/np2.h) \
    $(wildcard include/config/arch/populates/node/map.h) \
    $(wildcard include/config/discontigmem.h) \
    $(wildcard include/config/flat/node/mem/map.h) \
    $(wildcard include/config/have/memory/present.h) \
    $(wildcard include/config/need/node/memmap/size.h) \
    $(wildcard include/config/need/multiple/nodes.h) \
    $(wildcard include/config/sparsemem.h) \
    $(wildcard include/config/have/arch/early/pfn/to/nid.h) \
    $(wildcard include/config/flatmem.h) \
    $(wildcard include/config/sparsemem/extreme.h) \
    $(wildcard include/config/nodes/span/other/nodes.h) \
    $(wildcard include/config/holes/in/zone.h) \
  include/linux/threads.h \
    $(wildcard include/config/nr/cpus.h) \
    $(wildcard include/config/base/small.h) \
  include/linux/numa.h \
    $(wildcard include/config/nodes/shift.h) \
  include/linux/nodemask.h \
  include/linux/bitmap.h \
  include/linux/string.h \
  include/asm/string.h \
  include/linux/memory_hotplug.h \
    $(wildcard include/config/have/arch/nodedata/extension.h) \
  include/linux/notifier.h \
  include/linux/mutex.h \
    $(wildcard include/config/debug/mutexes.h) \
  include/linux/rwsem.h \
    $(wildcard include/config/rwsem/generic/spinlock.h) \
  include/linux/rwsem-spinlock.h \
  include/linux/srcu.h \
  include/linux/topology.h \
    $(wildcard include/config/sched/smt.h) \
    $(wildcard include/config/sched/mc.h) \
  include/linux/cpumask.h \
  include/linux/smp.h \
  include/asm/topology.h \
  include/asm-generic/topology.h \
  include/linux/slab_def.h \
  include/linux/kmalloc_sizes.h \
  include/asm/percpu.h \
  include/asm-generic/percpu.h \
  include/linux/hardirq.h \
    $(wildcard include/config/preempt/bkl.h) \
    $(wildcard include/config/virt/cpu/accounting.h) \
  include/linux/smp_lock.h \
    $(wildcard include/config/lock/kernel.h) \
  include/asm/hardirq.h \
  include/asm/irq.h \
  include/asm/mach/irq.h \
  include/linux/irq_cpustat.h \
  include/asm/module.h \
  include/linux/proc_fs.h \
    $(wildcard include/config/proc/devicetree.h) \
    $(wildcard include/config/proc/kcore.h) \
    $(wildcard include/config/mmu.h) \
  include/linux/fs.h \
    $(wildcard include/config/dnotify.h) \
    $(wildcard include/config/quota.h) \
    $(wildcard include/config/inotify.h) \
    $(wildcard include/config/security.h) \
    $(wildcard include/config/epoll.h) \
    $(wildcard include/config/auditsyscall.h) \
    $(wildcard include/config/block.h) \
    $(wildcard include/config/fs/xip.h) \
    $(wildcard include/config/migration.h) \
  include/linux/limits.h \
  include/linux/ioctl.h \
  include/asm/ioctl.h \
  include/asm-generic/ioctl.h \
  include/linux/kdev_t.h \
  include/linux/dcache.h \
    $(wildcard include/config/profiling.h) \
  include/linux/rcupdate.h \
  include/linux/namei.h \
  include/linux/radix-tree.h \
  include/linux/prio_tree.h \
  include/linux/pid.h \
  include/asm/semaphore.h \
  include/linux/quota.h \
  include/linux/dqblk_xfs.h \
  include/linux/dqblk_v1.h \
  include/linux/dqblk_v2.h \
  include/linux/nfs_fs_i.h \
  include/linux/nfs.h \
  include/linux/sunrpc/msg_prot.h \
  include/linux/fcntl.h \
  include/asm/fcntl.h \
  include/asm-generic/fcntl.h \
    $(wildcard include/config/64bit.h) \
  include/linux/err.h \
  include/linux/magic.h \
  include/linux/ctype.h \
  include/linux/workqueue.h \
  include/linux/timer.h \
    $(wildcard include/config/timer/stats.h) \
  include/linux/ktime.h \
    $(wildcard include/config/ktime/scalar.h) \
  include/linux/jiffies.h \
  include/linux/calc64.h \
  include/asm/div64.h \
  include/asm-generic/div64.h \
  include/linux/timex.h \
    $(wildcard include/config/time/interpolation.h) \
    $(wildcard include/config/no/hz.h) \
  include/asm/param.h \
    $(wildcard include/config/hz.h) \
  include/asm/timex.h \
  include/linux/device.h \
    $(wildcard include/config/debug/devres.h) \
  include/linux/ioport.h \
  include/linux/klist.h \
  include/linux/completion.h \
  include/linux/pm.h \
    $(wildcard include/config/pm.h) \
  include/asm/device.h \
  include/asm-generic/device.h \
  include/linux/delay.h \
  include/asm/delay.h \
  include/linux/param.h \
  drivers/dahdi/xpp/xpd.h \
    $(wildcard include/config/debug/fs.h) \
  drivers/dahdi/xpp/xdefs.h \
  drivers/dahdi/xpp/xpp_version.h \
  drivers/dahdi/xpp/xproto.h \
  include/dahdi/kernel.h \
    $(wildcard include/config/dahdi/net.h) \
    $(wildcard include/config/dahdi/ppp.h) \
    $(wildcard include/config/calc/xlaw.h) \
    $(wildcard include/config/dahdi/watchdog.h) \
  include/dahdi/user.h \
    $(wildcard include/config/txlevel.h) \
    $(wildcard include/config/d4.h) \
    $(wildcard include/config/esf.h) \
    $(wildcard include/config/ami.h) \
    $(wildcard include/config/b8zs.h) \
    $(wildcard include/config/ccs.h) \
    $(wildcard include/config/hdb3.h) \
    $(wildcard include/config/crc4.h) \
    $(wildcard include/config/notopen.h) \
  include/dahdi/fasthdlc.h \
  include/dahdi/dahdi_config.h \
    $(wildcard include/config/h.h) \
    $(wildcard include/config/dahdi/mmx.h) \
    $(wildcard include/config/hdlc.h) \
    $(wildcard include/config/ppp.h) \
    $(wildcard include/config/old/hdlc/api.h) \
    $(wildcard include/config/dahdi/debug.h) \
    $(wildcard include/config/cac/groundstart.h) \
    $(wildcard include/config/dahdi/core/timer.h) \
  include/linux/poll.h \
  include/asm/poll.h \
  include/linux/sched.h \
    $(wildcard include/config/detect/softlockup.h) \
    $(wildcard include/config/split/ptlock/cpus.h) \
    $(wildcard include/config/keys.h) \
    $(wildcard include/config/bsd/process/acct.h) \
    $(wildcard include/config/taskstats.h) \
    $(wildcard include/config/inotify/user.h) \
    $(wildcard include/config/schedstats.h) \
    $(wildcard include/config/task/delay/acct.h) \
    $(wildcard include/config/blk/dev/io/trace.h) \
    $(wildcard include/config/cc/stackprotector.h) \
    $(wildcard include/config/sysvipc.h) \
    $(wildcard include/config/rt/mutexes.h) \
    $(wildcard include/config/task/xacct.h) \
    $(wildcard include/config/cpusets.h) \
    $(wildcard include/config/compat.h) \
    $(wildcard include/config/fault/injection.h) \
  include/linux/capability.h \
  include/linux/rbtree.h \
  include/asm/mmu.h \
    $(wildcard include/config/mpu.h) \
  include/asm/cputime.h \
  include/asm-generic/cputime.h \
  include/linux/sem.h \
  include/linux/ipc.h \
    $(wildcard include/config/ipc/ns.h) \
  include/asm/ipcbuf.h \
  include/asm/sembuf.h \
  include/linux/signal.h \
  include/asm/signal.h \
  include/asm-generic/signal.h \
  include/asm/sigcontext.h \
  include/asm/siginfo.h \
  include/asm-generic/siginfo.h \
  include/linux/securebits.h \
  include/linux/fs_struct.h \
  include/linux/seccomp.h \
    $(wildcard include/config/seccomp.h) \
  include/linux/futex.h \
    $(wildcard include/config/futex.h) \
  include/linux/rtmutex.h \
    $(wildcard include/config/debug/rt/mutexes.h) \
  include/linux/plist.h \
    $(wildcard include/config/debug/pi/list.h) \
  include/linux/resource.h \
  include/asm/resource.h \
  include/asm-generic/resource.h \
  include/linux/hrtimer.h \
    $(wildcard include/config/high/res/timers.h) \
  include/linux/task_io_accounting.h \
    $(wildcard include/config/task/io/accounting.h) \
  include/linux/aio.h \
  include/linux/aio_abi.h \
  include/linux/uio.h \
  include/asm/uaccess.h \
    $(wildcard include/config/access/check.h) \
    $(wildcard include/config/access/ok/l1.h) \
  include/linux/mm.h \
    $(wildcard include/config/sysctl.h) \
    $(wildcard include/config/stack/growsup.h) \
    $(wildcard include/config/debug/vm.h) \
    $(wildcard include/config/shmem.h) \
    $(wildcard include/config/ia64.h) \
    $(wildcard include/config/debug/pagealloc.h) \
  include/linux/debug_locks.h \
    $(wildcard include/config/debug/locking/api/selftests.h) \
  include/linux/backing-dev.h \
  include/linux/mm_types.h \
  include/asm/pgtable.h \
  include/asm-generic/4level-fixup.h \
  include/asm-generic/pgtable.h \
  include/linux/page-flags.h \
    $(wildcard include/config/s390.h) \
    $(wildcard include/config/swap.h) \
  include/linux/vmstat.h \
    $(wildcard include/config/vm/event/counters.h) \
  drivers/dahdi/xpp/card_global.h \
  drivers/dahdi/xpp/xbus-pcm.h \
  drivers/dahdi/xpp/card_fxs.h \
  drivers/dahdi/xpp/card_fxo.h \
  drivers/dahdi/xpp/card_bri.h \
  drivers/dahdi/xpp/card_pri.h \
  drivers/dahdi/xpp/xpp_dahdi.h \
  drivers/dahdi/xpp/xbus-core.h \
  include/linux/interrupt.h \
    $(wildcard include/config/generic/irq/probe.h) \
  include/linux/irqreturn.h \
  include/linux/irqflags.h \
    $(wildcard include/config/trace/irqflags/support.h) \
    $(wildcard include/config/x86.h) \
  drivers/dahdi/xpp/xframe_queue.h \
  drivers/dahdi/xpp/dahdi_debug.h \

drivers/dahdi/xpp/xbus-core.o: $(deps_drivers/dahdi/xpp/xbus-core.o)

$(deps_drivers/dahdi/xpp/xbus-core.o):
