#!/bin/bash

# [vdso]
#由于进行系统调用时，操作系统要由用户态切换到内核态，而这一操作是非常浪费时间的操作，无论采用早期的int 0x80/iret中断，还是sysenter/sysexit指令，再到syscall/sysexit指令。
#另一方面，某些系统调用并不会向内核提交参数，而仅仅只是从内核里请求读取某个数据，例如gettimeofday()，内核在处理这部分系统调用时可以把系统当前时间写在一个固定的位置，
#而应用程序直接从该位置简单读取即可，无需发起系统调用。内核与用户态程序之间进行数据交互的方法就是mmap。但由于vsyscall采用固定地址映射的方式，所以存在一定的安全隐患，
#这一方式便被vdso所改进，vdso的随机映射在一定程度上缓解了安全威胁。虽然有了vdso，但从历史兼容性上来讲，vsyscall不能就此完全抛弃，
#否则将导致一些陈旧的（特别是静态连接的）应用程序无法执行，因此现在在我的3.19内核上，将同时看到vdso和vsyscal
pid=$1

if [ $# -lt 1 ];then
    echo "Usage: pid"
    exit -1
fi

function pause()
{
    read -p "Enter to continue..."
}
gatefile=gate.so

offset=$(./checkmem_size $pid | grep -w vdso | awk -F"-" '{print substr($1,0,9)}')
offset="0x$offset"
echo $offset

# 单位bs=4K
# 转储进程的内存内容
dd if=/proc/${pid}/mem of=$gatefile bs=4096 skip=$[$offset] count=2
pause

# 查看文件类型
file $gatefile
pause

# 反编译
objdump -d $gatefile
