.extern handler
.section my_code_main
ld $handler, %r1
csrwr %r1, %handler
ld $0x1, %r1
handler:
ld $5, %r2
.end