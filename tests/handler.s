.extern isr_timer, isr_terminal, isr_software

.global handler
.section my_handler
handler:
  push %r1
  push %r2
  csrrd %cause, %r1
  ld $2, %r2
  beq %r1, %r2, handle_timer
  ld $3, %r2
  beq %r1, %r2, handle_terminal
  ld $4, %r2
  beq %r1, %r2, handle_software
finish:
  pop %r2
  pop %r1
  iret

handle_timer:
  call isr_timer
  jmp finish

handle_terminal:
  call isr_terminal
  jmp finish

handle_software:
  call isr_software
  jmp finish

.end