OUTPUT_FORMAT("binary")
ENTRY(main)
SECTIONS
{
	.text 0x0000000000100000 :
	{
		*(.text)
	}
	.data :
	{
		*(.data)
	}
	.bss 0x00000000001C00000 :
	{
		*(.bss)
	}
}
