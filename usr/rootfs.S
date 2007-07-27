.section .rootfs, "a"
.weak rootfs_start
.equ rootfs_start,.
.incbin "rootfs.cpio"
.weak rootfs_end
.equ rootfs_end,.

