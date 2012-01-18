@ECHO OFF

qemu\qemu-system-x86_64.exe -L qemu\ -m 128 -hda jsdos.img -soundhw pcspk -rtc base=localtime -M pc -smp 4,cores=4 -name "JSDOS"
