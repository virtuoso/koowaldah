#ifndef ROOTFS_CPIO
#error "Please specify the rootfs cpio archive location"
#endif

.section .rootfs, "a"
.align 4
.weak rootfs_start
.equ rootfs_start,.
.incbin ROOTFS_CPIO
.weak rootfs_end
.equ rootfs_end,.
