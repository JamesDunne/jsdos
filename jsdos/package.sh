#!/bin/bash
IMGFILE="jsdos-`date +%Y%m%d`.img"
VMDIR='JSDOS VM for Windows'
VMZIP="JSDOS-`date +%Y%m%d` VM for Windows.zip"

pushd ../release &>/dev/null
cp ../build/$IMGFILE .
gzip -cfq9 $IMGFILE > $IMGFILE.gz
echo $IMGFILE.gz
cp "$IMGFILE" "$VMDIR/jsdos.img"
rm -f "$VMDIR/qemu/stderr.txt" &>/dev/null
rm -f "$VMDIR.zip" &>/dev/null
zip -r9 "$VMZIP" "$VMDIR" &>/dev/null
echo "$VMZIP"
popd &>/dev/null
