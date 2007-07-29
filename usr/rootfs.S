.section .rootfs, "a"
.align 4
.weak rootfs_start
.equ rootfs_start,.
.incbin "rootfs.cpio"
.weak rootfs_end
.equ rootfs_end,.

